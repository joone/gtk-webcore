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
#include "BridgeImpl.h"

#include "NRCore/WebCoreResourceLoader.h"
#include "NRCore/KWIQResponse.h"

#include "osb.h"
#include "Http.h"


class ResourceLoadResponse;

class ResourceLoadStatus : public OSB::ResourceStatus
{
    friend class ResourceLoadListener;
public:
    ResourceLoadStatus(const gchar * name = 0) : ResourceStatus(name) {}
};

/**tighty coupled with BridgeImpl**/
class ResourceLoadListener : public HttpRequestListener, public WebCoreResourceHandle
{

    BridgeImpl* bridge;
    WebCoreResourceLoader* loader;
    ResourceLoadResponse* response;
    HttpRequest* request;

    ResourceLoadStatus status;
    bool resp_sent;
    bool _started;

public:

    ResourceLoadListener(BridgeImpl* b, WebCoreResourceLoader* r);
    virtual ~ResourceLoadListener();

    // HttpRequestListener
    bool header(const HttpRequest*, const HttpHeader *header);
    bool data(const HttpRequest*, const char* data, int len);
    bool started(const HttpRequest*);
    bool finished(const HttpRequest*);
    bool error(const HttpRequest*);
    bool headersEnd(const HttpRequest*);

    // WebCoreResourceHandle
    void cancel();
    
    WebCoreResourceHandle* handle(HttpRequest* req);
};


