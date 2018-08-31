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
#include "PageCache.h"
#include "NRCore/KWIQPageCache.h"

static void gcharDestroyFunc(gpointer data)
{
    gchar *c = static_cast<gchar *>(data);
    g_free(c);    
}

static void pageCacheEntryDestroyFunc(gpointer data) {
    PageCacheEntry * entry = static_cast<PageCacheEntry *>(data);
    delete entry;
}

static gboolean pageCacheRemoveExtraEntriesFunc(gpointer key, gpointer value, gpointer data) {
    int * del = static_cast<int *>(data);
    if (*del > 0) {
        --*del;
        return TRUE;
    }
    return FALSE;
}

PageCache::PageCache()
{ 
    pagesByURL = g_hash_table_new_full(g_str_hash, g_str_equal,
                                       gcharDestroyFunc,
                                       pageCacheEntryDestroyFunc);
}

PageCache::~PageCache()
{
    g_hash_table_destroy(pagesByURL);
}

void PageCache::insertByURL(const gchar* url, KWIQPageState* state, const gchar* title)
{
    g_hash_table_remove(pagesByURL, url);
    const unsigned int size = g_hash_table_size(pagesByURL);
    if (size > maxCacheSize) {
        int toDelete = size - maxCacheSize;
        g_hash_table_foreach_remove(pagesByURL, pageCacheRemoveExtraEntriesFunc, &toDelete);
    }
    PageCacheEntry* item = new PageCacheEntry(url, state, title);    
    g_hash_table_insert(pagesByURL, g_strdup(url), item); 
}
	
PageCacheEntry* PageCache::take(const gchar* url)
{

    PageCacheEntry* item = static_cast<PageCacheEntry *>(g_hash_table_lookup(pagesByURL, url)); 
    if (!item) 
	return 0;
    
    // remove page from pagecache, it can be only reffed by the core or by the pagecache, not both
    // don't delete the page. 
    g_hash_table_remove(pagesByURL, url);
    return item;
}
