/*
 * Copyright (c) 2004 Nokia. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Nokia nor the names of its contributors may be
 * used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "BridgeImpl.h"

#include "NRCore/WebCoreViewFactory.h"
#include "NRCore/WebCoreSettings.h"
#include "NRCore/WebCoreResourceLoader.h"
#include "NRCore/WebCoreCache.h"
#include "NRCore/KWIQResponse.h"

#include "Http.h"
#include "XftTextRendererFactory.h"
#include "ImageRendererFactory.h"
#include "VisitedURLHistory.h"
#include "GdkXftContext.h"

#include "UnicodeImpl.h"
#include "ResourceLoadListener.h"
#include "PageLoadListener.h"

#include "osbimpl.h"
#include "GdkHelpers.h"
#include "GLibHelpers.h"


static void mapToParentWindow(GdkWindow* parent, GdkWindow* child, int&x, int&y);

extern "C" {
    static void size_allocate( GtkWidget *widget, GtkAllocation *allocation, gpointer data);
    static int expose(GtkWidget *widget, GdkEventExpose *event, gpointer data);
    static gint motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer data);
    static gint button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
    static gint button_release(GtkWidget *widget, GdkEventButton *event, gpointer data);
    static gboolean focus(GtkWidget *widget, GtkDirectionType dir, gpointer data);
    static  gboolean focus_inout_event(GtkWidget *widget, GdkEventFocus *event, gpointer data);
    static gboolean key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);
}



class FactoryInitializer
{
public:
    FactoryInitializer()
    {
	WebCoreUnicodeDigitValueFunction = UnicodeImplDigitValueFunction;
	WebCoreUnicodeDirectionFunction = UnicodeImplDirectionFunction;
	WebCoreUnicodeMirroredFunction = UnicodeImplMirroredFunction;
	WebCoreUnicodeMirroredCharFunction = UnicodeImplMirroredCharFunction;
	WebCoreUnicodeLowerFunction = UnicodeImplLowerFunction;
	WebCoreUnicodeUpperFunction = UnicodeImplUpperFunction;

	XftTextRendererFactory::useAsSharedFactory();
	ImageRendererFactory::useAsSharedFactory();
	VisitedURLHistory::useAsSharedProvider();
    }
};

static FactoryInitializer _fi;

extern "C"{
static gboolean
focus_scrolledwindow(GtkWidget *widget, GtkDirectionType dir, gpointer data)
{
#if DEBUG
    g_printerr("%s: widget:%x dir:%x data:%x \n",
	       __PRETTY_FUNCTION__,
	       (int)widget,
	       (int)dir,
	       (int)data);
#endif
    BridgeImpl *bridge = static_cast<BridgeImpl*>(data);
    GtkWidget *foundWidget = 0;

    switch (dir){
    case GTK_DIR_TAB_FORWARD:
	foundWidget = bridge->nextKeyView();
	break;
    case GTK_DIR_TAB_BACKWARD:
	foundWidget = bridge->previousKeyView();
	break;
    default:
	break;
    }
    if (foundWidget && foundWidget != widget) 
	bridge->makeFirstResponder(foundWidget);

    return foundWidget ? TRUE : FALSE; // TRUE To stop propagating,  FALSE to continue
}

static gboolean
focus_in_scrolledwindow(GtkWidget *widget, GdkEventFocus *event, gpointer data)
{
#if DEBUG
    g_printerr("%s: widget:%x data:%x in:%x\n",
	   __PRETTY_FUNCTION__,
	   (int)widget,
	   (int)data, event->in);
#endif
    
    if (event->in == FALSE) {
	// FIXME: How to unset document focus
	// BridgeImpl *bridge = static_cast<BridgeImpl*>(data);
    }

    return FALSE;
}

}

static
void freeGListOfStrings(GList* listOfStrings)
{
    if (listOfStrings) {
	GList* iter = listOfStrings;
	while (iter) {
	    if (iter->data) g_free(iter->data);
	    iter = g_list_next(iter);
	}
	g_list_free(listOfStrings);
    }
}

BridgeImpl::BridgeImpl(BridgeImpl *parent)
    :_parent(parent)
    , _childFrames(0)
    , _documentState(0)
    , _shouldReapplyStyles(false)
    , _inexpose(false)
    , _requestedURL(0)
    , _currentURL(0)
    , _generatedFrameName(0)
    , _generatedFrameNameId(0)
    , _frameName(0)
    , _activeRequest(0)
    , _isReloading(false)

{
    if (parent) { 
	_parent = parent;
	setParent(parent);
	didSetName("root frame");
    } else {
	_parent = 0;
    }

    WebCoreBridge::init();
    WebCoreCache::setDisabled(false);

    _oldCanvasSize.x = _oldCanvasSize.y =  _oldCanvasSize.width = _oldCanvasSize.height = 0;

    frameWidget = GTK_SCROLLED_WINDOW (gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_shadow_type(frameWidget, GTK_SHADOW_NONE);

    GtkAdjustment* ha = gtk_scrolled_window_get_hadjustment(frameWidget);
    GtkAdjustment* va = gtk_scrolled_window_get_vadjustment(frameWidget);
    ha->step_increment = 20;
    va->step_increment = 10;    

    frameCanvas = gtk_layout_new(gtk_scrolled_window_get_hadjustment(frameWidget),
				 gtk_scrolled_window_get_vadjustment(frameWidget));

    gtk_widget_set_double_buffered(GTK_WIDGET (frameCanvas), FALSE);
    gtk_widget_set_double_buffered(GTK_WIDGET (frameWidget), FALSE);

    gtk_widget_add_events(GTK_WIDGET (frameCanvas),
			  GDK_KEY_PRESS_MASK
			  | GDK_KEY_RELEASE_MASK
			  | GDK_BUTTON_PRESS_MASK
			  | GDK_BUTTON_RELEASE_MASK
			  | GDK_FOCUS_CHANGE_MASK
			  | GDK_POINTER_MOTION_MASK 
			  | GDK_POINTER_MOTION_HINT_MASK
			  | GDK_EXPOSURE_MASK);

    gtk_container_add(GTK_CONTAINER (frameWidget), frameCanvas);

    frameWidget.connect("focus",
			::focus_scrolledwindow,
			this);
    frameWidget.connect("focus-in-event",
			::focus_in_scrolledwindow,
			this);
    frameWidget.connect("focus-out-event",
		       ::focus_in_scrolledwindow,
			this);

    connectFrameContents();
    createKHTMLViewWithGtkWidget(GTK_WIDGET(frameWidget), 0,0);

}

void BridgeImpl::sizeAllocate(GtkWidget *widget, GtkAllocation* allocation)
{
    if (_oldCanvasSize.x == allocation->x && 
	_oldCanvasSize.y == allocation->y && 
	_oldCanvasSize.width == allocation->width &&
	_oldCanvasSize.height == allocation->height)
	return;
    
    _oldCanvasSize = *allocation;

    GtkAdjustment* ha = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(frameWidget));
    GtkAdjustment* va = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(frameWidget));

    va->page_increment = (gdouble) allocation->width;
    ha->page_increment = (gdouble) allocation->height;

    forceLayoutAdjustingViewSize(false);
    sendResizeEvent();
}

/** 
 *
 * Bridge delete semantics:
 *  on deleting top level main frame, the KHTMLPart takes care of subframes.
 *  HTMLFrameElementImpl::detach calls KHTMLPart::frameDetached, which will inform
 *  Bridge that frame is ready to be deleted
 *  Bridge then deletes itself and derefs its part.
 *  After that KHTMLPart::frameDetached removes the part KHTMLPart's subframe list
 *  and derefs the part for the last time. part gets destroyed.
 */

BridgeImpl::~BridgeImpl()
{
#ifdef DEBUG
    g_printerr("%s this:%x, part:%x, frameName:%s",
	       __PRETTY_FUNCTION__, 
	       (int)this, 
	       (int)part(),
	       _frameName);
#endif
    if (_frameName) g_free(_frameName);
    if (_requestedURL) g_free(_requestedURL);
    if (_currentURL) g_free(_currentURL);
    if (_generatedFrameName) g_free(_generatedFrameName);

    GList* iter = g_list_first(_childFrames);
    BridgeImpl* child;
    while (iter) {
	child = static_cast<BridgeImpl*>(iter->data);
	assert(child);
	child->_parent = 0;
	iter = g_list_next(iter);
    }

    g_list_free(_childFrames);
    freeGListOfStrings(_documentState);

    disconnectFrameContents();    
}

BridgeImpl* BridgeImpl::mainFrame()
{
    if (_parent) 
	return _parent->mainFrame();

    return this;
}

/** Searches for a child frame from current frame and subframes 
 * internal function
 * Searches for a child frame from current frame and subframes recursively
 * @return 0 on failure
 */
BridgeImpl* BridgeImpl::findChildFrameNamed(const gchar* frameName)
{
    if (strcmp(this->_frameName, frameName)==0)
	return this;

    BridgeImpl* result;
    GList* iter = g_list_first(_childFrames);
    BridgeImpl* child;
    while (iter) {
	child = static_cast<BridgeImpl*>(iter->data);
	assert(child);
	result = child->findChildFrameNamed(frameName);
	if (result) 
	    return result;

	iter = g_list_next(iter);
    }
    return 0;
}

/** Searches for child frames from current frame, subframes, and parent frames 
 * internal
 * First searches name from self, then childframes, except asendFrom -branch. 
 * If not found, ascens one level up in the hierarchy
 */
BridgeImpl* BridgeImpl::ascendingFindFrameNamed(const gchar* frameName, BridgeImpl* ascendFrom)
{
    // search self
    if (strcmp(this->_frameName, frameName)==0)
	return this;

    BridgeImpl* result = 0;

    // search subtrees != ascendFrom
    GList* iter = g_list_first(_childFrames);
    BridgeImpl* child;
    while (iter) {
	child = static_cast<BridgeImpl*>(iter->data);
	assert(child);

	if (child != ascendFrom) {
	    result = child->findChildFrameNamed(frameName);
	    if (result) return result;
	}
	iter = g_list_next(iter);
    }

    // search parent
    if (_parent)
	return _parent->ascendingFindFrameNamed(frameName, this);

    return 0;
}


WebCoreBridge* BridgeImpl::findFrameNamed(const gchar *frameName)
{    
    // search common names
    if (strcmp(frameName, "_top")==0)
	return mainFrame();

    if (strcmp(frameName,"_parent")==0) 
	return _parent ? _parent : this;

    if (strcmp(frameName,"_self")==0)
	return this;

    // search self
    if (this->_frameName == frameName)
	return this;

    BridgeImpl* result = 0;

    // search subframes
    result = findChildFrameNamed(frameName);
    if (result) 
	return result;

    // search upper branches
    if (_parent) {
	result = _parent->ascendingFindFrameNamed(frameName, this);
	if (result)
	    return result;
    }


    // search other peers
    BridgeImpl *thisMain = mainFrame();
    GList* myPeers = thisMain->peers();
    GList* iter = g_list_first(myPeers);
    BridgeImpl *child;

    while (iter) {
	child = static_cast<BridgeImpl*>(iter->data);
	if (thisMain != child) {  // skip this windows' branch, already searched
	    result = child->findChildFrameNamed(frameName);
	    if (result) 
		return result;
	}
	iter = g_list_next(iter);
    }

    return 0;
}

const gchar* BridgeImpl::generateFrameName()
{
    static gchar* templ = "<!-- frame: %d-->";
    // doesn't this leak one duplicated string?
    if (_generatedFrameName) g_free(_generatedFrameName);
    _generatedFrameName = g_strdup_printf(templ, _generatedFrameNameId);
    _generatedFrameNameId++;
    return _generatedFrameName;
}

void BridgeImpl::frameDetached()
{
    if (_parent) {
	_parent->_childFrames = g_list_remove(_parent->_childFrames, this);
    }
    
    delete this;
}

GtkWidget* BridgeImpl::documentView()
{
    return GTK_WIDGET (frameWidget);
}

void BridgeImpl::commitLoad()
{
    assignToString(&_currentURL, _requestedURL);
    emitCommitLoad();    
}

// methods overrides default WebCore method. 
void BridgeImpl::openURL(const gchar *_URL, 
			 bool reload, 
			 const gchar *_contentType,
			 const gchar *refresh, 
			 GTime _lastModified, 
			 KWIQPageCache* pageCache)
{
    _generatedFrameNameId = 0;
    clearFrameContents(true);

    WebCoreBridge::openURL(_URL, reload, _contentType, refresh, _lastModified, pageCache);
    VisitedURLHistory::sharedProvider()->insertVisitedURL(_URL);
}


/** 
 * call didNotOpenURL(URL)
 * if user doesn't accept the load  (popup-blocker, form submission dialog
 */

void BridgeImpl::loadURL(const gchar* URL,
			 const gchar* referrer,
			 bool reload, 
			 bool onLoadEvent,  
			 const gchar* target,
			 NSEvent *event, 
			 NRCit::DOMElement* form, 
			 GHashTable* formValues)
{
    WebCoreBridge *targetBridge = this;
    if (isEmptyString(_frameName))
	assignToString(&_frameName, target);

    if (!isEmptyString(target)) targetBridge = findFrameNamed(target);
    
    if (targetBridge && targetBridge != this) {
	targetBridge->loadURL(URL, referrer, reload, onLoadEvent, target, event, form, formValues);
	return;

    } 

    if (!URL) URL = "";

    if (!targetBridge) {
        // unknown windows should open in new window
	// target will have _blank or some custom name
	if (onLoadEvent) {
	    // FIXME: implement: settings -> block popups
	    didNotOpenURL(URL); 
	} else { 
	    mainFrame()->createWindowWithURL(URL, target);
	    return;
	}
    }

    closeURL();
    
    if (createRequest(URL, referrer, reload, HttpRequest::GET)) {    
	provisionalLoadStarted();
	emitFrameLoadStarted();
	_activeRequest->execute();
    }
}


void BridgeImpl::postWithURL(const gchar* URL, const gchar *referrer,  const gchar* target, 
			     GByteArray* data, const gchar* contentType, 
			     NSEvent *onLoadEvent, NRCit::DOMElement *form, GHashTable *formValues)
{
    WebCoreBridge *targetBridge = this;
    if (!isEmptyString(target)) targetBridge = findFrameNamed(target);
    
    if (targetBridge && targetBridge != this) {
	targetBridge->postWithURL(URL, referrer, target, data,contentType, onLoadEvent, form, formValues);
	return;
    } 

    if (!targetBridge) {
        // unknown windows should open in new window
	// target will have _blank or some custom name
	if (onLoadEvent) {		    
	    // FIXME: implement: settings -> block popups
	    didNotOpenURL(URL); 
	} else { 
	    mainFrame()->createWindowWithURL(URL, target);
	    return;
	}
    }
    _generatedFrameNameId = 0;

    if (createRequest(URL, referrer, false, HttpRequest::POST)) {
        _activeRequest->setPostData(contentType, data);
	provisionalLoadStarted();
	emitFrameLoadStarted();
	_activeRequest->execute();
    }
}


bool BridgeImpl::createRequest(const gchar* URL,  
                               const gchar* referrer,  
                               const bool reload,
                               const HttpRequest::Type method)
{   
    assignToString(&_requestedURL, URL);

    const gchar *pageURL = URL;
#if 0    
    KURL pageURL(URL);
    pageURL.setRef(QString::null);
#endif

    // the page currently viewed shouldn't be in the cache
    PageCacheEntry* cacheEntry = pageCache.take(pageURL);

    if (reload) {
	_isReloading = true;
	delete cacheEntry;
	cacheEntry = 0;
    }
    
    if (isEmptyString(pageURL) || strcmp(pageURL,"about:blank")==0) {
	createKHTMLViewWithGtkWidget(GTK_WIDGET(frameWidget), 0, 0);
	loadEmptyDocumentSynchronously();
	return false;
    }


    if (cacheEntry) { 
	delete cacheEntry;
	// disabled for the moment
#if 0
	emitFrameLoadStarted();

	clearFrameContents(false);	
	openURL(URL, false /*reload*/, "text/html", "", QDateTime(), cacheEntry);
	emitTitleChanged(cacheEntry->title().utf8());
	delete cacheEntry;
	
	installInFrame(GTK_WIDGET (frameWidget));

	loadURLFinished(false);
	return false;
#endif
    }

    if (_activeRequest)  delete _activeRequest;
    HttpRequestListener* listener = new PageLoadListener(this, pageURL);    
    _activeRequest = HttpFactory().createRequest(listener, credentials(), pageURL, method);

    if (_activeRequest) {
	if (!isEmptyString(referrer))
	    _activeRequest->setReferrer(referrer);
	
	if (reload)
	    _activeRequest->setForceReload(true);

        _activeRequest->setUserAgent(userAgentForURL(URL));
        return true;
    }

    g_printerr("%s could not create request for url: %s\n", __PRETTY_FUNCTION__, URL);
    return false;    
}

WebCoreBridge* BridgeImpl::createWindowWithURL(const gchar* URL, const gchar *name)
{

    BridgeImpl* newbridge = createNewRoot(URL);
    
    if (!newbridge) return 0;
        
    if (strcmp(name, "_blank")!=0 && strcmp(name, "_new")!=0)
	newbridge->setName(name);

    return newbridge;
}

void BridgeImpl::showWindow()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

const gchar* BridgeImpl::userAgentForURL(const gchar* URL)
{
    // FIXME: this should be dynamic. -- psalmi
    // (includes apple webkit for better compliancy with sites
    //  targetted to Apple Safari).
    return "Mozilla/5.0 (X11; U; Linux i686; en-us) AppleWebKit/146.1 (KHTML, like Gecko) GtkWebCore/0.5.0a";
}

void BridgeImpl::setTitle(const gchar* title)
{
    emitTitleChanged(title);
}

void BridgeImpl::setStatusText(const gchar* status)
{
    emitSetStatusText(status);
}


void BridgeImpl::setIconURL(const gchar* URL)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

void BridgeImpl::setIconURL(const gchar* URL, const gchar* name)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}


WebCoreBridge* BridgeImpl::createChildFrameNamed(const gchar* frameName,
						  const gchar* URL, 
						  KHTMLRenderPart *renderPart,
						  bool allowsScrolling, 
						  int marginWidth, 
						  int marginHeight)
{
    assert(renderPart);

    BridgeImpl* newbridge = createChildFrame();

    newbridge->didSetName(frameName);  // name is aready in name list of the caller. Just set our name
    newbridge->createKHTMLViewWithGtkWidget(GTK_WIDGET (frameWidget), marginWidth, marginHeight);

    newbridge->setRenderPart(renderPart);
    newbridge->installInFrame(GTK_WIDGET (newbridge->frameWidget));

    _childFrames = g_list_append(_childFrames, newbridge);

    if (allowsScrolling == false) {
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (newbridge->frameWidget), 
				       GTK_POLICY_NEVER, 
				       GTK_POLICY_NEVER);
    }

    newbridge->loadURL(URL, referrer(), false, false, frameName, 0, 0, 0);
    return newbridge;
}

bool BridgeImpl::areToolbarsVisible()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);    
    return true;
}
void BridgeImpl::setToolbarsVisible(bool visible)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);    
}
bool BridgeImpl::isStatusBarVisible()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return true;
}

void BridgeImpl::setStatusBarVisible(bool visible)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

bool BridgeImpl::areScrollbarsVisible()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return true;
}

void BridgeImpl::setScrollbarsVisible(bool visible)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

#if 0
NSWindow *BridgeImpl::window()
{
}
#endif

void BridgeImpl::setWindowFrame(GdkRectangle* frame)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

void BridgeImpl::windowFrame(GdkRectangle* outFrame)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

void BridgeImpl::setWindowContentRect(GdkRectangle* frame)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}


void BridgeImpl::windowContentRect(GdkRectangle* outRect)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    fillGdkRectangle(outRect, 0, 0, 0, 0);
}

void BridgeImpl::setWindowIsResizable(bool resizable)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

bool BridgeImpl::windowIsResizable()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return true;
}

GtkWidget *BridgeImpl::firstResponder()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return 0;
}

void BridgeImpl::makeFirstResponder(GtkWidget *view)
{
    if (!gtk_widget_is_focus(view))
	gtk_widget_grab_focus(view);    
}

void BridgeImpl::closeWindowSoon()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

WebCoreResourceHandle* BridgeImpl::startLoadingResource(WebCoreResourceLoader *loader, 
							const gchar* URL, 
							GHashTable* headers)
{    
    const gchar* realURL = URL;

    ResourceLoadListener* listener = new ResourceLoadListener(this, loader);
    listener->retain(); // fixme: leaks

    HttpRequest *req = HttpFactory().createRequest(listener, credentials(), realURL, HttpRequest::GET);
    req->setUserAgent(userAgentForURL(URL));
    req->execute();


    return listener->handle(req);
}

WebCoreResourceHandle* BridgeImpl::startLoadingResource(WebCoreResourceLoader *loader, 
							const gchar* URL, 
							GHashTable* headers,
							GByteArray* data)
{
    return 0;
}

void BridgeImpl::objectLoadedFromCacheWithURL(const gchar* URL, KWIQResponse* response, unsigned bytes)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

bool BridgeImpl::isReloading()
{
    return _isReloading;
}

void BridgeImpl::reportClientRedirectToURL(const gchar* URL, 
					   double seconds, 
					   GTime date, 
					   bool lockHistory, 
					   bool isJavaScriptFormAction)
{
    emitClientRedirectReceived(URL);
}

void BridgeImpl::reportClientRedirectCancelled(bool cancelWithLoadInProgress)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    emitClientRedirectCancelled("");
}


void BridgeImpl::focusWindow()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

void BridgeImpl::unfocusWindow()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

GtkWidget* BridgeImpl::nextKeyViewOutsideWebFrameViews()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return 0;
}

GtkWidget* BridgeImpl::previousKeyViewOutsideWebFrameViews()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return 0;
}


bool BridgeImpl::defersLoading()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return false;
}

void BridgeImpl::setDefersLoading(bool loading)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

void BridgeImpl::saveDocumentState(GList* documentState)
{
    freeGListOfStrings(_documentState);
    _documentState = 0;
    GList* iter = documentState;
    while(iter) {
	_documentState = g_list_append(_documentState, g_strdup((gchar*)iter->data));
	iter = g_list_next(iter);
    }
}

GList* BridgeImpl::documentState()
{
    return _documentState;
}

void BridgeImpl::setNeedsReapplyStyles()
{
    _shouldReapplyStyles = true;
}



const gchar* BridgeImpl::requestedURLString()
{
    return _requestedURL;
}


const gchar* BridgeImpl::incomingReferrer()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return "";
}


bool BridgeImpl::saveDocumentToPageCache(KWIQPageState *documentInfo)
{
    //FIXME: query title from DOM
    pageCache.insertByURL(_currentURL, documentInfo, "");
    return true;
}

int BridgeImpl::getObjectCacheSize()
{
    return 1000;
}

bool BridgeImpl::frameRequiredForMIMEType(const gchar* MIMEType, const gchar* URL)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);    
    return false;
}
	

void BridgeImpl::loadEmptyDocumentSynchronously()
{
    provisionalLoadStarted();
    openURL("about:blank", false, "text/html", "", 0/*timeval*/, 0);
    setEncoding("latin1", true);
    addData(0, 0);
    end();
}

const gchar* BridgeImpl::MIMETypeForPath(const gchar* path)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);    
    return "text/html";
}

void BridgeImpl::handleMouseDragged(NSEvent *event)
{
    g_warning("NotYetImplemented: %s", __PRETTY_FUNCTION__);
}

void BridgeImpl::handleAutoscrollForMouseDragged(NSEvent *event)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

bool BridgeImpl::mayStartDragWithMouseDragged(NSEvent *event)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return false;
}


int BridgeImpl::historyLength()
{
    return history().length();
}

void BridgeImpl::goBackOrForward(int distance)
{
    int to = history().pos() + distance;
    if (to >=0 && to < history().length()) {
	history().setPos(to);
	history().lock();
	const gchar* url = history().current();
	loadURL(url,
		"",		     
		false,  	// reload: NO
		false,          // onLoadEvent: onLoad
		"_self",	// target: nil
		0,		// triggeringEvent: nil
		0,		// form: nil
		0);             // formValues
    }
}

// notice: if we have GtkLayout inside GtkScrollView, the 
// widget == GtkLayout, 
// widget->window == GtkLayout's outer window ("viewport")
// event->window == GtkLayout's inner window (big window, global coords)

gint BridgeImpl::buttonPress(GtkWidget *widget, GdkEventButton *event)
{
    GdkEventButton viewportEvent = *event;
    gint x = (gint)event->x, y = (gint)event->y;
    mapToParentWindow(widget->window, event->window, x, y);
    viewportEvent.x = (gdouble)x;
    viewportEvent.y = (gdouble)y;
    mouseDown((GdkEvent*)&viewportEvent);
    return TRUE;
}

gint BridgeImpl::buttonRelease(GtkWidget *widget, GdkEventButton *event)
{
    GdkEventButton viewportEvent = *event;
    gint x = (gint)event->x, y = (gint)event->y;
    mapToParentWindow(widget->window, event->window, x, y);
    viewportEvent.x = (gdouble)x;
    viewportEvent.y = (gdouble)y;
    mouseUp((GdkEvent*)&viewportEvent);
    return TRUE;
}

gint BridgeImpl::motionNotify(GtkWidget *widget, GdkEventMotion* event)
{
    GdkModifierType state = (GdkModifierType) event->state;
    gint x = (gint) event->x, y = (gint)event->y;

    if (event->is_hint) 
	gdk_window_get_pointer(event->window, &x, &y, &state);

    gint viewportx = x;
    gint viewporty = y;

    // hackish workaround because we want coordinates in viewport coordinates, not layout coordinates 
    GdkEventMotion viewportEvent = *event;
    viewportEvent.state = state;
    mapToParentWindow(widget->window, event->window, viewportx, viewporty);
    viewportEvent.x = (gdouble)viewportx;
    viewportEvent.y = (gdouble)viewporty;
    mouseMoved(&viewportEvent);

    WebCoreElementInfo info;
    memset(&info, 0, sizeof(info));
    elementAtPoint(x, y, &info); 

    emitMouseOverChanged(&info);    

#define FREE_IF_VALID(x) if ((x)) g_free((x));
    FREE_IF_VALID(info.linkTitle);
    FREE_IF_VALID(info.linkLabel);
    FREE_IF_VALID(info.linkURL);
    FREE_IF_VALID(info.linkTarget);
    FREE_IF_VALID(info.imageURL);
    FREE_IF_VALID(info.imageAltText);
#undef FREE_IF_VALID

    return TRUE;
}


gint BridgeImpl::expose(GtkWidget *widget, GdkEventExpose *event)
{
    if (_inexpose) 
	return FALSE;    

    _inexpose = true;

    GdkRectangle exposeRect = event->area;

    if (hasStaticBackground()) {
	GtkAdjustment* ha = gtk_layout_get_hadjustment(GTK_LAYOUT(frameCanvas));
	GtkAdjustment* va = gtk_layout_get_vadjustment(GTK_LAYOUT(frameCanvas));
	GtkAllocation* a = &frameCanvas->allocation;

	fillGdkRectangle(&exposeRect, (int)ha->value, (int)va->value, (int)ha->value + a->width, (int)va->value + a->height);
    }

    if (_shouldReapplyStyles) {
	_shouldReapplyStyles = false;
	reapplyStylesForDeviceType(WebCoreDeviceScreen);
    }

    GdkRegion* paintRegion = gdk_region_rectangle(&exposeRect);    
    gdk_window_begin_paint_region(event->window, paintRegion);

    GdkXftContext gc(this, GTK_LAYOUT(frameCanvas)->bin_window);

    drawRect(&exposeRect, &gc);

    gdk_window_end_paint(event->window);
    gdk_region_destroy(paintRegion);    

    _inexpose = false;
    return TRUE;    
}

extern "C" {
static void _remove_child(GtkWidget* child, gpointer data)
{
    GtkContainer* container = GTK_CONTAINER (data);
    assert(container);
    assert(child); 
    gtk_container_remove(container, child);
#ifdef DEBUG
    g_printerr("removed child %x\n", (int) child);
#endif
}
}

void BridgeImpl::clearFrameContents(bool createView)
{
#ifdef DEBUG
    g_printerr("clear frame contents \n");
#endif
    gtk_container_foreach(GTK_CONTAINER (frameCanvas), _remove_child, GTK_CONTAINER (frameCanvas));
#ifdef DEBUG
    g_printerr("clear frame contents end\n");
#endif
    if (createView)
	createKHTMLViewWithGtkWidget(GTK_WIDGET(frameWidget), 0,0);
}

void BridgeImpl::disconnectFrameContents()
{
    frameCanvas = 0;
}

void BridgeImpl::connectFrameContents()
{
    frameCanvas.connect("expose_event",
			::expose,
			this);
    frameCanvas.connect("motion_notify_event",
			::motion_notify,
			this);

    frameCanvas.connect("button_press_event",
			::button_press,
			this);
    frameCanvas.connect("button_release_event",
			::button_release,
			this);

    frameCanvas.connect("focus",
			::focus,
			this);	
    frameCanvas.connect("focus-in-event",
			::focus_inout_event,
			this);
    frameCanvas.connect("focus-out-event",
		       ::focus_inout_event,
			this);
    frameCanvas.connect("key-press-event",
			::key_press,
			this);
    frameCanvas.connect("key-release-event",
			::key_press,
			this);

    frameCanvas.connect("size_allocate",
			::size_allocate,
			this);

    gtk_widget_hide(GTK_WIDGET (frameWidget));
    gtk_widget_show_all(GTK_WIDGET (frameWidget));
}

void BridgeImpl::setHasBorder(bool hasBorder)
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

WebCoreKeyboardUIMode BridgeImpl::keyboardUIMode()
{
    return WebCoreKeyboardUIMode (WebCoreKeyboardAccessFull | WebCoreKeyboardAccessTabsToLinks);
};

void BridgeImpl::didSetName(const gchar* name)
{
    assignToString(&_frameName, name);
    g_warning("%s %s %s", __PRETTY_FUNCTION__, name,_frameName);

}

void BridgeImpl::loadURLFinished(bool hasError)
{  
    delete _activeRequest;
    _activeRequest = 0;

    end();

    _isReloading = false;

    history().unlock();    // unlock the history


    scrollToAnchor(_requestedURL);

}

GtkWidget* BridgeImpl::widgetForPluginWithURL(const gchar* URL,
					      GList* attributesArray,
					      const gchar* baseURLString,
					      const gchar *MIMEType)
{
    
#if 0
    OSB::ContentRepresentationFactory *reprf = closestMatchForMIME(mainFrame()->reprForMIME, MIMEType);

    if (reprf) {
	OSB::ContentRepresentation *repr = reprf->create(MIMEType.latin1());
	return new PluginWidget(reprf, repr);
    }
#endif
    gchar str[100];
    g_snprintf(str, 100, "plugin for mimeType: %s", MIMEType);
    GtkWidget *b = gtk_label_new(str);

    return b;
}

#if 0
const gchar* BridgeImpl::renderTreeString()
{
    return _renderTree_u8 = renderTreeAsExternalRepresentation();
}
#endif

void BridgeImpl::print()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

void BridgeImpl::issueCutCommand()
{
   g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
   assert(0);
}

void BridgeImpl::issueCopyCommand()
{
   g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
   assert(0);
}
void BridgeImpl::issuePasteCommand()
{ 
   g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
   assert(0);
}

void BridgeImpl::respondToChangedSelection()
{
    emitSelectionChanged();
}

void BridgeImpl::respondToChangedContents()
{
   g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}



/** other methods */


GtkWidget* BridgeImpl::widget()
{
    return GTK_WIDGET(frameWidget);
}

void BridgeImpl::saveHistoryItem()
{
  if (!isReloading()) {
    VisitedURLHistory::sharedProvider()->insertVisitedURL(requestedURLString());
    history().pushNew(requestedURLString());
  }
}

SimpleHistory& BridgeImpl::history()
{
    return _history;
}

void BridgeImpl::setFrameName(const gchar* frameName)
{
    
}


const gchar* BridgeImpl::overrideMediaType()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
    return "";
}
void  BridgeImpl::windowObjectCleared()
{
    g_warning("NotYetImplemented: %s",__PRETTY_FUNCTION__);
}

bool BridgeImpl::canGoBack()
{
    return history().pos()>0;
}

bool BridgeImpl::canGoForward()
{
    return history().pos() < (history().length()-1);
}

void BridgeImpl::goForward()
{
    if (canGoForward()) history().next();
    const gchar* url = history().current();
    history().lock();
    loadURL(url,
	    "",			// referer:
	    false,		// reload: 
	    false,		// onLoadEvent: 
	    "_self",		// target:
	    0,		        // triggeringEvent: 
	    0,			// form: 
	    0);			// formValues:
}

void BridgeImpl::goBack()
{
    if (canGoBack()) history().prev();
    const gchar* url = history().current();
    history().lock();

    loadURL(url,
	    "",			// referer
	    false,		// reload:
	    false,		// onLoadEvent:
	    "_self",		// target: 
	    0,		        // triggeringEvent: 
	    0,			// form: 
	    0);			// formValues:
}

void BridgeImpl::addContentRepresentation(OSB::ContentRepresentationFactory* reprf, const char* mimeType)
{
    
}

void BridgeImpl::regionExpiresAt(GTimeVal* moment, GdkRectangle* rect, CGContext* caller)
{
    // use this for animations
}

void BridgeImpl::redirection(const gchar* url)
{
    assignToString(&_currentURL, url);
    emitServerRedirected(_currentURL);
}

void BridgeImpl::changeSettingsDescendingToChildren(WebCoreSettings* s)
{
    assert(s);
    initializeSettings(s);
    GList* iter = g_list_first(_childFrames);
    BridgeImpl* child;
    while (iter) {
	child = static_cast<BridgeImpl*>(iter->data);
	assert(child);	
	child->initializeSettings(s);
	iter= g_list_next(iter);
    }
}

GList* BridgeImpl::childFrames()
{
    return _childFrames;
}

extern "C" {
static void
size_allocate( GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
#if DEBUG
    g_printerr("%s widget:%x, data:%x new_dim:(%d,%d;%d,%d)\n", 
	       __PRETTY_FUNCTION__, 
	       (int)widget,
	       (int)data,
	       allocation->x, allocation->y, allocation->width, allocation->height);

#endif

    BridgeImpl* self = static_cast<BridgeImpl*>(data);
    assert(self);

    self->sizeAllocate(widget, allocation);
}

static int
expose(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
#if DEBUG
    g_printerr("%s widget:%x event:%x, data:%x (window %x, x:%d,y:%d,w:%d,h:%d)\n",
	   __PRETTY_FUNCTION__, 
	       (int)widget,
	       (int)event,
	       (int)data,
	       (int)event->window,
	       event->area.x,
	       event->area.y,
	       event->area.width,
	       event->area.height);    
#endif

    BridgeImpl *bridge = static_cast<BridgeImpl*>(data);
    return bridge->expose(widget, event);
}



static gint 
motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    BridgeImpl *self = static_cast<BridgeImpl*>(data);
    return self->motionNotify(widget, event);
}

static gint
button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    BridgeImpl *self = static_cast<BridgeImpl*>(data);
    return self->buttonPress(widget, event);
}

static gint
button_release(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    BridgeImpl *self = static_cast<BridgeImpl*>(data);
    return self->buttonRelease(widget, event);
}

static gboolean
focus(GtkWidget *widget, GtkDirectionType dir, gpointer data)
{
#if DEBUG
    g_printerr("%s: widget:%x dir:%x data:%x \n",
	       __PRETTY_FUNCTION__,
	       (int)widget,
	       (int)dir,
	       (int)data);
#endif
    return FALSE;
}

static  gboolean
focus_inout_event(GtkWidget *widget, GdkEventFocus *event, gpointer data)
{
#if DEBUG
    g_printerr("%s: widget:%x event:%x data:%x \n",
	   __PRETTY_FUNCTION__,
	       (int)widget,
	       (int)event,
	       (int)data);
#endif
    
    return FALSE;
}

static gboolean
key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
#if DEBUG
    g_printerr("%s: widget %x, data:%x, event->keyval %d",
	       __PRETTY_FUNCTION__, 
	       (int)widget, 
	       (int)data, 
	       (int)event->keyval);
#endif

    return FALSE;
}

}

static 
void mapToParentWindow(GdkWindow* parent, GdkWindow* child, int&x, int&y)
{
    int nx, ny;
    while (child != parent) {
	gdk_window_get_position (child, &nx, &ny);
	x += nx;
	y += ny;
	child = gdk_window_get_parent (child);
	assert(child);
    }    
}


/**
   Local Variables:   
   c-basic-offset: 4
   End:
 */
