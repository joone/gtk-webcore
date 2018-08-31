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
#ifndef osbimpl_h
#define osbimpl_h

#include <glib.h>
#include "osb.h"

namespace OSB {
  class URLCredential;
};

/** Dictionary containing mappings from Realm -> Credentials.
 * At the moment we support only one credential per realm.
 */
//typedef GHashTable* RealmDict; 

/** Interface for the credential storage. */
class URLCredentialStorageImpl : public OSB::URLCredentialStorage
{
public:
    /** Returns dictionary of all credentials */
    // virtual const NSDictionary allCredentials();
    
    /** Get default credentials for protection space. */
    const OSB::URLCredential * defaultCredential(const OSB::URLProtectionSpace &) const;
    
    /** Set the default credentials for protection space. */
    void setDefaultCredential(const OSB::URLCredential &,
                              const OSB::URLProtectionSpace &);

    /** Get all credentials for protection space in a
        dictionary.  Usernames work as keys and credentials
        as values. */
/*     virtual const NSDictionary * */
/*         credential(URLProtectionSpace *) = 0; */

    /** Set the default credentials for protection space. */
    void setCredential(const OSB::URLCredential &credential,
                       const OSB::URLProtectionSpace &space) { setDefaultCredential(credential, space); }    
    
    /** Removes credential from the credential storage
        for the specified protectionSpace. */
    void removeCredential(const OSB::URLCredential &,
                          const OSB::URLProtectionSpace &);

    /** Creates the URLCredentialStorage. */
    URLCredentialStorageImpl();
    virtual ~URLCredentialStorageImpl() {}
    
    
private:
    GHashTable*  m_storage;

    /** Stores the mappings from host:port/path to realm. */
    GHashTable* m_defaultRealms;
};


#endif
