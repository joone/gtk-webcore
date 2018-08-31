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
#include <stdio.h>
#include <glib.h>
#include <string.h>

#include "osb.h"
#include "BridgeImpl.h"

#include "Http.h"
#include "osbimpl.h"
#include "rootimpl.h"
#include "frameimpl.h"
#include "prefimpl.h"
#include "NRCore/WebCoreSettings.h"

#include "GLibHelpers.h"

using namespace OSB;

int ResourceStatus::uniqId = 0;

Root::Root(const gchar* frameName, const gchar* groupName)
{
    impl = new RootImpl(this, frameName, groupName);
}

Root::~Root()
{
    delete impl;
}

void Root::setGroup(const gchar* group)
{
    return impl->setGroup(group);
}

const gchar* Root::group() 
{
    return impl->group();
}


void Root::setPreferences(const Preferences& prefs)
{
    impl->setPreferences(prefs);
}

Preferences*  Root::preferences()
{
    return impl->preferences();
}

Frame* Root::mainFrame()
{
    return impl->mainFrame();
}

BackForwardList* Root::backForwardList()
{
    return impl->backForwardList();
}

void Root::goBack()
{
    impl->goBack();
}

bool Root::canGoBack()
{
    return impl->canGoBack();
}
void Root::goForward()
{
    impl->goForward();
}

bool Root::canGoForward()
{
    return impl->canGoForward();
}

void Root::setFrameLoadDelegate(FrameLoadDelegate* d)
{
    impl->setFrameLoadDelegate(d);
}

void Root::setResourceLoadDelegate(ResourceLoadDelegate* d)
{
    impl->setResourceLoadDelegate(d);
}

void Root::setUIDelegate(UIDelegate* d)
{
    impl->setUIDelegate(d);
}

ProtocolDelegate* Root::protocolDelegateForURL(const gchar* url)
{
    return impl->protocolDelegateForURL(url);
}

void Root::setTextSizeMultiplier(float multiplier)
{
    impl->setTextSizeMultiplier(multiplier);
}

float Root::textSizeMultiplier()
{
    return impl->textSizeMultiplier();
}

bool Root::searchFor(const gchar *string, bool forward, bool caseSensitive)
{
    return impl->searchFor(string, forward, caseSensitive);
}




/** Interface for HTTP/URL credentials. */

/** Interface for HTTP/URL credentials. */
URLCredential::URLCredential(const gchar* user, const gchar* pass, Persistence p)
    :m_user(0)
    ,m_pass(0)
    ,m_pers(p)
{
    assignToString(&m_user, user);	
    assignToString(&m_pass, pass);
}

URLCredential::URLCredential(const URLCredential& other)
    :m_user(0)
    ,m_pass(0)
    ,m_pers(None)    
{
    *this = other;
}

URLCredential& URLCredential::operator=(const URLCredential& other)
{
    assignToString(&m_user, other.m_user);	
    assignToString(&m_pass, other.m_pass);
    m_pers = other.m_pers;

    return *this;
}

URLCredential::~URLCredential()
{
    if (m_user) g_free(m_user);
    if (m_pass) g_free(m_pass);
}

URLProtectionSpace::URLProtectionSpace(const gchar *url, const gchar *realm,
				       URLProtectionSpace::AuthenticationMethod method,
				       URLProtectionSpace::ProxyType proxyType)
    : m_method(method)
    , m_proxyType(proxyType)
    , m_secure(false)    
    , m_url(0)
    , m_realm(0)
    , m_protocol(0)
    , m_host(0)
    , m_port(80)
{
    if (realm) m_realm = g_strdup(realm);

    /** FIXME: TODO: Proper pathname normalization.. */    
    const gchar* cleanurl = url;

    // remove filename
    const gchar* filename = strrchr(cleanurl, '/'); 
    // -1 == not found, 0 == first char
    if (filename) 
	m_url = g_strndup(cleanurl, filename - cleanurl);
    else
	m_url = g_strdup(cleanurl);

    // port
    // m_port = ??

    assignToString(&m_protocol, cleanurl);
    assignToString(&m_host, cleanurl);
}

URLProtectionSpace::URLProtectionSpace(const URLProtectionSpace& other)
    : m_method(Default)
    , m_proxyType(NoProxy)
    , m_secure(false)    
    , m_url(0)
    , m_realm(0)
    , m_protocol(0)
    , m_host(0)
    , m_port(80)
{
    *this = other;
}

URLProtectionSpace& URLProtectionSpace::operator=(const URLProtectionSpace& other)
{
    assignToString(&m_host, other.m_host);
    assignToString(&m_url , other.m_url);
    assignToString(&m_realm , other.m_realm);
    assignToString(&m_protocol , other.m_protocol);

    m_method = other.m_method;
    m_proxyType = other.m_proxyType;
    m_secure = other.m_secure;    
    m_port = other.m_port;
    return *this;
}

URLProtectionSpace::~URLProtectionSpace()
{
    if (m_host) g_free(m_host);
    if (m_protocol) g_free(m_protocol);
    if (m_realm) g_free(m_realm);
    if (m_url) g_free(m_url);    
} 


Preferences::Preferences()
    :d(new PreferencesImpl())
{
}

Preferences::~Preferences()
{
    delete d;
}

void Preferences::setMinimumFontSize(float a)
{
    d->settings->setMinimumFontSize(a);
}

float Preferences::minimumFontSize()
{
    return d->settings->minimumFontSize();    
}

void Preferences::setMinimumLogicalFontSize(float a)
{
    d->settings->setMinimumLogicalFontSize(a);    
}

float Preferences::minimumLogicalFontSize() 
{
    return d->settings->minimumLogicalFontSize();    
}

void Preferences::setDefaultFontSize(float a)
{
    d->settings->setDefaultFontSize(a);    
}

float Preferences::defaultFontSize()
{
    return d->settings->defaultFontSize();    
}

void Preferences::setDefaultFixedFontSize(float a)
{
    d->settings->setDefaultFixedFontSize(a);
}

float Preferences::defaultFixedFontSize(){
    return d->settings->defaultFixedFontSize();    
}
  
void Preferences::setJavaEnabled(bool a)
{
    d->settings->setJavaEnabled(a);
}
bool Preferences::javaEnabled() {
    return d->settings->javaEnabled();
}

void Preferences::setPluginsEnabled(bool a)
{
    d->settings->setPluginsEnabled(a);
}
bool Preferences::pluginsEnabled(){
    return d->settings->pluginsEnabled();
}

void Preferences::setJavaScriptEnabled(bool a)
{
    d->settings->setJavaScriptEnabled(a);
}

bool Preferences::javaScriptEnabled()
{
    return d->settings->javaScriptEnabled();
}

void Preferences::setJavaScriptCanOpenWindowsAutomatically(bool a) 
{
    d->settings->setJavaScriptCanOpenWindowsAutomatically(a);
}

bool Preferences::javaScriptCanOpenWindowsAutomatically()
{
    return d->settings->javaScriptCanOpenWindowsAutomatically();
}

void Preferences::setWillLoadImagesAutomatically(bool a)
{
    d->settings->setWillLoadImagesAutomatically(a);
}

bool Preferences::willLoadImagesAutomatically()
{
    return d->settings->willLoadImagesAutomatically();
}

void Preferences::setShouldPrintBackgrounds(bool a)
{
    d->settings->setShouldPrintBackgrounds(a);
}

bool Preferences::shouldPrintBackgrounds()
{
    return d->settings->shouldPrintBackgrounds();
}

void Preferences::setUserStyleSheetLocation(const gchar* location)
{
    d->settings->setUserStyleSheetLocation(location);
}  

const gchar* Preferences::userStyleSheetLocation()
{
    return d->settings->userStyleSheetLocation();
}

void Preferences::setDefaultTextEncoding(const gchar* encoding)
{
    d->settings->setDefaultTextEncoding(encoding);
}

const gchar* Preferences::defaultTextEncoding()
{
    return  d->settings->defaultTextEncoding();
}

void Preferences::setStandardFontFamily(const gchar* s)
{
    d->settings->setStandardFontFamily(s);    
}

const gchar* Preferences::standardFontFamily()
{
    return 0L;
}

void Preferences::setFixedFontFamily(const gchar* s)
{
    d->settings->setFixedFontFamily(s);
}

const gchar* Preferences::fixedFontFamily()
{
    return d->settings->fixedFontFamily();
}
  
void Preferences::setSerifFontFamily(const gchar* s)
{
    d->settings->setSerifFontFamily(s);
}

const gchar* Preferences::serifFontFamily()
{
    return d->settings->serifFontFamily();
}
  
void Preferences::setSansSerifFontFamily(const gchar* s)
{
    d->settings->setSansSerifFontFamily(s);
}

const gchar* Preferences::sansSerifFontFamily()
{
    return d->settings->sansSerifFontFamily();
}

void Preferences::setCursiveFontFamily(const gchar* s)
{
    d->settings->setCursiveFontFamily(s);
}

const gchar* Preferences::cursiveFontFamily()
{    
    return d->settings->cursiveFontFamily();
}

void Preferences::setFantasyFontFamily(const gchar* s)
{
    d->settings->setFantasyFontFamily(s);
}

const gchar* Preferences::fantasyFontFamily()
{
    return d->settings->fantasyFontFamily();
}
