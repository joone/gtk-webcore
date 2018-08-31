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
#ifndef frameimpl_h
#define frameimpl_h

#include "osb.h"
#include "BridgeImpl.h"

class WebCoreSettings;
class ResourceLoadStatus;

class FrameImpl : public OSB::Frame, public BridgeImpl
{
 public:
    FrameImpl(RootImpl* aroot, FrameImpl* parent);
    ~FrameImpl();
      /** 
     * OSB::Frame interface implementation 
     */
    void startLoad(const char* url);
    void stopLoad();
    void refresh();

    GtkWidget* widget() { return BridgeImpl::widget();}
    
    const gchar* title() { return _title;}
    const gchar* location();
    
    /** delegates */
    void setFrameLoadDelegate(OSB::FrameLoadDelegate* d);
    void setResourceLoadDelegate(OSB::ResourceLoadDelegate*);
    void setUIDelegate(OSB::UIDelegate*);

    OSB::FrameLoadDelegate* frameLoadDelegate() { return frame_d; }
    OSB::ResourceLoadDelegate* resourceLoadDelegate() { return res_d;}
    OSB::UIDelegate* uiDelegate() { return ui_d;  }


    BridgeImpl* createNewRoot(const gchar* url);
    BridgeImpl* createChildFrame(); // co-variant
    void runJavaScriptAlertPanelWithMessage(const gchar* message);
    bool runJavaScriptConfirmPanelWithMessage(const gchar* message);
    bool runJavaScriptTextInputPanelWithPrompt(const gchar* prompt, 
					       const gchar* defaultText, 
					       gchar** result);
    void addMessageToConsole(const gchar* message, unsigned lineNumber, const gchar* sourceURL);
    
    bool authenticate(const gchar* realm,
		      gchar** username,
		      gchar** password);
    

    void emitFrameLoadStarted();
    void emitFrameLoadFinished(int status);
    void emitResourceLoadStarted(const OSB::ResourceStatus* status);
    void emitResourceLoadHeaders(const OSB::ResourceStatus* status);
    void emitResourceLoadStatus(const OSB::ResourceStatus* status);
    void emitResourceLoadFinished(const OSB::ResourceStatus* status);
    void emitTitleChanged(const gchar* newtext);
    void emitSetStatusText(const gchar* newtext);
    void emitClientRedirectReceived(const gchar* url);
    void emitClientRedirectCancelled(const gchar* url);
    void emitServerRedirected(const gchar* url);
    void emitMouseOverChanged(WebCoreElementInfo* node);
    void emitSelectionChanged();
    void emitCommitLoad();

    // GList of BridgeImpls
    GList* peers(); 
    OSB::URLCredentialStorage* credentials();
    

 private:
    
    OSB::FrameLoadDelegate* frame_d; 
    OSB::ResourceLoadDelegate* res_d;
    OSB::UIDelegate* ui_d; 
   
    RootImpl* root;
    gchar* _title;
    gchar* _location;
};

#endif
