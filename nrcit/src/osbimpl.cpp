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

#include "osb.h"
#include "osbimpl.h"
#include "prefimpl.h"

#include "Http.h"
#include "BridgeImpl.h"

// FIXME: REMOVEME
#include "NRCore/WebCoreSettings.h"

/** Interface for HTTP/URL credentials. */
URLCredentialStorageImpl::URLCredentialStorageImpl()
{
#if 0
    // ensure cleanup
    m_storage.setAutoDelete(true);
    m_defaultRealms.setAutoDelete(true);
#endif
}
/** Interface for the credential storage. */
const OSB::URLCredential *
URLCredentialStorageImpl::defaultCredential(const OSB::URLProtectionSpace &space) const
{
#if 0
    GHashTable* realm = static_cast<GHashTable*>(g_hash_table_lookup(m_storage, space.host()));
    QString realm(space.realm());

    if (rdict) {
        if (!realm.isEmpty()) {
            return rdict->find(realm);
        } else {
            // search path -> realm mappings first.
            QString * defRealm = m_defaultRealms.find(space.hostPath());
            if (defRealm) {
                return rdict->find(*defRealm);
            }
        }
    }
#endif
    return 0L;
}
    
/** Set the default credentials for protection space. */
void
URLCredentialStorageImpl::setDefaultCredential(const OSB::URLCredential &cred,
                                               const OSB::URLProtectionSpace &space)
{
#if 0
    // add host -> realm -> credential mapping
    RealmDict * rdict = m_storage.find( space.host() );
    if ( !rdict ) {
        rdict = new RealmDict();
        m_storage.insert( space.host(), rdict );
    }
    OSB::URLCredential *newCred = new OSB::URLCredential(cred.user(), cred.password(), cred.persistence());
    rdict->replace(space.realm(), newCred);

    // now do the same with path -> realm mappings
    QString *defaultRealm = m_defaultRealms.find(space.hostPath());

    // create os update default realm for path
    if (!defaultRealm || *defaultRealm != space.realm()) {
        m_defaultRealms.insert(space.hostPath(), new QString(space.realm()));
    }
#endif
}
    

/** Removes credential from the credential storage
    for the specified protectionSpace. */
void
URLCredentialStorageImpl::removeCredential(const OSB::URLCredential & /* cred */, 
                                           const OSB::URLProtectionSpace &space)
{
#if 0
    RealmDict * rdict = m_storage.find(space.host());
    if (rdict) {
        rdict->remove(space.realm());
    }

    // remove default mappings.
    m_defaultRealms.remove(space.hostPath());
#endif
}


void
PreferencesImpl::copyToWebCoreSettings(WebCoreSettings* to) const
{
    assert(to);
    to->setMinimumFontSize(settings->minimumFontSize());
    to->setMinimumLogicalFontSize(settings->minimumLogicalFontSize());
    to->setDefaultFontSize(settings->defaultFontSize());
    to->setJavaEnabled(settings->javaEnabled());
    to->setJavaScriptEnabled(settings->javaScriptEnabled());
    to->setPluginsEnabled(settings->pluginsEnabled());
    to->setJavaScriptCanOpenWindowsAutomatically(settings->javaScriptCanOpenWindowsAutomatically());
    to->setWillLoadImagesAutomatically(settings->willLoadImagesAutomatically());
    to->setShouldPrintBackgrounds(settings->shouldPrintBackgrounds());
    to->setUserStyleSheetLocation(settings->userStyleSheetLocation());
    to->setDefaultTextEncoding(settings->defaultTextEncoding());
    to->setStandardFontFamily(settings->standardFontFamily());
    to->setFixedFontFamily( settings->fixedFontFamily());      
    to->setSerifFontFamily( settings->serifFontFamily());    
    to->setSansSerifFontFamily(settings->sansSerifFontFamily());    
    to->setCursiveFontFamily(settings->cursiveFontFamily());    
    to->setFantasyFontFamily(settings->fantasyFontFamily());
}
