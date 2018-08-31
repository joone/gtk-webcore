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
#ifndef __OSB_BROWSER_PRIVATE_H__
#define __OSB_BROWSER_PRIVATE_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <gtk-khtml.h>

#include "osb-browserglobal.h"
#include "osb-browser.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _OSBBrowserPrivate {
  OSBBrowserGlobal* global;
  GtkNotebook* tab;
  GladeXML *xml;
  
};

typedef enum
{
  PATH_UI_XML_DESCRIPTION,  
  PATH_DEFAULT_LOCATION_ICON,
  STRING_BROWSER_NAME,
  URL_HOME,
  INT_TAB_TITLE_LEN,
  INT_INITIAL_WIDTH,
  INT_INITIAL_HEIGHT,
  LAST_CONFIG_ITEM
} ConfigItem;

/* private methods for OSBBrowser */
/** */
GtkWidget * osb_browser_new_with_engine (OSBBrowserGlobal * global, GtkWidget* html_engine);
       
/** */
const gchar* osb_browser_config_get_string(OSBBrowser* self, ConfigItem item);
  
/** */
int osb_browser_config_get_int(OSBBrowser* self, ConfigItem item);
  
/** */
void osb_browser_set_location_icon(OSBBrowser* self, const gchar* path);
/** */
GtkKHTML* osb_browser_get_active_engine (OSBBrowser* self);
/** */
void osb_browser_connect_engine_signals(OSBBrowser* self, GtkKHTML *engine);

/** for global ui items, like title and location entry*/
void osb_browser_update_ui(OSBBrowser* self, GtkKHTML* engine);

/** for local things, like tab label*/
void osb_browser_update_ui_local(OSBBrowser* self, GtkKHTML* engine);

/** sets settings from global settings object*/
void osb_browser_set_settings(OSBBrowser* self, GtkKHTML* engine);

/** Prints out rendering tree. */
void osb_browser_print_render_tree(OSBBrowser* self);
  
/** Normalizes the filename into an absolute file, if one is required. */
gchar * normalize_filename(const gchar* file);

/** sets the title of the browser window */
void osb_browser_set_title (OSBBrowser* self, const gchar* title);

/** sets the location of the browser window */
void osb_browser_set_location (OSBBrowser* self, const gchar* location);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
