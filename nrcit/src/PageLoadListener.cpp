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
#include "PageLoadListener.h"
#include "ResourceLoadListener.h"
#include "GLibHelpers.h"

PageLoadListener::PageLoadListener(BridgeImpl* b, const gchar * URL)
    : bridge(b)
    , url(g_strdup(URL))
    , contentType(g_strdup("application/octet-stream"))
    , encoding(0)
    , refresh(0)
    , done_url(false)
    , recvd_enc(false)
    , has_error(false)
    , auth_attempted(false)
    , commited(false)
{
    lastModified = 0;
}
    
PageLoadListener::~PageLoadListener() 
{
    g_free(url);
    g_free(encoding);
    g_free(contentType);
    g_free(refresh);
}
    
bool PageLoadListener::header(const HttpRequest*, const HttpHeader *header)
{
    if (!commited) { 
	bridge->commitLoad();
	commited = true;
    }

    switch (header->type()){
    case HttpHeader::Location:
    {
	const gchar* rdir_url = header->value();
	// FIXME: check for relative url!

	assignToString(&url, rdir_url);	
	bridge->redirection(url);

        return true;
    }
    case HttpHeader::ContentType:
    {
	const HttpHeaderContentType* ct
	    = static_cast<const HttpHeaderContentType*>(header);
	    
	if (ct->encoding()) {
	    assignToString(&encoding, ct->encoding());
	    recvd_enc = true;
	}
	    
	assignToString(&contentType, ct->contentType());
	return true;
    }   
    case HttpHeader::ContentLength:
    {
	const HttpHeaderContentLength *cl
	    = static_cast<const HttpHeaderContentLength*>(header);
        status.setSize(cl->contentLength());
        return true;
    }
    case HttpHeader::Refresh:
    {
	assignToString(&refresh, header->value());
	return true;
    }
    case HttpHeader::Invalid:
    default:
    {
	return false;
    }
    }
}

bool PageLoadListener::data(const HttpRequest*, const char* data, int len)
{	     
    doOpenURL();
    bridge->addData(data,len);
    status.addReceived(len);
    bridge->emitResourceLoadStatus(&status);
    return true;
}

bool PageLoadListener::started(const HttpRequest*)
{
    bridge->emitResourceLoadStarted(&status);
    return true;
}

bool PageLoadListener::finished(const HttpRequest*)	
{
    doOpenURL();
    bridge->loadURLFinished(has_error);
    bridge->emitResourceLoadFinished(&status);
    if ( bridge->numPendingOrLoadingRequests()==0 || has_error) {
        bridge->emitFrameLoadFinished(has_error?-1:0);
    }

    return true;
}

bool PageLoadListener::error(const HttpRequest*)
{
    status.setError();
    has_error = true;
    return true;
}
    
bool PageLoadListener::authenticate(HttpRequest* request) 
{

    OSB::URLCredentialStorage* creds = bridge->credentials();

    OSB::URLProtectionSpace space(request->url(),
				  request->authRealm(),
				  OSB::URLProtectionSpace::Default, 
				  OSB::URLProtectionSpace::NoProxy);

    const OSB::URLCredential *cred = creds->defaultCredential(space);
    if (auth_attempted) {
	// authentication failed -- clear auth cache for that space
	if (cred) 
	    creds->removeCredential(*cred, space);
	cred = 0;
    }

    auth_attempted = true;

    if (!cred) {	
	gchar* user = 0;
	gchar* password = 0;
	bool ret = bridge->authenticate(request->authRealm(), &user, &password);
	if (ret && user && password) {
	    // update credentials
	    OSB::URLCredential newcred(user, password, OSB::URLCredential::ForSession);
	    creds->setCredential(newcred, space);
	    request->authenticate(newcred.user(), newcred.password());
	}
	if (user) g_free(user);
	if (password) g_free(password);
	return ret;
    }

    // try to apply authentication
    request->authenticate(cred->user(), cred->password());
    return true;
}

void PageLoadListener::doOpenURL()
{
    if (done_url) return;

    bridge->emitResourceLoadHeaders(&status);    
    bridge->saveHistoryItem();    

    /* clear current state */
    bridge->openURL(url, false, contentType, refresh, lastModified, 0);
    bridge->setEncoding(encoding, !recvd_enc);

    done_url = true;
}
   
