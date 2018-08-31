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
#include "ResourceLoadListener.h"

#include "GLibHelpers.h"

class ResourceLoadResponse : public KWIQResponse
{
    gchar* _mimeType;
    gchar* _headers;
 public:
    ResourceLoadResponse() 
	:_mimeType(0), _headers(0) 
    {
    }

    ~ResourceLoadResponse() 
    { 
	if (_mimeType) g_free(_mimeType);
	if (_headers) g_free(_headers);
#ifdef DEBUG
	g_printerr("%s :%x\n",__PRETTY_FUNCTION__, (int)(this));
#endif
    }
    
   
    void setMimeType(const gchar* mime)
    { 
	assignToString(&_mimeType, mime);
    }

    const gchar* getMimeType() 
    { 
	return _mimeType;
    }

    const gchar** allHeaderFields()
    {
	return 0;
    }

    const gchar* headers()
    { 
	return 0;
    }

    GTime expiresTime() 
    { 
	return 0; 
    }
};


/** idea of ResourceLoader is to hold WebCoreResourceLoader until corresponding handle is released.
 * after that, WebCoreResourceLoader is released
 */
ResourceLoadListener::ResourceLoadListener(BridgeImpl* b, WebCoreResourceLoader* aloader)
    :bridge(b)
    ,loader(aloader)
    ,response(new ResourceLoadResponse)
    ,request(0)
    ,status(0)
    ,resp_sent(false)
    ,_started(false)    
{
    assert(bridge);
    assert(loader);
    loader->retain();
}

ResourceLoadListener::~ResourceLoadListener() 
{
#ifdef DEBUG
	g_printerr("%s :%x\n",__PRETTY_FUNCTION__, (int)(this));
#endif
}
    
bool ResourceLoadListener::header(const HttpRequest* r, const HttpHeader *header)
{
    switch (header->type()) {
    case HttpHeader::ContentType:
    {
	const HttpHeaderContentType *ct
	    = static_cast<const HttpHeaderContentType*>(header);
	response->setMimeType(ct->contentType());	    
	    
	return true;
    }
    case HttpHeader::ContentLength:
    {
	const HttpHeaderContentLength *cl
	    = static_cast<const HttpHeaderContentLength*>(header);
        status.setSize(cl->contentLength());
        return true;
    }
    case HttpHeader::Invalid:
    default:
    {
	return false;
    }
    }
}

bool ResourceLoadListener::data(const HttpRequest*, const char* data, int len) 
{
    assert(resp_sent);
	
    loader->addData(data, len);
    status.addReceived(len);
    bridge->emitResourceLoadStatus(&status);
    return true;
}

bool ResourceLoadListener::started(const HttpRequest*) 
{
    assert(!_started);
    _started=true;
    bridge->emitResourceLoadStarted(&status);
    return true;
}

bool ResourceLoadListener::finished(const HttpRequest*)	
{
    assert(loader);

    loader->finish();
    loader->release();
    loader=0;

    bridge->emitResourceLoadFinished(&status);
    if ( bridge->numPendingOrLoadingRequests()==0) {
	bridge->emitFrameLoadFinished(0);
    } else {
#ifdef DEBUG
	g_printerr("%s pending loads: %d\n",__PRETTY_FUNCTION__, bridge->numPendingOrLoadingRequests());
#endif
    }

    return true;
}
    
bool ResourceLoadListener::error(const HttpRequest*)
{
    status.setError();
    loader->reportError();
    return true;
}

bool ResourceLoadListener::headersEnd(const HttpRequest*)
{
    assert(loader);
    assert(!resp_sent);
    resp_sent = true; 
    loader->receivedResponse(response);
    bridge->emitResourceLoadHeaders(&status);
	
    return false;
}




WebCoreResourceHandle* ResourceLoadListener::handle(HttpRequest* req)
{
    request = req;
    return this;
}

// WebCoreResourceHandle
void ResourceLoadListener::cancel()
{
    delete request;
    request = 0;
}


#if 0
/** When handle is alive, KWQResourceLoader will receive messages 
 * (receivedResponse, data, finish, reportError)
 * 
 * handle is deleted on 
 *  KWQResourceLoader::finish(), 
 *  KWQResourceLoader::reportError()
 *  KWQResourceLoader::cancel()
 *  which causes messages to stop (those methods must be called only once).
 */
class ResHandle : 
{
    HttpRequest* request;
public:

    ResHandle(HttpRequest* req) 
	:request(req) 
    {
    }

    
    ~ResHandle() 
    {
	if (request) delete request;
    }

    void cancel()
    {
#ifdef DEBUG
	g_printerr("%s CANCELLING\n", __PRETTY_FUNCTION__);
#endif
	assert(request);
	delete request;
	request = 0;
    }

    void unref() 
    {
	delete this; 
    }
};
#endif
