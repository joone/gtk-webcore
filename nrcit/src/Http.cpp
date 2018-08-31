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
#include <string.h>
#include <stdlib.h> //atoi
#include <glib.h>
#include <assert.h>

#include "Http.h"

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if defined(HAVE_LIBCURL)
#include "HttpCurl.h"
#endif

#if !defined(HAVE_LIBCURL)
#error Need curl.
#endif


class CombinerFactoryS : HttpFactoryS
{

    CombinerFactoryS();
 public:
    ~CombinerFactoryS();
    bool canProvide(const gchar * url) const;
    HttpRequest* createRequest(HttpRequestListener* listener,
                               OSB::URLCredentialStorage* credentials,
			       const gchar * url,
			       HttpRequest::Type type);

    void setProxy(const gchar * proto, const gchar * url);
 private:

    HttpFactoryS* factoryForURL(const gchar * url) const;

    friend HttpFactoryS& HttpFactory();
    GList * factories;
};

CombinerFactoryS::CombinerFactoryS()
    :factories(0)
{
    
#if defined(HAVE_LIBCURL)
    factories = g_list_append(factories, &CurlFactory());
#endif    
}


CombinerFactoryS::~CombinerFactoryS()
{
    g_list_free(factories);
}        

extern"C"{
static gboolean findFactoryFunc(gconstpointer fact, gconstpointer data)
{
    const HttpFactoryS * factory = static_cast<const HttpFactoryS *>(fact);
    const gchar * url = static_cast<const gchar *>(data);
    return factory->canProvide(url)? 0 : -1;
}
}

HttpFactoryS* CombinerFactoryS::factoryForURL(const gchar * url) const
{
    GList * item = g_list_find_custom(factories, url, findFactoryFunc);
    if (item) {
        return static_cast<HttpFactoryS *>(item->data);
    }
    return 0;
}

bool CombinerFactoryS::canProvide(const gchar * url) const
{
    return factoryForURL(url) != 0;

}

HttpRequest* CombinerFactoryS::createRequest(HttpRequestListener* listener,
                               OSB::URLCredentialStorage* credentials,
			       const gchar * url,
			       HttpRequest::Type type = HttpRequest::GET)
{
    HttpFactoryS *f = factoryForURL(url);

    if (f) 
	return f->createRequest(listener, credentials, url, type);

    return 0;
}

void CombinerFactoryS::setProxy(const gchar * proto, const gchar * proxyURL)
{

    HttpFactoryS *f = factoryForURL(proto);

    if (f) {
	f->setProxy(proto, proxyURL);
    }
	
}

HttpFactoryS& HttpFactory()
{
    static CombinerFactoryS factory;
    return factory;
}

HttpRequestListener* HttpFactoryS::dummyListener()
{
    static HttpRequestListener single;
    return &single;
}


/** HttpRequest */

HttpRequest::HttpRequest(const gchar * aurl, Type atype)
    :listener(HttpFactory().dummyListener())
    ,m_url(g_strdup(aurl))
    ,m_authRealm(0)
    ,m_authScheme(0)
    ,m_authIsProxy(false)
    ,m_agent(0)
    ,m_referrer(0)
    ,m_reload(false)
    ,m_type(atype)
    ,m_state(CREATED)
{

}


HttpRequest::~HttpRequest()
{
    g_free(m_url);
    g_free(m_authRealm);
    g_free(m_authScheme);
    g_free(m_agent);
    g_free(m_referrer);
    
    listener = 0; /* so we wouldn't get any references to listener after deletion */
}

void HttpRequest::setType(Type type) 
{
    m_type = m_type;    
}

void HttpRequest::setPostData(const gchar * contentType, GByteArray * buf) 
{

}

void HttpRequest::error()
{
    m_state = ERROR;
    listener->error(this);
}

void HttpRequest::reset()
{
    m_state = CREATED;
}    

void HttpRequest::started()
{
    m_state = RUNNING;
    listener->started(this);    
}    

void HttpRequest::finished()
{
    m_state = FINISHED;
    listener->finished(this);
}

HttpHeaderLocation::HttpHeaderLocation(const gchar * value)
    :HttpHeader(Location, "Location", value)
{
}

HttpHeaderRefresh::HttpHeaderRefresh(const gchar * value)
    :HttpHeader(Refresh, "Http-Refresh", value)
{
}

HttpHeaderContentType::HttpHeaderContentType(const gchar * value) 
    :HttpHeader(ContentType, "Content-Type", value)
    ,m_contentType(0)
    ,m_encoding(0)
{

    gchar **chunks = g_strsplit(value, ";", 2);
    assert(chunks);

    int i = 0;
    while(chunks[i]) {
	gchar* charset = strstr(chunks[i], "charset="); 
	if (charset) {
	    // assume "CONTENT-TYPE; charset=CHARSET-NAME" or "charset=CHARSET-NAME"
	    if (!m_encoding) {
		m_encoding = g_strstrip(g_strdup(charset+8));
		g_free(chunks[i]);
	    }
        } else {
	    // assume "CONTENT-TYPE" or "CONTENT-TYPE; CHARSET-NAME"
	    if (!m_contentType) 
		m_contentType = chunks[i];
	    else if (!m_encoding)
		m_encoding = chunks[i];
	}
	++i;
    }
    g_free(chunks);
}


HttpHeaderContentType::~HttpHeaderContentType()
{
    if (m_contentType) g_free(m_contentType); 
    if (m_encoding) g_free(m_encoding); 
}

HttpHeaderContentLength::HttpHeaderContentLength(const gchar * value) 
    :HttpHeader(ContentLength, "Content-Length", value)
    ,m_length(0)
{
    assert(value);
    gchar* endptr = 0;

    m_length = strtol(value, &endptr, 10);

    if (!(*value != '\0' && endptr && *endptr=='\0'))
	m_length = -1;
	
}
