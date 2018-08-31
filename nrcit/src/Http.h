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
#ifndef KWIQHttp_h
#define KWIQHttp_h
#include "osb.h" //OSB::URLCredentialStorage

// authentication required status code.
#define HTTP_AUTH_REQUIRED 401


class HttpRequest;
class HttpRequestListener;
class HttpHeader;

class HttpRequestListener
{
 public:
  virtual ~HttpRequestListener() {};
  virtual bool started(const HttpRequest*) { return false;}
  virtual bool finished(const HttpRequest*) { return false;}
  virtual bool header(const HttpRequest*, const HttpHeader *header) { return false;}
  virtual bool data(const HttpRequest*, const char* data, int len) { return false;}
  virtual bool error(const HttpRequest* ) {return false;}

  virtual bool headersEnd(const HttpRequest*) { return false;}
  
  virtual bool resolving(const HttpRequest*) { return false;}

  virtual bool authenticate(HttpRequest*) { return false;}
};



class HttpRequest
{
 public:
  enum Type{ GET, POST };
  enum State {CREATED, RUNNING, ERROR, FINISHED};
  
  HttpRequest(const gchar* aurl, Type atype);    
  virtual ~HttpRequest();

  Type type() { return m_type; }
  
  /** converts params */
  void setType(Type type);   

  virtual void setPostData(const gchar* contentType, GByteArray *);
  
  void setReferrer(const gchar* str) { m_referrer = g_strdup(str);}
  const gchar* referrer() const { return m_referrer; }

  void setUserAgent(const gchar* str) { m_agent = g_strdup(str); }
  const gchar* userAgent() const { return m_agent; }

  void setForceReload(bool force) { m_reload = force;}
  bool forceReload() const {return m_reload;}
  
  /* start the request */
  virtual void execute() = 0;
  
  /** */
  virtual void stop() = 0;

  void setListener(HttpRequestListener* alistener) { listener = alistener;}

  virtual void started(); 
  virtual void finished();
  virtual void error();

  State state() const { return m_state; }

  const gchar* authRealm() const { return m_authRealm; }
  const gchar* authScheme() const { return m_authScheme; }
  bool authIsProxy() const { return m_authIsProxy; }
  virtual void authenticate(const gchar* user, const gchar* pass) = 0;
  

  const gchar * url() { return m_url; }
 protected:
  /** Resets the request. */
  virtual void reset(); 

  HttpRequestListener* listener;
  gchar* m_url;
  gchar* m_authRealm;
  gchar*m_authScheme;
  bool m_authIsProxy;
  
 private:  
  // preferences
  gchar* m_agent;
  gchar*  m_referrer;
  bool m_reload;

  Type m_type;
  State m_state;
        
};



class HttpFactoryS
{

 public:
  HttpFactoryS() {};

  virtual ~HttpFactoryS() {};

  virtual bool canProvide(const gchar* url) const = 0;
  virtual HttpRequest* createRequest(HttpRequestListener* listener,
                                     OSB::URLCredentialStorage* credentials,
				     const gchar* url,
				     HttpRequest::Type type = HttpRequest::GET) = 0;
  

  virtual void setProxy(const gchar* proto, const gchar* url) = 0;
  
  HttpRequestListener* dummyListener();  
};


HttpFactoryS& HttpFactory();


class HttpHeader
{
 public:
  enum Type {
    Location,
    ContentType,
    ContentLength,
    Refresh,
    Unknown,
    Invalid
  };
    
 public:

  HttpHeader(Type atype, const gchar* key, const gchar* aval)
    :m_type(atype), m_key(g_strdup(key)), m_value(g_strdup(aval)) {}
  
  virtual ~HttpHeader() {g_free(m_key);g_free(m_value); }

  virtual Type type() const { return m_type; }
  virtual const gchar*  key() const { return m_key;}
  virtual const gchar*  value() const {return m_value;}

 private:
  Type m_type;
  gchar* m_key;
  gchar* m_value;
  
};


class HttpHeaderLocation : public HttpHeader
{
 public:
  HttpHeaderLocation(const gchar* value);
  ~HttpHeaderLocation() {};
 private:  
};


class HttpHeaderContentType : public HttpHeader
{
 public:
  HttpHeaderContentType(const gchar* value);
  ~HttpHeaderContentType();
    
  const gchar* contentType() const { return m_contentType;}
  const gchar* encoding() const { return m_encoding; }
 private:
  gchar* m_contentType;
  gchar* m_encoding;
};

class HttpHeaderContentLength : public HttpHeader
{
 public:
  HttpHeaderContentLength(const gchar* value);
  ~HttpHeaderContentLength() {}
    
  const gint contentLength() const { return m_length;}
 private:
  gint m_length;
};

class HttpHeaderRefresh :  public HttpHeader
{
 public:
  HttpHeaderRefresh(const gchar* value);
  ~HttpHeaderRefresh() {};
   
};




#endif
