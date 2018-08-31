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
#ifndef __OSB_BROWSERGLOBAL_H__
#define __OSB_BROWSERGLOBAL_H__

#include <glib.h>
#include <glib-object.h>
#include "osb-browserglobal-settings.h"

/* class OSBBrowserGlobal 
 * Singleton class for osb-browser.
 *
 * Class to implement osb-browser system wide, "inter-window"
 * communication.
 *
 * Currently is just a dummy implementation, but here for future
 * use.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define OSB_TYPE_BROWSERGLOBAL	(osb_browserglobal_get_type())
#define OSB_BROWSERGLOBAL(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), osb_browserglobal_get_type(), OSBBrowserGlobal)
#define OSB_BROWSERGLOBAL_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), osb_browserglobal_get_type(), OSBBrowserGlobal const)
#define OSB_BROWSERGLOBAL_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), osb_browserglobal_get_type(), OSBBrowserGlobalClass)
#define OSB_IS_BROWSERGLOBAL(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), osb_browserglobal_get_type ())

#define OSB_BROWSERGLOBAL_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), osb_browserglobal_get_type(), OSBBrowserGlobalClass)

/* Private structure type */
typedef struct _OSBBrowserGlobalPrivate OSBBrowserGlobalPrivate;

/*
 * Main object structure
 */
typedef struct _OSBBrowserGlobal OSBBrowserGlobal;
struct _OSBBrowserGlobal {
  GObject __parent__;  
  OSBBrowserGlobalSettings *settings;

  /** A boolean flag for testing only that specifies whether we
   * should print rendering tree instead of launching browser. */
  gboolean renderOnly;
  gchar* group;
  /** Status text of the browser. Status text set by the browser
   * from javascript or during page load. */
  gchar* status;
  
  /*< private >*/
  OSBBrowserGlobalPrivate *_priv;
};

/*
 * Class definition
 */
typedef struct _OSBBrowserGlobalClass OSBBrowserGlobalClass;
struct _OSBBrowserGlobalClass {
  GObjectClass __parent__;
  
  /*signal*/
  void (* browser_created) (OSBBrowserGlobal* self, gpointer);
  void (* browser_destroyed) (OSBBrowserGlobal* self, gpointer);
  void (* settings_changed) (OSBBrowserGlobal* self);
};


/*
 * Public methods
 */
GType osb_browserglobal_get_type ();
GObject* osb_browserglobal_new ();

const gchar* osb_browserglobal_get_group(OSBBrowserGlobal*);
const gchar* osb_browserglobal_get_status(OSBBrowserGlobal*);
void osb_browserglobal_set_status(OSBBrowserGlobal* self, const gchar * status);



void osb_browserglobal_destroy_all_browsers (OSBBrowserGlobal*);
gint osb_browserglobal_register_plugin(OSBBrowserGlobal* self, const gchar* path);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
