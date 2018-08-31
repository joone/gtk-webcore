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
#include <string.h>
#include <glib.h>
#include <curl/curl.h>

#include "osb.h" // URLProtectionSpaec
#include "HttpCurl.h"
#include "GLibHelpers.h"

class CurlRequest : public HttpRequest
{
    friend class CurlFactoryS;
public:
    CurlRequest(CurlFactoryS *p, const gchar * aurl, Type atype, CURLM* multi);
    ~CurlRequest();

    void execute();
    void stop();

    void error(int curlerror);

    void data(const char* data, int len);
    void header(const char* data, int len);

    void finish();  
    
    void authenticate(const gchar * user, const gchar * pass);
    void setProxy(const gchar * proxyURL);
    void setPostData(const gchar * contentType, GByteArray *);   

protected:
    bool shouldAuthenticate() { return m_authenticate; }
    
    /** Authenticates the request. */
    void doAuth();
    
    void cleanup();

private:    

    CURLM *curlMultiHandle;	// not freed
    CURL *handle;

    gchar *password;
    const gchar *curl_referrer;

    struct HttpPost* curl_post;
    struct HttpPost* curl_post_last;

    GByteArray * postData;
    bool headerEnd;
    bool m_authenticate;

    gchar * proxy;
    CurlFactoryS* parent;

};

extern "C" {

static
gboolean
event_cb(gpointer user_data)
{
    assert(user_data);
    CurlFactoryS* self = static_cast<CurlFactoryS*>(user_data);

    return self->event();
}

}
static bool curlInitialized = false;

CurlFactoryS::CurlFactoryS()
    : proxy(0)
{
    if (!curlInitialized) {
	curl_global_init(CURL_GLOBAL_ALL);
	curlInitialized = true;
    }

    curl_version_info_data *curl_info;
    curl_info = curl_version_info(CURLVERSION_NOW);
    curl_protocols = curl_info->protocols;

    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);

    gsId = g_timeout_add_full(G_PRIORITY_DEFAULT_IDLE,
			       callbackTimeoutMS,  // milliseconds
			       (GSourceFunc)& ::event_cb, //callback
			       this, //data
			       NULL); //notify

    curlMultiHandle = curl_multi_init();
}

CurlFactoryS::~CurlFactoryS()
{
    if (gsId) {
	GSource* s = g_main_context_find_source_by_id(NULL, gsId);
	if (s) g_source_destroy(s);
    }

    if (curlMultiHandle)
	curl_multi_cleanup(curlMultiHandle);

    if (curlInitialized) {
	curl_global_cleanup();
	curlInitialized = false;
    }

    g_free(proxy);
}

void CurlFactoryS::setProxy(const gchar * proto, const gchar * proxyURL)
{
    assignToString(&proxy, proxyURL);
}

bool CurlFactoryS::canProvide(const gchar * url) const
{
    int i = 0;
    while(curl_protocols[i]) {
	if (g_str_has_prefix(url, curl_protocols[i]))
	    return true;
	++i;
    }
    
    return false;
}


HttpRequest* CurlFactoryS::createRequest(HttpRequestListener* listener,
                                         OSB::URLCredentialStorage* credentials,
					 const gchar * url,
					 HttpRequest::Type type)
{
    if (!curlMultiHandle) {
	return 0;
    }

    CurlRequest* req = new CurlRequest(this, url, type, curlMultiHandle);
    req->setListener(listener);
    
    if (proxy)
	req->setProxy(proxy);

    OSB::URLProtectionSpace space( url,  "",
                                   OSB::URLProtectionSpace::Default, 
                                   OSB::URLProtectionSpace::NoProxy);
    
    const OSB::URLCredential * cred = credentials->defaultCredential(space);
    if (cred) {
        req->authenticate(cred->user(), cred->password());
    }
    
    return req;
}

void CurlFactoryS::update()
{

    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);
    curl_multi_fdset(curlMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
}


/** timeout event callback
 *  @return FALSE if timeout is to be removed,
 *          TRUE if timeout is to be occur again
 */
gboolean CurlFactoryS::event()
{
    update();

    int nrunning;
    struct timeval timeout;
    int retval;
    timeout.tv_sec = 0;
    timeout.tv_usec = selectTimeoutMS * 1000;	// select waits microseconds

    retval = ::select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
    switch(retval) {
    case -1:			// select error
#if DEBUG	
	g_printerr("%s, select error(?)",__PRETTY_FUNCTION__);
#endif
	/* fallthrough*/
    case 0:			// select timeout
	/* fallthrough. this can be the first perform to be made */
    default: 			// 1+ descriptors have data
	while(CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curlMultiHandle, &nrunning));
	break;
    }

    // check the curl messages indicating completed transfers
    // and free their resources
    CurlRequest* req;
    CURLMsg* msg;
    CURL *handle;
    int nmsgs;

    while ((msg = curl_multi_info_read(curlMultiHandle, &nmsgs)))
    {
	handle = msg->easy_handle;

	if (msg->msg == CURLMSG_DONE) {	    
	    // find the node which has same handle as completed transfer
	    assert(handle);
            
	    curl_easy_getinfo(handle, CURLINFO_PRIVATE, &req);
	    assert(req); //fixme: assert->if ?
                        
	    // if found, delete it
	    if (req) {
                switch (msg->data.result) {
		case CURLE_OK:
		{
                    // use this to authenticate          
                    int respCode = -1;
                    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &respCode);            

                    if (respCode == HTTP_AUTH_REQUIRED &&
                        req->shouldAuthenticate()) {
                        // reset -- be careful not to call finish.
                        // another thing also is to validate whether
                        // data should be shown prior 
                        req->cleanup();
                        // authenticate
                        req->doAuth();
                        // execute query again.
                        req->execute();
                    } else {         
                        req->finish();
                    }
                    break;
		}
		default:
		{
		    req->error(msg->data.result);
		    req->finish();
		    break;
		}
 		}

	    } else {
#if DEBUG		
		g_printerr("CurlRequest not found, eventhough curl handle finished");
#endif
		assert(0);		
	    }
	}
    }

    return TRUE;
}

CurlFactoryS& CurlFactory()
{
    static CurlFactoryS single;
    return single;
}


extern "C" {
static size_t
write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
    assert(data);
    int realsize = size * nmemb;
    CurlRequest *req = static_cast<CurlRequest*> (data);
    req->data((char*)ptr, realsize);

    return realsize;
}

static size_t
header_cb(char *ptr, size_t size, size_t nmemb, void* data)
{
    assert(data);
    int realsize = size * nmemb;
    CurlRequest *req = static_cast<CurlRequest*> (data);

    req->header((char*)ptr, realsize);
    return realsize;
}

}

CurlRequest::CurlRequest(CurlFactoryS* p, 
			 const gchar * aurl, 
			 HttpRequest::Type atype, 
			 CURLM *multi)
    :HttpRequest(aurl, atype)
    , curlMultiHandle(multi)
    , handle(NULL)
    , password(NULL)
    , curl_referrer(NULL)
    , curl_post(NULL)
    , curl_post_last(NULL)
    , postData(NULL)
    , headerEnd(false)
    , m_authenticate(true)
    , proxy(NULL)
    , parent(p)    
{
    assert(curlMultiHandle);
    assert(parent);
}

CurlRequest::~CurlRequest()
{
    cleanup();
    g_byte_array_free(postData, true);
    g_free(password);
    g_free(proxy);
    curlMultiHandle = 0;
}

void CurlRequest::cleanup()
{
    if (handle) {
	curl_multi_remove_handle(curlMultiHandle, handle);
	curl_easy_cleanup(handle);
	handle = NULL;
    }    
}

void CurlRequest::setProxy(const gchar * proxyURL)
{
    if (handle) 
	return;

    assignToString(&proxy, proxyURL);    
}

void CurlRequest::setPostData(const gchar * contentType, GByteArray * data) 
{
    if (strcmp(contentType, "application/x-www-form-urlencoded") ||
        strncmp(contentType, "multipart/form-data", 19)) {
        postData = g_byte_array_new();
        memcpy( postData->data, data->data, data->len );
        postData->len = data->len;
    } else { 
#if DEBUG
	g_printerr("Content type '%s' not supported", contentType);
#endif
	assert(0);
    }
}

void CurlRequest::authenticate(const gchar * user, const gchar * pass)
{
    if (handle)
	return;
    if (this->password)
        g_free(this->password);
    if (user && pass)
        this->password = g_strdup_printf("%s:%s", user, pass);
}

void CurlRequest::doAuth() {
    m_authenticate = listener->authenticate(this);
}

void CurlRequest::execute()
{
    if (handle)
	return; 		// already running

    handle = curl_easy_init();

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, ::write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, ::header_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEHEADER, this);

    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 10);

    // url ptr must remain valid through the request
    curl_easy_setopt(handle, CURLOPT_URL, m_url); 

    if (this->password) {
        curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
        curl_easy_setopt(handle, CURLOPT_USERPWD, this->password);
    }
    
    // must remain valid or not ?
    if (proxy)
	curl_easy_setopt(handle, CURLOPT_PROXY, proxy);

    curl_easy_setopt(handle, CURLOPT_PRIVATE, this);

    if (type() == HttpRequest::POST) {
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postData->data);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, postData->len);
    }

    started();

    CURLMcode ret = curl_multi_add_handle(curlMultiHandle, handle);
    if (ret == CURLM_CALL_MULTI_PERFORM) {
	// don't call perform, because events must be async
	// timeout will occur and do curl_multi_perform
    } else if (ret == CURLM_OK) {

    } else {
	cleanup();
	HttpRequest::error();
    }

}

void CurlRequest::stop()
{
    cleanup();

    listener->finished(this);
}

void CurlRequest::error(int curlerror)
{
#if DEBUG
    g_printerr("curl error while loading url:'%s' errorno:%d", m_url, curlerror);
#endif		    
    HttpRequest::error();	
}

void CurlRequest::data(const char* data, int len)
{
    if (!headerEnd) {
	listener->headersEnd(this);
	headerEnd = true;
    }

    // use this to check whether authentication is needed. 
    int status = -1;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &status);            
    
    // if we are in middle of authenticating, do nothing.
    if (status != HTTP_AUTH_REQUIRED || !m_authenticate) {
        listener->data(this, data, len);
    }
}

#define HTTP_LOCATION "Location"
#define HTTP_CONTENT_TYPE "Content-Type"
#define HTTP_CONTENT_LENGTH "Content-Length"
#define HTTP_REFRESH "Http-Refresh"
#define HTTP_WWW_AUTHENTICATE "WWW-Authenticate"

HttpHeader* parseHeader(const gchar* data, int len)
{
    int i=0;
    while(i<len) {
	if (data[i] == ':') break;
	++i;
    }
    if (i==len) return 0;
    
    // these are freed
    gchar * key = g_strstrip(g_strndup(data, i));
    gchar * value = g_strstrip(g_strndup(&data[i+1], len - i - 1));

    HttpHeader * header;
    if (!strcmp(key, HTTP_LOCATION)) {
	header = new HttpHeaderLocation(value);
    } else if (!strcmp(key, HTTP_CONTENT_TYPE)) {
	header = new HttpHeaderContentType(value);
    } else if (!strcmp(key, HTTP_CONTENT_LENGTH)) {
	header = new HttpHeaderContentLength(value);
    } else if (!strcmp(key, HTTP_REFRESH)) {
        header = new HttpHeaderRefresh(value);
    } else {
        header = new HttpHeader(HttpHeader::Unknown, key, value);
    }

    g_free(key);
    g_free(value);

    return header;
}

void CurlRequest::header(const char* data, int len)
{
    HttpHeader *hdr = parseHeader(data, len);
    if (hdr) {
        if ( hdr->key() == HTTP_WWW_AUTHENTICATE)
        {
            const gchar * value = hdr->value();
            char * realm = g_strrstr(value, "realm=" );
            m_authScheme = g_strstrip(g_strndup(value, realm - value));
            m_authRealm = g_strstrip(g_strdup(realm+6));
        }             
        listener->header(this, hdr);
	delete hdr;
    }
}

void CurlRequest::finish()
{
    cleanup();
    finished();
}






