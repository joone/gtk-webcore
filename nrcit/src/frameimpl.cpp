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

#include "frameimpl.h"
#include "prefimpl.h"
#include "rootimpl.h"
#include "GLibHelpers.h"

class NullDelegate : public OSB::UIDelegate, public OSB::FrameLoadDelegate, public OSB::ResourceLoadDelegate
{
public:

};

static NullDelegate* 
nullDelegate()
{
    static NullDelegate single;
    return &single;
}

FrameImpl::FrameImpl(RootImpl* aroot, FrameImpl* parent)
    :BridgeImpl(parent)
    ,frame_d(nullDelegate())
    ,res_d (nullDelegate())
    ,ui_d(nullDelegate()) 
    ,root(aroot)
    ,_title(0)
    ,_location(0)
{
    assert(root);

    // must have settings
    initializeSettings(root->sharedSettings()); 
}

FrameImpl::~FrameImpl()
{
    if (_title) g_free(_title);
    if (_location) g_free(_location);
}


BridgeImpl* FrameImpl::createNewRoot(const gchar* url)
{
    OSB::Root* newroot = uiDelegate()->createNewRoot(this, url);
    if (newroot) {
	OSB::Frame* frame = newroot->mainFrame();
	return static_cast<FrameImpl*>(frame);
    }

    return 0;
};

BridgeImpl* FrameImpl::createChildFrame()
{
    FrameImpl* newframe =  new FrameImpl(root, this);
    newframe->setFrameLoadDelegate(frame_d);
    newframe->setUIDelegate(ui_d);
    newframe->setResourceLoadDelegate(res_d);
    return newframe;
}


void FrameImpl::setFrameLoadDelegate(OSB::FrameLoadDelegate* d)
{
    if (d == 0) frame_d = nullDelegate();
    else frame_d = d;
}

void FrameImpl::setResourceLoadDelegate(OSB::ResourceLoadDelegate* d)
{
    if (d == 0) res_d = nullDelegate();
    else res_d = d;    
}

void FrameImpl::setUIDelegate(OSB::UIDelegate* d)
{
    if (d == 0) ui_d = nullDelegate();
    else ui_d = d;
}


const gchar* FrameImpl::location()
{
    return currentURL();
}


void FrameImpl::startLoad(const char* url)
{
    loadURL(url,
	    0,			// referer:
	    false,		// reload:
	    false,		// onLoadEvent: 
	    "_self",		// target: 
	    0,		        // triggeringEvent: 
	    0,			// form: 
	    0);			// formValues
}

void FrameImpl::refresh()
{
    const gchar* url = requestedURLString();
    loadURL(url,
	    0,			// referer:
	    true,		// reload: 
	    false,		// onLoadEvent: 
	    "_self",		// target: 
	    0,		        // triggeringEvent: 
	    0,			// form: 
	    0);			// formValues:
}

void FrameImpl::stopLoad()
{
    closeURL();
}


void FrameImpl::emitFrameLoadStarted()
{
    frameLoadDelegate()->onFrameLoadStarted(this);
}

void FrameImpl::emitFrameLoadFinished(int status)
{
    frameLoadDelegate()->onFrameLoadFinished(this, status);
}

void FrameImpl::emitResourceLoadStarted(const OSB::ResourceStatus * status)
{
    resourceLoadDelegate()->onResourceLoadStarted(this, status);
}

void FrameImpl::emitResourceLoadHeaders(const OSB::ResourceStatus * status)
{
    resourceLoadDelegate()->onResourceLoadHeaders(this, status);
}

void FrameImpl::emitResourceLoadStatus(const OSB::ResourceStatus * status)
{
    resourceLoadDelegate()->onResourceLoadStatus(this, status);
}

void FrameImpl::emitResourceLoadFinished(const OSB::ResourceStatus * status)
{
    resourceLoadDelegate()->onResourceLoadFinished(this, status);
}

void FrameImpl::emitTitleChanged(const gchar* newtitle)
{
    assignToString(&_title,newtitle);
    frameLoadDelegate()->onTitleChanged(this, newtitle);
}

void FrameImpl::emitSetStatusText(const gchar* newtext)
{
    uiDelegate()->setStatusText(this, newtext);
}

void FrameImpl::emitClientRedirectReceived(const gchar* url)
{
    frameLoadDelegate()->onClientRedirectReceived(this, url);
}

void FrameImpl::emitClientRedirectCancelled(const gchar* url)
{
    frameLoadDelegate()->onClientRedirectCancelled(this, url);
}

void FrameImpl::emitServerRedirected(const gchar* url)
{
    frameLoadDelegate()->onServerRedirected(this, url);
}

void FrameImpl::emitCommitLoad()
{
    frame_d->onCommitLoad(this);
}



class ElementInfoImpl : public OSB::ElementInfo
{
    WebCoreElementInfo *info;
public:
    ElementInfoImpl(WebCoreElementInfo* ainfo):info(ainfo) {}
    ~ElementInfoImpl() {}

    gboolean selected() 
    { return info->isSelected;}
    const gchar* linkTitle()
    { return info->linkTitle; }
    const gchar*  linkLabel()
    { return info->linkLabel; }
    const gchar* linkURL()
    { return info->linkURL; }
    const gchar* linkTarget()
    { return info->linkTarget;}

    /** x = y = w = h= -1 if there's no image */
    void imageRect(int &x, int &y, int &w,int &h)
    { 
	x=info->imageRect.x; 
	y=info->imageRect.y;
	w=info->imageRect.width;
	h=info->imageRect.height;
    }

    const gchar* imageURL()
    { return info->imageURL;}

    const gchar* imageAltText()
    { return info->imageAltText;}

};

void FrameImpl::emitMouseOverChanged(WebCoreElementInfo* node)
{
    ElementInfoImpl ei(node);
    uiDelegate()->onMouseOverChanged(this, &ei);
}

void FrameImpl::emitSelectionChanged()
{
    //uiDelegate()->onSelectionChanged(this);
}

void FrameImpl::runJavaScriptAlertPanelWithMessage(const gchar* message)
{
    uiDelegate()->alertPanel(this, message);
}

bool FrameImpl::runJavaScriptConfirmPanelWithMessage(const gchar* message)
{
    return uiDelegate()->confirmPanel(this, message);
}

bool FrameImpl::runJavaScriptTextInputPanelWithPrompt(const gchar* prompt, 
						      const gchar* defaultText, 
						      gchar** result)
{
    assert(result);
    bool ret = uiDelegate()->textInputPanel(this, prompt, defaultText, result);
    return ret;
}

void FrameImpl::addMessageToConsole(const gchar* message, unsigned lineNumber, const gchar* sourceURL)
{
    //uiDelegate()->consoleMessage() ...

    g_printerr("javascript console message: '%s', line %d url %s", 
	       message,
	       lineNumber,
	       sourceURL);

}
bool FrameImpl::authenticate(const gchar* realm,
			     gchar** username,
			     gchar** password)
{
    assert(username);
    assert(password);
    bool ret = uiDelegate()->authPanel(this, realm, username, password);
    return ret;
}

GList* FrameImpl::peers()
{
    return root->peers();
}

OSB::URLCredentialStorage* FrameImpl::credentials()
{
    return root->credentials();
}

