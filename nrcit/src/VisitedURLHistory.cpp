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
#include "VisitedURLHistory.h"

/** 
 * class for coloring vlinks
 * used in cssstyleselector.cpp:511
 * checkPseudoState(..)
 *
 * problem with this is that contains() gets the link in html href, thus it can be relative..
 */
extern "C" {
void vlinks_value_destroy(gpointer data)
{
    /* empty*/
}

void vlinks_key_destroy(gpointer data)
{
    gchar *c = static_cast<gchar*>(data);
    g_free(c);
}
#if 0
gboolean stealfunc(gpointer key, gpointer value, gpointer user_data)
{
    g_warning("visited url: %s", (gchar*)key);
    return FALSE;
}
#endif
}

VisitedURLHistory::VisitedURLHistory()
  : _locked(false) 
{
    vlinks = g_hash_table_new_full(g_str_hash, g_str_equal,
                                   vlinks_key_destroy,
				   vlinks_value_destroy);
}

VisitedURLHistory::~VisitedURLHistory()
{
    g_hash_table_destroy(vlinks);
}

VisitedURLHistory* VisitedURLHistory::sharedProvider()
{
    static VisitedURLHistory singleton;
    return &singleton;
}

void VisitedURLHistory::useAsSharedProvider()
{
    WebCoreHistory::setHistoryProvider(sharedProvider());
}

void VisitedURLHistory::insertVisitedURL(const gchar* url)
{
    if (!url) return;
    gpointer ptr = g_hash_table_lookup(vlinks, url);
    if (!ptr) {
	g_hash_table_insert(vlinks, g_strdup(url), (gpointer) 0x1);
    }
#if 0
    g_hash_table_foreach_steal(vlinks, stealfunc, this);
#endif
}

bool VisitedURLHistory::containsItemForURLLatin1(const gchar* url, int len)
{
    return containsItemForURLUTF8(url, len);
}

bool VisitedURLHistory::containsItemForURLUTF8(const gchar* url, int len)
{
    if (!url || !len) return false;
    gpointer ptr = g_hash_table_lookup(vlinks, url);
#if 0
    g_warning("contains url:%s, %d", url, ptr!=NULL);
#endif
    return ptr ? TRUE : FALSE;
}






