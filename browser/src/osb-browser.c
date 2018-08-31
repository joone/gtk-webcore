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
#include "osb-browser.h"
#include "osb-browser-private.h"
#include <stdlib.h>		/* strtod */
#include <string.h> 		/* strlen */
#include <assert.h> 		/* assert */
#include <glib.h>
#include <glib/gprintf.h>       /* g_printf */
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "osb-browserglobal.h"
#include "osb-browserglobal-private.h"

#include "gtk-khtml.h"

#include "callbacks.h"

#ifndef _
#define _(x) (x)
#endif

static void _osb_browser_class_init (OSBBrowserClass * c);

static void _osb_browser_obj_init (OSBBrowser * self);
static void _osb_browser_obj_finalize (GObject * self);

static GtkWindowClass *parent_class = NULL;

GType
osb_browser_get_type (void)
{
  static GType type = 0;
  
  if G_UNLIKELY(type == 0) {
    static const GTypeInfo info = {
      sizeof (OSBBrowserClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) _osb_browser_class_init,
      (GClassFinalizeFunc) NULL,
      NULL /* class_data */,
      sizeof (OSBBrowser),
      0 /* n_preallocs */,
      (GInstanceInitFunc) _osb_browser_obj_init,
    };
    
    type = g_type_register_static (GTK_TYPE_WINDOW, "OSBBrowser", &info, (GTypeFlags)0);
  }

  return type;
}


static void 
_osb_browser_class_init (OSBBrowserClass * c)
{
  GObjectClass *g_object_class = (GObjectClass*) c;
  parent_class = (GtkWindowClass *)g_type_class_ref (GTK_TYPE_WINDOW);

  g_object_class->finalize = _osb_browser_obj_finalize;
  return;
}


/* object */

static void 
_osb_browser_obj_init (OSBBrowser * self)
{
  self->_priv = g_new0 (OSBBrowserPrivate, 1);
  self->_priv->global = 0;
  return;
}


static void
_osb_browser_obj_finalize(GObject *obj)
{
  OSBBrowser *self = OSB_BROWSER (obj);
  gpointer priv = self->_priv;
  g_warning("finalize");
  g_free (priv);
  
  if(G_OBJECT_CLASS(parent_class)->finalize) 
    (* G_OBJECT_CLASS(parent_class)->finalize)(obj);
  
  return;  
}
#if DEBUG
static void
_weak_notify(gpointer data, GObject *was)
{
  g_warning("Destroyed: %s at %x\n", (const char*) data, (int) was);
}


static void
_attach_notify(GObject *obj, const char* msg)
{
  g_object_weak_ref( obj, (GWeakNotify)_weak_notify, (gpointer) msg);
}

#endif

static gboolean
_get_bool(const gchar* value)
{
  if (value == NULL) return FALSE;
  
  if (strcmp (value, "1") == 0)
    return TRUE;
  
  return FALSE;
}

static gfloat
_get_float (const gchar* value)
{
  gchar* endptr = NULL;
  gfloat val;
  
  if (value == NULL) return -1; 

  val  = (float) strtod(value, &endptr);
  if (endptr == value) {
    return -1;
  }
  return val;
}

static const gchar*
_get_string (const gchar* value)
{
  if (value == NULL) return NULL;
  return value;
}

/*private*/
void
osb_browser_set_settings (OSBBrowser* self, GtkKHTML* engine)
{
  const gchar* http_proxy;
  
#define ENSURE_GOOD(var, fallback) \
  if ((var)<1) var = fallback;

  OSBBrowserGlobalSettings* settings = self->_priv->global->settings;
  GtkKHTMLSettings s = {0,};
  GtkKHTMLSettings *ks = &s;
  
  ks->javascript_enabled =
    _get_bool (osb_browserglobal_settings_get_string (settings,
						      OSB_BROWSERGLOBAL_SETTINGS_JAVASCRIPT));
  
  ks->java_enabled =
    _get_bool (osb_browserglobal_settings_get_string (settings,
						      OSB_BROWSERGLOBAL_SETTINGS_JAVA)); 

  ks->plugins_enabled =
    _get_bool (osb_browserglobal_settings_get_string (settings,
						      OSB_BROWSERGLOBAL_SETTINGS_PLUGINS)); 

  ks->autoload_images =
    _get_bool (osb_browserglobal_settings_get_string (settings,
						      OSB_BROWSERGLOBAL_SETTINGS_AUTOLOADIMAGES)); 


  ks->minimum_font_size =
    _get_float (osb_browserglobal_settings_get_string (settings,
						       OSB_BROWSERGLOBAL_SETTINGS_MINIMUMFONTSIZE));
  
  ENSURE_GOOD(ks->minimum_font_size, 7);

  ks->default_font_size =
    _get_float (osb_browserglobal_settings_get_string (settings,
						       OSB_BROWSERGLOBAL_SETTINGS_DEFAULTFONTSIZE));

  ENSURE_GOOD(ks->default_font_size, 7);

  ks->default_fixed_font_size =
    _get_float (osb_browserglobal_settings_get_string (settings,
						       OSB_BROWSERGLOBAL_SETTINGS_DEFAULTFIXEDFONTSIZE));
  ENSURE_GOOD(ks->default_fixed_font_size, 7);

  ks->default_text_encoding = 
    _get_string (osb_browserglobal_settings_get_string (settings,
							OSB_BROWSERGLOBAL_SETTINGS_DEFAULTTEXTENCODING));
  
  ks->serif_font_family = 
    _get_string (osb_browserglobal_settings_get_string (settings,
							OSB_BROWSERGLOBAL_SETTINGS_SERIFFONTFAMILY));
  
  ks->sans_serif_font_family = 
    _get_string (osb_browserglobal_settings_get_string (settings,
							OSB_BROWSERGLOBAL_SETTINGS_SANSSERIFFONTFAMILY));

  ks->fixed_font_family = 
    _get_string (osb_browserglobal_settings_get_string (settings,
							OSB_BROWSERGLOBAL_SETTINGS_FIXEDFONTFAMILY));

  ks->standard_font_family = 
    _get_string (osb_browserglobal_settings_get_string (settings,
							OSB_BROWSERGLOBAL_SETTINGS_STANDARDFONTFAMILY));

  ks->cursive_font_family = 
      _get_string (osb_browserglobal_settings_get_string (settings,
							  OSB_BROWSERGLOBAL_SETTINGS_CURSIVEFONTFAMILY));

  ks->fantasy_font_family = 
      _get_string (osb_browserglobal_settings_get_string (settings,
							  OSB_BROWSERGLOBAL_SETTINGS_FANTASYFONTFAMILY));
  
  http_proxy = g_getenv ("http_proxy");
  if (!http_proxy)
    http_proxy = g_getenv ("HTTP_PROXY");
  
  if (!http_proxy) {
    /* TODO: falback to what*/
    http_proxy = "" ;
  }
  
  osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_HTTPPROXY, http_proxy);
  ks->http_proxy = g_strdup(http_proxy);

  gtk_khtml_set_settings (engine, ks);
  
#undef ENSURE_GOOD
}

GtkWidget * 
osb_browser_new (OSBBrowserGlobal * global)
{
    GtkWidget *html_engine;
    html_engine = gtk_khtml_new ();
    gtk_khtml_set_group(GTK_KHTML (html_engine), osb_browserglobal_get_group(global));
    gtk_khtml_set_emit_internal_status (GTK_KHTML (html_engine), TRUE);
    return osb_browser_new_with_engine (global, html_engine);
}

/*private*/
GtkWidget * 
osb_browser_new_with_engine (OSBBrowserGlobal * global, GtkWidget* html_engine)
{
  static gboolean _settings_initialized = FALSE;
  const gchar* path;
  OSBBrowser* self;
  GtkWidget *root_widget;
  GtkWidget *label;
  const gchar *title;
    
  self = ((OSBBrowser *)g_object_new (osb_browser_get_type(),  NULL));

  self->_priv->tab = 0;
  self->_priv->global = global;


  if (!_settings_initialized) {
    gchar* plugin_path;
    /* FIXME: REPLACE ME WITH SETTINGS SAVING/LOADING FUNCTIONALITY*/
    OSBBrowserGlobalSettings* settings = global->settings;
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_JAVASCRIPT, "1");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_JAVA, "0");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_PLUGINS, "1");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_AUTOLOADIMAGES , "1");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_MINIMUMFONTSIZE, "10.0");

    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_DEFAULTFONTSIZE, "16.0");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_DEFAULTFIXEDFONTSIZE, "10.0");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_DEFAULTTEXTENCODING, "ISO-8859-1");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_SERIFFONTFAMILY, "serif");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_SANSSERIFFONTFAMILY, "sans");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_FIXEDFONTFAMILY, "fixed");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_STANDARDFONTFAMILY, "times");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_CURSIVEFONTFAMILY, "times");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_FANTASYFONTFAMILY, "lucidabright");
    osb_browserglobal_settings_set_string (settings, OSB_BROWSERGLOBAL_SETTINGS_HTTPPROXY, "");

    plugin_path = g_strconcat (g_get_home_dir (), "/.osb-browser/plugins/", "libflashplayer.so", NULL);
    if (osb_browserglobal_register_plugin (global, plugin_path) == -1) {
      g_warning ("couldn't register plugin: %s", plugin_path);
    }
    g_free(plugin_path);

    plugin_path = g_strconcat (g_get_home_dir (), "/.osb-browser/plugins/", "libNPSVG3.so", NULL);
    if (osb_browserglobal_register_plugin (global, plugin_path) == -1) {
      g_warning ("couldn't register plugin: %s", plugin_path);
    }
    g_free(plugin_path);


  }
  
  /* load the interface */
  path = osb_browser_config_get_string (self, PATH_UI_XML_DESCRIPTION);

  self->_priv->xml = glade_xml_new (path, NULL, NULL);
    
  if (!self->_priv->xml) {
    g_warning ("Couldn't load ui description file from '%s'", path);
    return 0; /* leak*/
  }
  
  

  /* connect the signals in the interface */
#define CONNECT_WITH_DATA(__s, __h, __d)\
  glade_xml_signal_connect_data (self->_priv->xml, __s, G_CALLBACK( __h), __d);
  
  /* toolbar */
  CONNECT_WITH_DATA ("on_back_clicked", on_back_clicked, self);
  CONNECT_WITH_DATA ("on_forward_clicked", on_forward_clicked, self);
  CONNECT_WITH_DATA ("on_stop_clicked", on_stop_clicked, self);
  CONNECT_WITH_DATA ("on_refresh_clicked", on_refresh_clicked, self);
  CONNECT_WITH_DATA ("on_home_clicked", on_home_clicked, self);
  CONNECT_WITH_DATA ("on_exec_clicked", on_exec_clicked, self);
  /* location */
  CONNECT_WITH_DATA ("on_location_activate", on_location_activate, self);
  CONNECT_WITH_DATA ("on_location_location_key_release_event", on_location_key_release_event, self);

  /* menus */
  CONNECT_WITH_DATA ("on_file_new_window_activate", on_file_new_window_activate, self);
  CONNECT_WITH_DATA ("on_file_new_tab_activate", on_file_new_tab_activate, self);
  CONNECT_WITH_DATA ("on_file_open_activate", on_file_open_activate, self);
  CONNECT_WITH_DATA ("on_file_close_activate", on_file_close_activate, self);
  CONNECT_WITH_DATA ("on_file_quit_activate", on_file_quit_activate, self);
  
  CONNECT_WITH_DATA ("on_edit_find_activate", on_edit_find_activate, self);
  CONNECT_WITH_DATA ("on_edit_find_again_activate", on_edit_find_again_activate, self);

  CONNECT_WITH_DATA ("on_view_zoom_in_activate", on_view_zoom_in_activate, self);
  CONNECT_WITH_DATA ("on_view_zoom_out_activate", on_view_zoom_out_activate, self);
  CONNECT_WITH_DATA ("on_view_zoom_default_activate", on_view_zoom_default_activate, self);
  
  CONNECT_WITH_DATA ("on_go_back_activate", on_go_back_activate, self);
  CONNECT_WITH_DATA ("on_go_forward_activate", on_go_forward_activate, self);
  CONNECT_WITH_DATA ("on_go_home_activate", on_go_home_activate, self);

  CONNECT_WITH_DATA ("on_rendering_tree_activate", on_rendering_tree_activate, self);

  /* tab */
  CONNECT_WITH_DATA ("on_engine_container_switch_page", on_engine_container_switch_page, self);
  
  /* find dialog */
  CONNECT_WITH_DATA ("on_find_find_clicked", on_find_find_clicked, self);
  CONNECT_WITH_DATA ("on_find_close_clicked", on_find_close_clicked, self);
  CONNECT_WITH_DATA ("on_find_delete_event", on_find_delete_event, self);
  
#undef CONNECT_WITH_DATA

  /* connect the signals in the top level window */
  g_signal_connect (G_OBJECT (self),
		    "destroy",
		    G_CALLBACK (on_browser_destroy),
		    self);
  
  g_signal_connect (G_OBJECT (self),
		    "delete_event",
		    G_CALLBACK (on_browser_delete_event),
		    self);

  path = osb_browser_config_get_string (self, PATH_DEFAULT_LOCATION_ICON);
    
  if (path) {
    osb_browser_set_location_icon (self, path);
  }

  root_widget  = glade_xml_get_widget (self->_priv->xml, "browser_root");
  gtk_widget_reparent (root_widget, GTK_WIDGET (self));

  self->_priv->tab = GTK_NOTEBOOK (gtk_notebook_new());
  gtk_notebook_set_show_tabs (self->_priv->tab, FALSE);
  
  gtk_container_add (GTK_CONTAINER (glade_xml_get_widget (self->_priv->xml, "view_container")),
		     GTK_WIDGET (self->_priv->tab));
  gtk_widget_show (GTK_WIDGET (self->_priv->tab));

  osb_browser_set_settings(self, GTK_KHTML (html_engine));
#if DEBUG
  /*debug*/  
  _attach_notify (G_OBJECT(html_engine), "khtml");
  _attach_notify (G_OBJECT(self), "OSBBrowser");
#endif
  
  osb_browser_connect_engine_signals (self, GTK_KHTML (html_engine));
  
  title = _("Untitled");
  label = gtk_label_new (title);
  gtk_notebook_append_page (self->_priv->tab, html_engine, label);
  
  gtk_notebook_set_tab_label_packing (self->_priv->tab, html_engine,
				      TRUE, TRUE, GTK_PACK_START);

  gtk_widget_show_all (html_engine);

  if (self->_priv->global->renderOnly) {
    gtk_window_set_default_size (GTK_WINDOW (self), 822, 718);    
  } else {
    gtk_window_set_default_size (GTK_WINDOW (self),
                                 osb_browser_config_get_int (self, INT_INITIAL_WIDTH),
                                 osb_browser_config_get_int (self, INT_INITIAL_HEIGHT));
  }
      
  osb_browser_update_ui(self, GTK_KHTML (html_engine));
  osb_browser_update_ui_local(self, GTK_KHTML (html_engine));
    
  /* inform our global, which keeps track of browser objects */
  osb_browserglobal_browser_created (self->_priv->global, self);
  
  return (GtkWidget*) self;
}

void
osb_browser_open_url(OSBBrowser* self, const gchar* url)
{
  GtkKHTML *active = osb_browser_get_active_engine(self);
  gchar* realUrl = normalize_filename(url);
  gtk_khtml_stop_load(active);
  gtk_khtml_load_url(active, realUrl);
  g_free(realUrl);
}


/*private*/
const gchar* 
osb_browser_config_get_string(OSBBrowser* self, ConfigItem item)
{
  self = self;
  switch(item) {
  case PATH_DEFAULT_LOCATION_ICON:
    return DATADIR "default-location-icon.png";
    break;
  case PATH_UI_XML_DESCRIPTION:
    return DATADIR "osb-browser.glade";
  case STRING_BROWSER_NAME:
    return "Flower";
  case URL_HOME:
    return "http://www.nokia.fi/";

  default:
    g_warning("config_get_string: couldn't find item: %d",item);
    return 0;
  }
}


int
osb_browser_config_get_int(OSBBrowser* self, ConfigItem item)
{
  self = self;
  switch(item) {
    
  case INT_TAB_TITLE_LEN:
    return 50;
  case INT_INITIAL_WIDTH:
    return 800;
  case INT_INITIAL_HEIGHT:
    return 600;

  default:
    return -1;
  }
}


/*private*/
void
osb_browser_set_location_icon(OSBBrowser* self, const gchar* path)
{
  GtkWidget *browser_location_icon =
    glade_xml_get_widget(self->_priv->xml, "browser_location_icon");
    
  GError *gerr = NULL;
 
  GdkPixbuf* default_location_pixbuf =
    gdk_pixbuf_new_from_file(path,
			     &gerr);
    
	
  if (default_location_pixbuf) {
    GtkRequisition req;
    GdkPixbuf *scaled_buf;
    gtk_widget_size_request (browser_location_icon, &req);	
    scaled_buf = gdk_pixbuf_scale_simple (default_location_pixbuf,
				      req.width,
				      req.height,
				      GDK_INTERP_BILINEAR);
    g_object_unref (default_location_pixbuf);
	
    gtk_image_set_from_pixbuf (GTK_IMAGE(browser_location_icon),
			       scaled_buf);
    g_object_unref (scaled_buf);
  } else
    g_warning("couldn't load default location icon from path %s", path);
    
}
/*private*/
GtkKHTML*
osb_browser_get_active_engine (OSBBrowser* self)
{
  int curpage = gtk_notebook_get_current_page (self->_priv->tab);
  GtkKHTML *engine = GTK_KHTML (gtk_notebook_get_nth_page (self->_priv->tab, curpage));

  return engine;
}

/*private*/
void
osb_browser_set_location (OSBBrowser* self, const gchar* location)
{
  GtkEntry *entry;
  if (!location) location = "";
  entry = GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "location"));
  gtk_entry_set_text (entry, location);
}

/*private*/
void
osb_browser_set_title (OSBBrowser* self, const gchar* title)
{
  gchar *clean_title;
  gchar *win_title;
  int len;
  gchar* lf;
  
  if (!title)
    title = "";

  clean_title = g_strdup(title);
  len = strlen(clean_title);
  lf = NULL;

  while ( (lf = g_strstr_len (clean_title, len, "\n"))) {
    *lf = ' ';
  }
  
  clean_title = g_strstrip(clean_title);
  
  win_title = g_strconcat (clean_title,
			   " - ",
			   osb_browser_config_get_string (self, STRING_BROWSER_NAME),
			   NULL);
  g_free (clean_title);
  
  gtk_window_set_title (GTK_WINDOW (self), win_title);
  g_free (win_title); 
}

/*private*/
void
osb_browser_update_ui(OSBBrowser* self, GtkKHTML* engine)
{
  const gchar *title = gtk_khtml_get_title (engine);
  const gchar *loc = gtk_khtml_get_location (engine);
  
  osb_browser_set_location (self, loc);
  if (title)
    osb_browser_set_title (self, title);
  else
    osb_browser_set_title (self, loc);
  
  gtk_widget_set_sensitive (glade_xml_get_widget (self->_priv->xml, "back"),
			    gtk_khtml_can_go_back (engine));
  
  gtk_widget_set_sensitive (glade_xml_get_widget (self->_priv->xml, "go_back"),
			    gtk_khtml_can_go_back (engine));

  
  gtk_widget_set_sensitive (glade_xml_get_widget (self->_priv->xml, "forward"),
			    gtk_khtml_can_go_forward (engine));
  
  gtk_widget_set_sensitive (glade_xml_get_widget (self->_priv->xml, "go_forward"),
			    gtk_khtml_can_go_forward (engine));
    
}

/* private */
void
osb_browser_update_ui_local(OSBBrowser* self, GtkKHTML *engine)
{
  const gchar *title = gtk_khtml_get_title (engine);
  gchar* lf;  
  int max;
  int len;
  gchar *realtitle;
  GtkLabel* label;
  
  if (!title)
    title = gtk_khtml_get_location (engine);

  if (!title)
    title = _("Untitled");
  
  max =  osb_browser_config_get_int (self, INT_TAB_TITLE_LEN);
  realtitle = g_strndup (title, max);
  realtitle[max] = '\0';
  g_strstrip(realtitle);
  
  len = strlen(realtitle);
   lf = NULL;
  while ( (lf = g_strstr_len (realtitle, len, "\n"))) {
    *lf = ' ';
  }
  
  label = GTK_LABEL (gtk_notebook_get_tab_label (self->_priv->tab, GTK_WIDGET (engine)));
  gtk_label_set_text (label, realtitle);
  g_free (realtitle);
}

static void
on_engine_location(GtkKHTML* engine, gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *active = osb_browser_get_active_engine(self);
  if (engine == active) {
    osb_browser_set_location (self, gtk_khtml_get_location (engine));
  }

  osb_browser_update_ui_local(self, engine);  
}

static void
on_engine_title(GtkKHTML* engine, gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *active = osb_browser_get_active_engine(self);
  if (engine == active) {
    osb_browser_set_title (self, gtk_khtml_get_title (engine));
  }

  osb_browser_update_ui_local(self, engine);    
}

static void
on_engine_status(GtkKHTML* engine, const GtkKHTMLLoadStatus* status, gpointer user_data)
{  
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *active = osb_browser_get_active_engine(self);
  if (active == engine) {
    GtkStatusbar *sbar = GTK_STATUSBAR (glade_xml_get_widget (self->_priv->xml, "statusbar"));
    GtkProgressBar *pbar = GTK_PROGRESS_BAR (glade_xml_get_widget (self->_priv->xml, "progressbar"));
    if (sbar){
      const gchar * msg;
      guint cid;
      OSBBrowserGlobal* g = self->_priv->global;
      gchar _msg[400];      
      g_snprintf(_msg, 400, "Received %d of %d bytes, %d/%d files", status->received, status->size, status->ready, status->files);
      osb_browserglobal_set_status(g, _msg);
      msg = osb_browserglobal_get_status(g);
      cid = gtk_statusbar_get_context_id(sbar, "status-text");      
      gtk_statusbar_pop(sbar, cid);
      gtk_statusbar_push(sbar, cid, msg);
    }
    if (pbar && status->size > 0 && status->files > 0) {
      gdouble fraction = 0.0;
      if ( status->filesWithSize < status->files ) {
        gdouble ratio = (gdouble) status->filesWithSize / (gdouble) status->files;      
        fraction += status->received >= status->size ? ratio : ratio * (gdouble) status->received / (gdouble) status->size;
        fraction += status->ready >= status->files ? (1.0 - ratio) : (1.0 - ratio) * (gdouble) (status->ready - status->filesWithSize) / (gdouble) (status->files  - status->filesWithSize);
      } else {
        fraction = (gdouble) status->received / (gdouble) status->size;
      }
      if ( fraction > 1.0 ) {
        fraction = 1.0;
      }
      gtk_progress_bar_set_fraction (pbar, fraction);
    }
  }
  
}

static void
on_engine_mouse_over(GtkKHTML* engine, const gchar* link_title, const gchar* link_label, const gchar* link_url, const gchar* link_target, gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *active = osb_browser_get_active_engine(self);
  
  self = self;
  if (active == engine) {
    GtkStatusbar *sbar = GTK_STATUSBAR (glade_xml_get_widget (self->_priv->xml, "statusbar"));
    if (sbar){
      const gchar* msg;
      guint cid;
      cid = gtk_statusbar_get_context_id(sbar, "link-message");      
      gtk_statusbar_pop(sbar, cid);
      msg = link_url;

      if (msg && *msg != '\0')
	gtk_statusbar_push(sbar, cid, msg);
    }
    

  }
}

static void
on_engine_load_start(GtkKHTML* engine, gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *active = osb_browser_get_active_engine(self);
  GtkVBox *pbarBox = GTK_VBOX (glade_xml_get_widget (self->_priv->xml, "progressbar_box"));
  GtkProgressBar *pbar = GTK_PROGRESS_BAR (glade_xml_get_widget (self->_priv->xml, "progressbar"));
  gtk_progress_bar_set_fraction (pbar, 0.0);
  gtk_widget_show(GTK_WIDGET(pbarBox));
  
  self = self;
  if (active == engine) {
    /* start animating the icon */
  }
  g_warning("load started");
}

static void
on_engine_load_stop(GtkKHTML* engine, gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *active = osb_browser_get_active_engine(self);
  GtkVBox *pbar =  GTK_VBOX (glade_xml_get_widget (self->_priv->xml, "progressbar_box"));
  
  gtk_widget_hide(GTK_WIDGET(pbar));
  
  self = self;
  if (active == engine) {    
    GtkStatusbar *sbar = GTK_STATUSBAR (glade_xml_get_widget (self->_priv->xml, "statusbar"));

    osb_browser_update_ui (self, engine);

    if (sbar) {
      const gchar * msg;
      guint cid;
      OSBBrowserGlobal* g = self->_priv->global;
      osb_browserglobal_set_status(self->_priv->global, "");

      msg = osb_browserglobal_get_status(g);
      cid = gtk_statusbar_get_context_id(sbar, "status-text");      
      gtk_statusbar_pop(sbar, cid);
      gtk_statusbar_push(sbar, cid, msg);
    }    
  }

  osb_browser_update_ui_local (self,engine);
  if (self->_priv->global->renderOnly) {
    osb_browser_print_render_tree(self);
    /* close the ui. */
    gtk_widget_destroy (GTK_WIDGET (self));
  }

  
  
  g_warning("load stopped");  
}

static GtkKHTML*
on_engine_req_new_window(GtkKHTML* engine, const gchar* url, gpointer user_data)
{
  OSBBrowser* new_browser;
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* new_engine;
  const gchar* group;
  
  new_browser = OSB_BROWSER (osb_browser_new (self->_priv->global));

  new_engine = osb_browser_get_active_engine (new_browser);
  group = gtk_khtml_get_group(engine);
  gtk_khtml_set_group(GTK_KHTML(new_engine), group);
  
  gtk_khtml_load_url(new_engine, url);
  
  gtk_widget_show (GTK_WIDGET (new_browser)); /* this should actually be off */
  
  
  return new_engine;
}

static void
on_engine_req_js_prompt(GtkKHTML* engine, GtkKHTMLPromptArgs* args, gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkDialog* dialog;
  gchar* title;
  gint response;
  
  if (args->type == GTK_KHTML_ALERT){
    dialog = GTK_DIALOG(gtk_message_dialog_new (GTK_WINDOW (self),
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					        GTK_MESSAGE_WARNING,
					        GTK_BUTTONS_CLOSE,
					        args->msg));
    
    gtk_dialog_set_has_separator(dialog, FALSE);

    title = g_strconcat(_("JavaScript "), _("Alert"), NULL);
    gtk_window_set_title(GTK_WINDOW(dialog), title);    
    g_free(title);
    gtk_dialog_set_default_response (dialog, GTK_RESPONSE_CLOSE);
    response = gtk_dialog_run (dialog);
    gtk_widget_destroy (GTK_WIDGET (dialog));
    /** setup return parameters */
    if (response == GTK_RESPONSE_YES)
      args->out_ok_pressed = TRUE;
    else
      args->out_ok_pressed = FALSE;
  } else if (args->type == GTK_KHTML_CONFIRM) {
    dialog = GTK_DIALOG(gtk_message_dialog_new (GTK_WINDOW (self),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_WARNING,
						GTK_BUTTONS_YES_NO,
						args->msg));
    
    gtk_dialog_set_has_separator(dialog, FALSE);
    
    title = g_strconcat(_("JavaScript "), _("Confirm"), NULL);
    gtk_window_set_title(GTK_WINDOW(dialog), title);    
    g_free(title);
    
    gtk_dialog_set_default_response (dialog, GTK_RESPONSE_NO);
    response = gtk_dialog_run (dialog);
    gtk_widget_destroy (GTK_WIDGET (dialog));
    /** setup return parameters */
    if (response == GTK_RESPONSE_YES)
      args->out_ok_pressed = TRUE;
    else
      args->out_ok_pressed = FALSE;

  } else if (args->type == GTK_KHTML_INPUT) {
    /* no input supported atm,  */   
    dialog = GTK_DIALOG(gtk_message_dialog_new (GTK_WINDOW (self),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_WARNING,
						GTK_BUTTONS_YES_NO,
						args->msg));
    
    gtk_dialog_set_has_separator(dialog, FALSE);
    
    title = g_strconcat(_("JavaScript "), _("Input"), NULL);
    gtk_window_set_title(GTK_WINDOW(dialog), title);    
    g_free(title);
    
    gtk_dialog_set_default_response (dialog, GTK_RESPONSE_NO);
    response = gtk_dialog_run (dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
    /** setup return parameters */
    args->out_input = g_strdup("not implemented");
    if (response == GTK_RESPONSE_YES)
      args->out_ok_pressed = TRUE;
    else
      args->out_ok_pressed = FALSE;
    
  } else {
    args->out_ok_pressed = FALSE;
  }

}

static void
on_engine_req_auth_prompt(GtkKHTML* engine, GtkKHTMLAuthArgs* args, gpointer user_data)
{
  OSBBrowser *self = OSB_BROWSER (user_data);
  GtkDialog *dialog;
  GtkEntry *entry;
  gchar *realmText = NULL;
  gint response;

  dialog = GTK_DIALOG (glade_xml_get_widget (self->_priv->xml, "authentication"));

  realmText = g_strdup_printf ("Authentication required for %s", args->realm);
  gtk_label_set_text (GTK_LABEL (glade_xml_get_widget (self->_priv->xml, "auth_label")), realmText);
  gtk_entry_set_text (GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "username")), "");
  gtk_entry_set_text (GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "password")), "");
    
  response = gtk_dialog_run (dialog);

  gtk_widget_hide (GTK_WIDGET (dialog));
  
  entry = GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "username"));
  args->out_username = g_strdup (gtk_entry_get_text (entry));
  entry = GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "password"));
  args->out_password = g_strdup (gtk_entry_get_text (entry));
  
  switch (response) { 
  case GTK_RESPONSE_OK:
    args->out_ok_pressed = TRUE;
    break;
  case GTK_RESPONSE_CANCEL:
    /* fallthrough */
  case GTK_RESPONSE_NONE:
    /* fallthrough */
  default:
    args->out_ok_pressed = FALSE;
    break;
  }    
}


/*private*/
void
osb_browser_connect_engine_signals(OSBBrowser* self, GtkKHTML *engine)
{
#define CONNECT_WITH_DATA(__o, __s, __h, __d)\
  g_signal_connect (__o, __s, G_CALLBACK( __h), __d);

  
  CONNECT_WITH_DATA (engine, "location", on_engine_location, self);
  CONNECT_WITH_DATA (engine, "title", on_engine_title, self);
  CONNECT_WITH_DATA (engine, "status", on_engine_status, self);
  CONNECT_WITH_DATA (engine, "req-js-prompt", on_engine_req_js_prompt, self);
  CONNECT_WITH_DATA (engine, "req-auth-prompt", on_engine_req_auth_prompt, self);
  CONNECT_WITH_DATA (engine, "req-new-window", on_engine_req_new_window, self);
  CONNECT_WITH_DATA (engine, "load-start", on_engine_load_start, self);
  CONNECT_WITH_DATA (engine, "load-stop", on_engine_load_stop, self);
  CONNECT_WITH_DATA (engine, "mouse-over", on_engine_mouse_over, self);
		    
#undef CONNECT_WITH_DATA  
}

void
osb_browser_print_render_tree(OSBBrowser* self) {
  GtkKHTML *engine = osb_browser_get_active_engine(self);
  const gchar* rt = gtk_khtml_render_tree (engine);
  g_printf("%s\n", rt);
}

gchar *
normalize_filename(const gchar* file)
{
  if (file) {
    const int len = strlen(file);       
    if (len > 0 && file[0] == '/') {
      /* pad url with necessary "file://" */
      gchar * fullName = g_strconcat ("file://", file, NULL);        
      return fullName;
    }
    if (((len > 1 && file[1] == '/') || len == 1) &&
        (file[0] == '.' || file[0] == '~')) {
      switch (file[0]) {
      case '.': /* case current directory */
      {
        gchar * curdir = g_get_current_dir ();       
        gchar * fullName = g_strconcat ("file://", curdir, file+1, NULL);
        g_free (curdir);
        return fullName;
      }
      case '~': /* case home directory */
      {
        gchar* fullName = g_strconcat ("file://", g_get_home_dir(), file+1, NULL);
        return fullName;
      }
      }
    }
  }

  /* I guess it is a valid URL then. */
  return g_strdup(file);
}
