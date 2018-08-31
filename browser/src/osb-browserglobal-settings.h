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
#ifndef __OSB_BROWSERGLOBAL_SETTINGS_H__
#define __OSB_BROWSERGLOBAL_SETTINGS_H__

#include <glib.h>
#include <glib-object.h>

/** Global settings structure for osb-browser
 *
 * Global settings for osb-browser
 */


#define OSB_BROWSERGLOBAL_SETTINGS_JAVASCRIPT "java_script_enabled"
#define OSB_BROWSERGLOBAL_SETTINGS_JAVA "java_enabled"
#define OSB_BROWSERGLOBAL_SETTINGS_PLUGINS "pluginsEnabled"
#define OSB_BROWSERGLOBAL_SETTINGS_AUTOLOADIMAGES "autoLoadImages"
#define OSB_BROWSERGLOBAL_SETTINGS_MINIMUMFONTSIZE "minimumFontSize"
#define OSB_BROWSERGLOBAL_SETTINGS_DEFAULTFONTSIZE "defaultFontSize"
#define OSB_BROWSERGLOBAL_SETTINGS_DEFAULTFIXEDFONTSIZE "defaultFixedFontSize"
#define OSB_BROWSERGLOBAL_SETTINGS_DEFAULTTEXTENCODING "defaultTextEncoding"
#define OSB_BROWSERGLOBAL_SETTINGS_SERIFFONTFAMILY "serifFontFamily"
#define OSB_BROWSERGLOBAL_SETTINGS_SANSSERIFFONTFAMILY "sansSerifFontFamily"
#define OSB_BROWSERGLOBAL_SETTINGS_FIXEDFONTFAMILY "fixedFontFamily"
#define OSB_BROWSERGLOBAL_SETTINGS_STANDARDFONTFAMILY "standardFontFamily"
#define OSB_BROWSERGLOBAL_SETTINGS_CURSIVEFONTFAMILY "cursiveFontFamily"
#define OSB_BROWSERGLOBAL_SETTINGS_FANTASYFONTFAMILY "fantasyFontFamily"
#define OSB_BROWSERGLOBAL_SETTINGS_HTTPPROXY "httpProxy"


typedef struct _OSBBrowserGlobalSettings OSBBrowserGlobalSettings;

/*
 * Main object structure
 */
struct _OSBBrowserGlobalSettings {
  GHashTable *_settings;
};

OSBBrowserGlobalSettings* osb_browserglobal_settings_alloc ();
void osb_browserglobal_settings_dealloc(OSBBrowserGlobalSettings* self);

const gchar* osb_browserglobal_settings_get_string(OSBBrowserGlobalSettings* self, const gchar* entry);
void osb_browserglobal_settings_set_string(OSBBrowserGlobalSettings* self, const gchar* entry, const gchar* value );

#endif
