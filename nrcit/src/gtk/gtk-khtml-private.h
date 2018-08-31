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
#ifndef __GTK_KHTML_PRIVATE_H__
#define __GTK_KHTML_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtk-khtml.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */



extern "C++"
{
#include "osb.h"
}

class MyRoot;


class MyEventListener : public OSB::FrameLoadDelegate, public OSB::UIDelegate, public OSB::ResourceLoadDelegate
{
 public:
    MyEventListener (MyRoot* root);

    ~MyEventListener();

    /** FrameLoadDelegate*/
    void onClientRedirectReceived(OSB::Frame* target, const gchar* to);
    void onServerRedirected(OSB::Frame* target, const gchar* to);

    void onFrameLoadStarted(OSB::Frame* target);
    void onFrameLoadFinished(OSB::Frame* target, int status);

    /** ResourceLoadDelegate*/
    void onResourceLoadStarted(OSB::Frame* target, const OSB::ResourceStatus *status);
    void onResourceLoadHeaders(OSB::Frame* target, const OSB::ResourceStatus *status);
    void onResourceLoadStatus(OSB::Frame* target, const OSB::ResourceStatus *status);
    void onResourceLoadFinished(OSB::Frame* target, const OSB::ResourceStatus *status);

    void onTitleChanged(OSB::Frame* target, const gchar* title);
    
    void onCommitLoad(OSB::Frame* target);
    /** UIDelegate */
    void onMouseOverChanged(OSB::Frame* target, OSB::ElementInfo* node);

    void setStatusText(OSB::Frame* target, const gchar* st);
    
    OSB::Root* createNewRoot(OSB::Frame* target, const gchar* url);

    void alertPanel(OSB::Frame* target, const gchar *message);
    bool confirmPanel(OSB::Frame* target, const gchar *message);
    bool textInputPanel(OSB::Frame* target, const gchar *prompt, const gchar *defaultText, const gchar **result);

    bool authPanel(OSB::Frame* target, const gchar *message, const gchar **user, const gchar **password );

    /* own  methods*/
    const gchar* statusText() { return status; }
    const gchar* location() { return loc; }
    const gchar* title() { return pageTitle; }
    void clearState();

 private:
    MyRoot* root;
    gchar* status;    
    gchar* loc;
    gchar* pageTitle;
    int strSz;
    
};

class MyRoot : public OSB::Root
{
    MyEventListener listener;
    GtkKHTMLLoadStatus* status;   
    GtkKHTML* gtkkhtml;   

    bool internal;
public:
    MyRoot(GtkKHTML* ) ;
    ~MyRoot();

    GtkKHTML* engine() { return gtkkhtml;}
    void setEmitInternal(bool flag) {internal = flag;}
    void internalStatusStart();
    void internalStatusStop(int status);
    void internalStatusStartItem(const OSB::ResourceStatus * status);
    void internalStatusHeadersItem(const OSB::ResourceStatus * status);
    void internalStatusProgressItem(const OSB::ResourceStatus * status);
    void internalStatusStopItem(const OSB::ResourceStatus * status);

    const GtkKHTMLLoadStatus* myStatus();
    const gchar* myLocation();
    const gchar* myTitle();
    
    gboolean findString(const gchar* text, gboolean case_sensitive, gboolean dir_down);
    
};

struct _GtkKHTMLPrivate {
    MyRoot* browser;
    
    GtkWidget* child;
    
    gchar* location;
    gchar* title;
    gchar* status_text;
    gchar* js_input_text;
    gchar* auth_username;
    gchar* auth_password;
};



#endif
