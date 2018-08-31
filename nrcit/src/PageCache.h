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
#ifndef PAGECACHE_H
#define PAGECACHE_H

#include <glib.h>

#include "NRCore/KWIQPageCache.h" // KWIQPageCache

class KWIQPageState;



class PageCacheEntry : public KWIQPageCache
{
    KWIQPageState* _state;
    gchar* _title;
    gchar* _url;
public:
    PageCacheEntry(const gchar* aurl, KWIQPageState* astate, const gchar* atitle)
      :_state(astate), _title(g_strdup(atitle)),_url(g_strdup(aurl))
    {}

    ~PageCacheEntry()
    {g_free(_title); g_free(_url); delete _state;}

    KWIQPageState* state() 
    { return _state;}

    const gchar* title() { return _title;}
    const gchar* URL() { return _url;}
};

class PageCache
{
 public:
    PageCache();
    ~PageCache();
    
    void insertByURL(const gchar* url, KWIQPageState*, const gchar* title);
	
    PageCacheEntry* take(const gchar* url);

 private:
    static const unsigned int minCacheSize = 20;
    static const unsigned int maxCacheSize = 40;

    GHashTable* pagesByURL;    
};

#endif
