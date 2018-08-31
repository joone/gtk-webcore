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
#include "osb-browserglobal-settings.h"

static void
_free_str(gpointer data)
{
  gchar* d = (gchar*) data;
  g_free(d);
}

/* object */

OSBBrowserGlobalSettings * osb_browserglobal_settings_alloc ()
{
  OSBBrowserGlobalSettings* self;
  self = g_new0(OSBBrowserGlobalSettings, 1);  
  self->_settings = g_hash_table_new_full (g_str_hash, g_str_equal, _free_str, _free_str);
  return self;
}

void osb_browserglobal_settings_dealloc (OSBBrowserGlobalSettings * self)
{
  g_hash_table_destroy (self->_settings);
  self->_settings = 0;
  g_free(self);  
}

void
osb_browserglobal_settings_set_string (OSBBrowserGlobalSettings* self, const gchar* entry, const gchar* value)
{
  g_return_if_fail (self);
  g_return_if_fail (entry);

  if (value == NULL) {
    g_hash_table_remove (self->_settings, entry);
    return;
  }

  g_hash_table_insert (self->_settings, g_strdup (entry), g_strdup (value));  
  
  return;
}


const gchar*
osb_browserglobal_settings_get_string (OSBBrowserGlobalSettings* self, const gchar* entry)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (entry, NULL);

  return (const gchar*) g_hash_table_lookup (self->_settings, entry);
}
