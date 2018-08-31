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
#ifndef OSB__osb_h
#define OSB__osb_h

#include <glib.h>
#include <gtk/gtk.h>

/** private forward declarations */
class FrameImpl;
class RootImpl;
class PreferencesImpl;

namespace OSB {

/** public forward declarations*/
class Preferences;
class History;
class Position; 
class Root;
class Frame;
class FrameLoadDelegate;
class UIDelegate;
class ProtocolDelegate;
class URLCredentialStorage;

class DataSource;

class ElementInfo
{
public:
    virtual ~ElementInfo() {}
    virtual gboolean selected()=0;
    virtual const gchar* linkTitle() =0;
    virtual const gchar*  linkLabel()=0;
    virtual const gchar* linkURL()=0;
    virtual const gchar* linkTarget()=0;
    /** x = y = w = h= -1 if there's no image */
    virtual void imageRect(int &x, int &y, int &w,int &h) = 0;
    virtual const gchar* imageURL()=0;
    virtual const gchar* imageAltText()=0;
};

class ContentRepresentation
{
public:
    virtual ~ContentRepresentation();
    virtual void setDataSource(DataSource* source)=0;
    virtual void receivedData(DataSource* source)=0;
    virtual void receivedError(DataSource* source)=0;
    virtual void finishedLoading(DataSource* source)=0;
    
    virtual bool canProvideDocumentSource()=0;
    virtual const gchar* documentSource()=0;

    virtual const gchar* title()=0;

    virtual GtkWidget* widget()=0;
};

class ContentRepresentationFactory
{
public:
    virtual ~ContentRepresentationFactory();
    virtual ContentRepresentation* create(const char* mime)=0;
    virtual void destroy(ContentRepresentation*)=0;

    enum Type { Embedded, Frame, Standalone };
    virtual Type type(const char* mime)=0;
};


class ProtocolDelegate 
{
public:
    virtual ~ProtocolDelegate() {}

    virtual bool provides(const gchar* proto){ return false;}
    virtual void setProxy(const gchar* proxy){}
};

class UIDelegate
{
public:
    virtual ~UIDelegate() {}

    virtual void setStatusText(Frame* sender, const gchar* status){}

    virtual bool toolbarsVisible(Frame* sender) {return true;}
    virtual void setToolbarsVisible(Frame* sender, bool flag){}
    virtual bool statusBarVisible(Frame* sender) {return true;}
    virtual void setStatusBarVisible(Frame* sender, bool flag) {}

    /** 
     * @param node type to be changed 
     */
    virtual void onMouseOverChanged(Frame* sender, ElementInfo* node){}


    virtual Root* createNewRoot(Frame* sender, const gchar* url){return 0;}

    /** Prompt user an alert panel
     * @param message message to display in alert panel
     */
    virtual void alertPanel(Frame* sender, const gchar *message) {}
    
    /** Prompt user with confirmation panel 
     *
     * @return \true if user confirmed the panel
     *         \false if user cancelled the panel
     */
    virtual bool confirmPanel(Frame* sender, const gchar *message) { return false; }

    /** Prompt user with text input panel
     *
     * @param result pointer to string that should contain result of the text user entered to panel
     *        String should be allocated with glib allocation functions (g_new(), g_strdup,..),
     *        it will be freed by the caller with g_free()
     * @return \true if user confirmed the panel
     *         \false if user cancelled the panel
     */
    virtual bool textInputPanel(Frame* sender, const gchar *prompt, const gchar *defaultText, gchar **result)
    {if (result) *result = 0; return false;}

    /** Prompt user with authentiction panel
     * 
     * @param realm realm to authenticate user in
     * @param username pointer to string that contains username to use. 
     *        String should be allocated with glib allocation functions (g_new(), g_strdup,..),
     *        it will be freed by the caller with g_free()
     * @param password password to use or 0.
     *        String should be allocated with glib allocation functions (g_new(), g_strdup,..),
     *        it will be freed by the caller with g_free()
     *
     *	@return \true if authentication should be tried,
     *	        \false if authentication should not be tried
     */    
    virtual bool authPanel(Frame* sender, const gchar * realm, gchar ** username, gchar ** password) { if (username) *username=0; if (password) *password=0; return false;}

};

class FrameLoadDelegate 
{
public:
    virtual ~FrameLoadDelegate() {}

    /** parameters will change*/
    virtual void onClientRedirectReceived(Frame* sender, const gchar* to){}
    virtual void onClientRedirected(Frame* sender, const gchar* to){}
    virtual void onClientRedirectCancelled(Frame* sender, const gchar* to){}
    virtual void onServerRedirected(Frame* sender, const gchar* to) {}

    virtual void onFrameLoadStarted(Frame* sender) {}
    virtual void onFrameLoadFinished(Frame* sender, int status) {}

    // after this we are working with new url 
    virtual void onCommitLoad(Frame* sender) {}
    
    virtual void onTitleChanged(Frame* sender, const gchar* title) {}
};

class ResourceStatus {
private:
    static int uniqId;
    static int newId() { return uniqId++; }

    gchar * _name;    
    gint _size;
    gint _last;
    gint _received;
    gint _status;
    bool _error;

    int _id;
    
protected:
    ResourceStatus(const gchar * name = 0)
        : _name(name?g_strdup(name):0),
        _size(0), _last(0), _received(0),
        _status(0), _error(false) {  _id = newId(); }
        ~ResourceStatus() { g_free(_name); }
    void setSize(gint size) { _size = size; }
    void addReceived(gint received) { _received += received; _last=received; }
    void setError(gint errorCode = -1) { _error =true; _status = errorCode; }
public:

    const gchar * name() { return _name; }

    gint id() const { return _id; }
    gint size() const { return _size; }
    gint received() const { return _received; }
    gint lastReceived() const { return _last; }
    gint status() const { return _status; }
    bool error() const { return _error; }
};

class ResourceLoadDelegate
{
public:
    virtual ~ResourceLoadDelegate() {}

    virtual void onResourceLoadStarted(Frame* sender, const ResourceStatus * status){}

    virtual void onResourceLoadHeaders(Frame* sender, const ResourceStatus * status){}

    /** for each item loaded in frame, including the frame page itself
     * @param item type to be changed 
     */
    virtual void onResourceLoadStatus(Frame* sender, const ResourceStatus * status){}

    
    virtual void onResourceLoadFinished(Frame* sender, const ResourceStatus * status){}

};

class BackForwardListItem
{
public:
    virtual ~BackForwardListItem() {}

    virtual const gchar* URL()=0;
    virtual const gchar* originalURL()=0;

    virtual const gchar* title()=0;
    virtual const gchar* alternateTitle()=0;
    virtual void setAlternateTitle(const gchar* title)=0;

    // GdkPixbuf icon();

    /** Get the last visited time
     * @returns the last visited time. struct guaranteed to be valid until destruction of the item
     */
    virtual const GDate* lastVisited()=0;
};

class BackForwardList 
{
public:
    typedef BackForwardListItem Item;
    typedef void (*ChangedFunc)(BackForwardList*, Item*, gpointer data);

    virtual ~BackForwardList() {};

    virtual void goBack()=0;
    virtual void goForward()=0;
    virtual void goToItem(Item* item)=0;

    virtual Item* backItem()=0;
    virtual int backListCount()=0;
    virtual Item* currentItem()=0;

    virtual Item* itemAtIndex(int index)=0;

    virtual Item* forwardItem()=0;
    virtual int forwardListCount()=0;

    virtual int pageCacheSize()=0;
    virtual void setPageCacheSize(int sz)=0;

    virtual int capacity()=0;
    virtual void setCapacity(int sz)=0;

    virtual void setChangedNotification(ChangedFunc notifyCallback, gpointer data)=0;
};

class Root
{
    RootImpl *impl;
public:
    Root(const gchar* frameName=NULL, const gchar* group=NULL);
    
    virtual ~Root();
    
    void setPreferences(const Preferences&);
    Preferences* preferences();
    
    void setGroup(const gchar* group);
    const gchar* group();

    Frame* mainFrame();    

    BackForwardList* backForwardList();

    URLCredentialStorage* credentialStorage();

    void goBack();
    bool canGoBack();
    void goForward();
    bool canGoForward();
    
    /** set text size multiplier
     *  set text multiplier. 
     * @param multiplier the text size multiplier 0.0 == 0%, 1.0 == 100% etc
     */
    void setTextSizeMultiplier(float multiplier);

    /** get text size multiplier */
    float textSizeMultiplier();

    void setFrameLoadDelegate(FrameLoadDelegate*);
    void setResourceLoadDelegate(ResourceLoadDelegate*);
    void setUIDelegate(UIDelegate*);

    ProtocolDelegate* protocolDelegateForURL(const gchar* url);

    void registerContentRepresentation(ContentRepresentationFactory* crf, const gchar* mimeType);
    bool searchFor(const gchar *string, bool forward, bool caseSensitive);
};

class Frame
{
public:
    virtual ~Frame() {};
    
    virtual void startLoad(const char* url) = 0;
    virtual void stopLoad() = 0;
    virtual void refresh() = 0;

    virtual GtkWidget* widget() = 0;

    /** */
    virtual const gchar* title() = 0;
    virtual const gchar* location() = 0;
};


/** class for HTTP/URL credentials. */
class URLCredential
{
public:
    /** Available persistence options. */
    enum Persistence {
        None,
        ForSession,
        Permanent
    };

public:
    URLCredential();
    URLCredential(const gchar* user, const gchar* pass, Persistence p);
    URLCredential(const URLCredential& other);

    URLCredential& operator=(const URLCredential& other);
    virtual ~URLCredential();
  
    
    /** Returns the user part of the credentials. */
    const gchar* user() const { return m_user?m_user:""; };

    /** Returns the password part of the credentials. */
    const gchar* password() const { return m_pass?m_pass:""; }
    
    URLCredential::Persistence persistence() const { return m_pers; }

private:
    gchar *m_user, *m_pass;
    Persistence m_pers;
};

/** Interface for HTTP/URL protection space. */
class URLProtectionSpace
{
public:
    /** Protection space proxy type. */
    enum ProxyType {
        NoProxy,
        HTTPProxy,
        HTTPSProxy,
        FTPProxy,
        SOCKSProxy
    };

    /** Protection space authentication method. */
    enum AuthenticationMethod{
        /** Default method. */
        Default,
        /** HTTP basic authentication. */
        HTTPBasic,
        /** HTTP MD5 digest authentication. */
        HTTPDigest,
        /** HTML Forms based authentication. */
        HTMLForm
    } ;
    
    URLProtectionSpace(const gchar* url,		       
		       const gchar* realm,
		       URLProtectionSpace::AuthenticationMethod method,
		       URLProtectionSpace::ProxyType proxyType);

    URLProtectionSpace(const URLProtectionSpace&);
    URLProtectionSpace& operator=(const URLProtectionSpace& other);
    virtual ~URLProtectionSpace();

    /** Returns the authentication method of the protection space. */
    AuthenticationMethod authenticationMethod() const { return m_method; }

    /** Returns the host of the protection space. */
    const gchar * host() const { return m_host?m_host:""; }

    /** Returns the scheme://host[:port]/path representation of the protection space. */
    const gchar * hostPath() const {return m_url; }
    
    /** Returns the host port of the protection space. */
    const guint port() const { return m_port;}
    
    /** Returns whether proxy authentication is used. */
    gboolean isProxy() const {return m_proxyType != URLProtectionSpace::NoProxy; }

    /** Returns the protocol part of the protection domain. */
    const gchar * protocol() const {return m_protocol;}

    /** Returns the type of the proxy. */
    ProxyType proxyType() const { return m_proxyType; }

    /** Returns the authentication realm */
    const gchar * realm() const {return m_realm;}

    /** Returns whether authentication in given domain is secure. */
    gboolean receivesCredentialSecurely() const { return m_secure; }
private:
    URLProtectionSpace::AuthenticationMethod m_method;
    URLProtectionSpace::ProxyType m_proxyType;
    gboolean m_secure;

    gchar* m_url;
    gchar* m_realm;
    gchar* m_protocol;
    gchar* m_host;
    guint m_port;

};

/** Interface for the credential storage. */
class URLCredentialStorage
{
public:
    /** Returns dictionary of all credentials */
    // virtual const NSDictionary allCredentials();

    /** Get default credentials for protection space. */
    virtual const URLCredential * defaultCredential(const URLProtectionSpace &) const =0;

    /** Set the default credentials for protection space. */
    virtual void setDefaultCredential(const URLCredential &, const URLProtectionSpace &) = 0;

    /** Get all credentials for protection space in a
        dictionary.  Usernames work as keys and credentials
        as values. */
/*     virtual const NSDictionary * */
/*         credential(URLProtectionSpace *) = 0; */

    /** Set the default credentials for protection space. */
    virtual void setCredential(const URLCredential &, const URLProtectionSpace &) = 0;

    
    /** Removes credential from the credential storage
        for the specified protectionSpace. */
    virtual void removeCredential(const URLCredential &, const URLProtectionSpace &) = 0;
};

class Preferences
{
    friend class RootImpl;
    /** preferences with default settings */
    Preferences();
    ~Preferences();

    /* copy semantics */
    Preferences(const Preferences& other);  
    Preferences& operator=(const Preferences&);
public:
    void setMinimumFontSize(float a);
    float minimumFontSize();
    
    void setMinimumLogicalFontSize(float a);
    float minimumLogicalFontSize();
    
    void setDefaultFontSize(float a);
    float defaultFontSize();
    
    void setDefaultFixedFontSize(float a);
    float defaultFixedFontSize();
    
    void setJavaEnabled(bool a);
    bool javaEnabled();
    
    void setPluginsEnabled(bool a);
    bool pluginsEnabled();

    void setJavaScriptEnabled(bool a);
    bool javaScriptEnabled();

    void setJavaScriptCanOpenWindowsAutomatically(bool a) ;
    bool javaScriptCanOpenWindowsAutomatically();

    void setWillLoadImagesAutomatically(bool a);
    bool willLoadImagesAutomatically();

    void setShouldPrintBackgrounds(bool a);
    bool shouldPrintBackgrounds();

    void  setUserStyleSheetLocation(const gchar* location);  
    const gchar* userStyleSheetLocation();

    void setDefaultTextEncoding(const gchar* encoding);
    const gchar* defaultTextEncoding();

    void setStandardFontFamily(const gchar* s);
    const gchar* standardFontFamily();

    void setFixedFontFamily(const gchar* s);
    const gchar* fixedFontFamily();  
  
    void setSerifFontFamily(const gchar* s);
    const gchar* serifFontFamily();
  
    void setSansSerifFontFamily(const gchar* s);
    const gchar* sansSerifFontFamily();

    void setCursiveFontFamily(const gchar* s);
    const gchar* cursiveFontFamily();
    
    void setFantasyFontFamily(const gchar* s);
    const gchar* fantasyFontFamily();

private:    
    PreferencesImpl* d;
};


} // namespace OSB


#endif
