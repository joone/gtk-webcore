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
#ifndef BRIDGEIMPL_H
#define BRIDGEIMPL_H

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "NRCore/WebCoreBridge.h" // WebCoreBridge

#include "osb.h" // OSB::Preferencs, OSB::CredentialStorage

#include "PageCache.h"     // PageCache
#include "SimpleHistory.h" // SimpleHistory
#include "Http.h"          // HttpRequest
#include "CGContextProvider.h" // CGContextProvider
#include "GObjectMisc.h"

class EventForwarder;
class HttpRequest;
class KWIQResponse;
class KWIQPageState;
class SimpleHistory;

class BridgeImpl : public WebCoreBridge, public CGContextProvider
{
    
 public:
    
    BridgeImpl(BridgeImpl *parent = 0);
    ~BridgeImpl();

    /** returns list of childframes for this frame
     *@return GList of \BridgeImpl* which are the childframes this frame */
    GList* childFrames(); 
    BridgeImpl* mainFrame(); //co-variant
    
    WebCoreBridge* findFrameNamed(const gchar* name);
    
    /** Creates a name for an frame unnamed in the HTML.  
     * It should produce repeatable results for loads of the same frameset.  */
    const gchar* generateFrameName(); 

    void frameDetached();
    
    GtkWidget* documentView();

    void loadURL(const gchar* URL, const gchar *referrer, bool reload, bool onLoadEvent, const gchar* target, NSEvent *event, NRCit::DOMElement* form, GHashTable* formValues);
    void postWithURL(const gchar* URL, const gchar *referrer,  const gchar *target, GByteArray* data, const gchar* contentType, NSEvent *event, NRCit::DOMElement *form, GHashTable *formValues);

    WebCoreBridge* createWindowWithURL(const gchar* URL, const gchar* name);
    void showWindow();

    const gchar* userAgentForURL(const gchar* URL);

    void setTitle(const gchar* title);
    void setStatusText(const gchar* status);

    void setIconURL(const gchar* URL);
    void setIconURL(const gchar* URL, const gchar* string);

    WebCoreBridge* createChildFrameNamed(const gchar* frameName, 
				      const gchar* URL,
				      KHTMLRenderPart *renderPart,
				      bool allowsScrolling, 
				      int marginWidth, 
				      int marginHeight);


    bool areToolbarsVisible();
    void setToolbarsVisible(bool visible);
    bool isStatusBarVisible();
    void setStatusBarVisible(bool visible);
    bool areScrollbarsVisible();
    void setScrollbarsVisible(bool visible);

    void setWindowFrame(GdkRectangle* frame);
    void windowFrame(GdkRectangle* outFrame);
    void setWindowContentRect(GdkRectangle* frame);
    void windowContentRect(GdkRectangle* outFrame);

    void setWindowIsResizable(bool resizable);
    bool windowIsResizable();

    GtkWidget *firstResponder();
    void makeFirstResponder(GtkWidget *view);

    void closeWindowSoon();

    WebCoreResourceHandle* startLoadingResource(WebCoreResourceLoader *loader, 
						const gchar* URL, 
						GHashTable* headers, 
						GByteArray* requestBody);
    WebCoreResourceHandle* startLoadingResource(WebCoreResourceLoader *loader,
						const gchar* URL, 
						GHashTable* headers);
    void objectLoadedFromCacheWithURL(const gchar* URL, KWIQResponse* response, unsigned bytes);
#if 0
    NSData* syncLoadResourceWithURL(NSURL *URL,
					    NSDictionary *requestHeaders, 
					    NSData *postData, 
					    NSURL **finalNSURL, 
					    NSDictionary **responseHeaderDict, 
					    int *statusCode);
#endif


    bool isReloading();

    void reportClientRedirectToURL(const gchar *URL, 
					   double seconds, 
					   GTime date, 
					   bool lockHistory, 
					   bool isJavaScriptFormAction);
    void reportClientRedirectCancelled(bool cancelWithLoadInProgress);

    void focusWindow();
    void unfocusWindow();

    GtkWidget* nextKeyViewOutsideWebFrameViews();
    GtkWidget* previousKeyViewOutsideWebFrameViews();

    bool defersLoading();
    void setDefersLoading(bool loading);
    void saveDocumentState(GList* documentState);
    GList* documentState();


    void setNeedsReapplyStyles();

    const gchar* requestedURLString();
    const gchar* incomingReferrer();

    GtkWidget* widgetForPluginWithURL(const gchar* URL,
				      GList* attributesArray,
				      const gchar* baseURLString,
				      const gchar* MIMEType);
    
    bool saveDocumentToPageCache(KWIQPageState *documentInfo);

    int getObjectCacheSize();

    bool frameRequiredForMIMEType(const gchar* mimeType, const gchar* URL);

    void loadEmptyDocumentSynchronously();

    const gchar* MIMETypeForPath(const gchar* path);

    void handleMouseDragged(NSEvent *event);
    void handleAutoscrollForMouseDragged(NSEvent *event);
    bool mayStartDragWithMouseDragged(NSEvent *event);

    int historyLength();
    void goBackOrForward(int distance);

    void setHasBorder(bool hasBorder);
    WebCoreKeyboardUIMode keyboardUIMode();
    void didSetName(const gchar* name);

    void print();
    
    void issueCutCommand();
    void issueCopyCommand();
    void issuePasteCommand();    
    void respondToChangedSelection();
    void respondToChangedContents();

    virtual const gchar* overrideMediaType();
    virtual void windowObjectCleared();    

    /** own methods */
    void saveHistoryItem();
    
    gint expose( GtkWidget *widget, GdkEventExpose *event);
    gint motionNotify(GtkWidget *widget, GdkEventMotion *event);
    gint buttonPress(GtkWidget *widget, GdkEventButton *event);
    gint buttonRelease(GtkWidget *widget, GdkEventButton *event);
    void sizeAllocate(GtkWidget *widget, GtkAllocation *alloc);
    
    void loadURLFinished(bool hasError);
    const gchar* renderTreeString();

    virtual void emitFrameLoadStarted()=0;
    virtual void emitFrameLoadFinished(int status)=0;
    virtual void emitResourceLoadStarted(const OSB::ResourceStatus* status)=0; 
    virtual void emitResourceLoadHeaders(const OSB::ResourceStatus* status)=0;
    virtual void emitResourceLoadStatus(const OSB::ResourceStatus* status)=0;
    virtual void emitResourceLoadFinished(const OSB::ResourceStatus* status)=0;
    virtual void emitTitleChanged(const gchar* newtext)=0;
    virtual void emitSetStatusText(const gchar* newtext)=0;
    virtual void emitClientRedirectReceived(const gchar* url)=0;
    virtual void emitClientRedirectCancelled(const gchar* url)=0;
    virtual void emitServerRedirected(const gchar* url)=0;
    virtual void emitMouseOverChanged(WebCoreElementInfo* node)=0;
    virtual void emitSelectionChanged()=0;
    virtual void emitCommitLoad()=0;
    
    SimpleHistory& history();
  
    virtual BridgeImpl* createChildFrame()=0;
    virtual BridgeImpl* createNewRoot(const gchar* url)=0;
    /** misc */
    void clearFrameContents(bool createView);
    
    void setFrameName(const gchar* name);

    bool canGoBack();
    bool canGoForward();
    void goForward();
    void goBack();
    void addContentRepresentation(OSB::ContentRepresentationFactory* reprf, const char* mimeType);
    
    /** gets list of main frame objects that can refer to same context
     * @return list of BridgeImpl* */
    virtual GList* peers() = 0; 
    virtual OSB::URLCredentialStorage* credentials() =0;     /** Returns credentials stored in browser. */
    
    /** called when user authentication needed */

     virtual bool authenticate(const gchar *realm,
			       gchar** username,
			       gchar** password)=0;  

    void regionExpiresAt(GTimeVal* moment, GdkRectangle* rect, CGContext* caller);
    void redirection(const gchar* url);

    void changeSettingsDescendingToChildren(WebCoreSettings* s);


    void openURL(const gchar *_URL, 
		 bool reload, 
		 const gchar *_contentType,
		 const gchar *refresh, 
		 GTime _lastModified, 
		 KWIQPageCache* pageCache);

      
    void commitLoad();
    const gchar* currentURL() { return _currentURL; }
 protected:
     GtkWidget* widget();
 private:

    /** Creates and prepares a new request for execution. */
     bool createRequest(const gchar* URL, const gchar* referrer, const bool reload, const HttpRequest::Type method);
    
    BridgeImpl* findChildFrameNamed(const gchar* frameName);
    BridgeImpl* ascendingFindFrameNamed(const gchar* frameName, BridgeImpl* ascendFrom);
    void connectFrameContents();
    void disconnectFrameContents();
    gchar* styleSheetForPrinting();

    
    BridgeImpl* _parent;
    GList*  _childFrames; 
    GList* _documentState;

    GPtr<GtkScrolledWindow> frameWidget;
    GPtr<GtkWidget> frameCanvas;

    bool _shouldReapplyStyles;    
    bool _inexpose;
    
    GtkAllocation _oldCanvasSize;

    gchar* _requestedURL;
    gchar* _currentURL;    

    gchar* _generatedFrameName;     // return value, has to be dupped
    int _generatedFrameNameId;
    gchar* _frameName;

    HttpRequest *_activeRequest;
    
    PageCache pageCache;
    SimpleHistory _history;
    
    bool _isReloading;
};

#endif
