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
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>
#include "osb-browserglobal.h"
#include "osb-browserglobal-private.h"

#include "osb-browserglobal-marshal.h"
#include "osb-browserglobal-settings.h"


/* here are local prototypes */
static void _osb_browserglobal_class_init (OSBBrowserGlobalClass * c);

static void _osb_browserglobal_obj_init (OSBBrowserGlobal * self);
static void _osb_browserglobal_obj_finalize (GObject * self);


enum {
	BROWSER_CREATED_SIGNAL,
	BROWSER_DESTROYED_SIGNAL,
	LAST_SIGNAL
};

static guint object_signals[LAST_SIGNAL] = {0};


/* pointer to the class of our parent */
static GObjectClass *parent_class = NULL;

GType
osb_browserglobal_get_type ()
{
  static GType type = 0;
  
  if G_UNLIKELY(type == 0) {
    static const GTypeInfo info = {
      sizeof (OSBBrowserGlobalClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) _osb_browserglobal_class_init,
      (GClassFinalizeFunc) NULL,
      NULL /* class_data */,
      sizeof (OSBBrowserGlobal),
      0 /* n_preallocs */,
      (GInstanceInitFunc) _osb_browserglobal_obj_init,
    };

    type = g_type_register_static (G_TYPE_OBJECT, "OSBBrowserGlobal", &info, (GTypeFlags)0);
  }

  return type;
}

/* class meta object */
static void 
_osb_browserglobal_class_init (OSBBrowserGlobalClass * c)
{
  GObjectClass *g_object_class = (GObjectClass*) c;
  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_signals[BROWSER_CREATED_SIGNAL] =
    g_signal_new ("browser-created",
		  G_TYPE_FROM_CLASS (g_object_class),
		  (GSignalFlags)(G_SIGNAL_RUN_LAST),
		  G_STRUCT_OFFSET (OSBBrowserGlobalClass, browser_created),
		  NULL, NULL,
		  osb_browserglobal_marshal_VOID__POINTER,
		  G_TYPE_NONE,
		  1,
		  G_TYPE_POINTER);
  
  object_signals[BROWSER_DESTROYED_SIGNAL] =
    g_signal_new ("browser-destroyed",
		  G_TYPE_FROM_CLASS (g_object_class),
		  (GSignalFlags)(G_SIGNAL_RUN_LAST),
		  G_STRUCT_OFFSET (OSBBrowserGlobalClass, browser_destroyed),
		  NULL, NULL,
		  osb_browserglobal_marshal_VOID__POINTER,
		  G_TYPE_NONE,
		  1,
		  G_TYPE_POINTER); 
  
  g_object_class->finalize = _osb_browserglobal_obj_finalize;
  return;
}

#if 0 				/* not used */
static void 
_osb_browserglobal_class_finalize (GObject * self)
{
  
}
#endif


/* object */

static void 
_osb_browserglobal_obj_init (OSBBrowserGlobal * self)
{
  int group_len = 20;

  self->_priv = g_new0 (OSBBrowserGlobalPrivate, 1);
  self->_priv->browsers = 0;
  self->settings = osb_browserglobal_settings_alloc();
  
  self->group = g_new0(gchar, group_len);
  srand((int)time(NULL));
  g_snprintf(self->group, group_len, "%d", rand());

  return;
}

static void
_osb_browserglobal_obj_finalize(GObject *obj)
{
  OSBBrowserGlobal *self = OSB_BROWSERGLOBAL (obj);
  g_free(self->group);
  osb_browserglobal_settings_dealloc(self->settings);
  self->settings = 0;
  osb_browserglobal_destroy_all_browsers (self);

  /* chain finalize */
  if(G_OBJECT_CLASS(parent_class)->finalize) 
    (* G_OBJECT_CLASS(parent_class)->finalize) (obj);  

  g_free (self->_priv);
  return;
}

GObject * 
osb_browserglobal_new ()
{
    return (GObject *) ((OSBBrowserGlobal *)g_object_new(osb_browserglobal_get_type(), NULL));
}
const gchar*
osb_browserglobal_get_group(OSBBrowserGlobal* self)
{
  return self->group;
}

const gchar*
osb_browserglobal_get_status(OSBBrowserGlobal* self)
{
  return self->status;
}

void
osb_browserglobal_set_status(OSBBrowserGlobal* self, const gchar * status)
{
  if (self->status) g_free(self->status);
  self->status = g_strdup(status);
}

void
osb_browserglobal_browser_created (OSBBrowserGlobal* self, gpointer browser)
{
  GList *node = g_list_find (self->_priv->browsers, browser);
  
  if (!node) {
    self->_priv->browsers = g_list_append (self->_priv->browsers, browser);
    g_signal_emit_by_name (G_OBJECT(self),"browser-created", browser);
  }
}

void
osb_browserglobal_browser_destroyed (OSBBrowserGlobal* self, gpointer browser)
{
  GList *node = g_list_find (self->_priv->browsers, browser);
  
  if (node) {
    g_signal_emit_by_name (G_OBJECT(self),"browser-destroyed", browser);
    self->_priv->browsers = g_list_delete_link (self->_priv->browsers, node);
  }
}


void
osb_browserglobal_destroy_all_browsers (OSBBrowserGlobal* self)
{
  if (self->_priv->browsers) {
    GList* browsers = g_list_copy (self->_priv->browsers);
    GList* b = browsers;
    while (b) {
      gtk_widget_destroy (GTK_WIDGET (b->data));
      b = b->next;
    }
    g_list_free (browsers);
  }
  if (self->_priv->browsers) {
    g_list_free (self->_priv->browsers);
    self->_priv->browsers = 0;
  }
  
  return;
}

