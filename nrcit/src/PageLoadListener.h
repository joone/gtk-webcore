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
#ifndef PageLoadListener_h
#define PageLoadListener_h

#include "BridgeImpl.h"
#include "Http.h"
#include "osb.h"


class PageLoadStatus : public OSB::ResourceStatus
{
    friend class PageLoadListener;
public:
    PageLoadStatus(const gchar * name = 0) : ResourceStatus(name) {}
};

class PageLoadListener : public HttpRequestListener 
{
    BridgeImpl* bridge;
    gchar* url;
    gchar* contentType;
    gchar* encoding;
    gchar* refresh;
    GTime lastModified;


    bool done_url;
    bool recvd_enc;
    bool has_error;
    bool auth_attempted;
    bool commited;

    PageLoadStatus status;
    
public:
    PageLoadListener(BridgeImpl* b, const gchar* URL);    
    ~PageLoadListener();
    
    bool header(const HttpRequest*, const HttpHeader *header);
    bool data(const HttpRequest*, const char* data, int len);
    bool started(const HttpRequest*);
    bool finished(const HttpRequest*);
    bool error(const HttpRequest*);
    
    bool authenticate(HttpRequest* request);
    void doOpenURL();

   
};
#endif
