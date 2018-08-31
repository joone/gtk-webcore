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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include <assert.h>
#include <glib.h>
/* #include <glib/gi18n.h> */
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <gdk/gdkkeysyms.h> /* for GDK_Escape */
#include <gtk-khtml.h>

#include "callbacks.h"

#include "osb-browser.h"
#include "osb-browser-private.h"
#include "osb-browserglobal-private.h"

#ifndef _
#define _(x) (x)
#endif

void
on_file_new_window_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkWidget* new_window = osb_browser_new (self->_priv->global);
  
  gtk_widget_show (new_window);
}


void
on_file_new_tab_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = GTK_KHTML (gtk_khtml_new ());
  const gchar* title = gtk_khtml_get_title (engine);
  gint page;
  
  osb_browser_set_settings(self, GTK_KHTML (engine));
  
  if (!title) title = _("Untitled");

  gtk_notebook_append_page (self->_priv->tab, GTK_WIDGET (engine), gtk_label_new(title));
  gtk_notebook_set_tab_label_packing (self->_priv->tab, GTK_WIDGET (engine), TRUE, TRUE, GTK_PACK_START);
  gtk_widget_show (GTK_WIDGET (engine));
  
  /* gtk_notebook_append_page returns new page only in 2.4 */
  page = gtk_notebook_page_num (self->_priv->tab, GTK_WIDGET (engine) );
  
  if (page == -1) {
    /* XXX: cant create new tab */
    g_warning ("couldn't create new tab");
    return;
  }
  
  gtk_notebook_set_current_page (self->_priv->tab, page);
  gtk_notebook_set_show_tabs (self->_priv->tab, TRUE);
  osb_browser_connect_engine_signals (self, engine);
  
}


void
on_file_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_file_close_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);

  int i = gtk_notebook_get_n_pages (self->_priv->tab);
  if (i == 1) {
    gtk_widget_destroy (GTK_WIDGET (self));
  } else {    
    gtk_notebook_remove_page (self->_priv->tab,
			      gtk_notebook_get_current_page (self->_priv->tab));
    if (i == 2) 
      gtk_notebook_set_show_tabs(self->_priv->tab, FALSE);
  }
}


void
on_file_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  gtk_widget_destroy (GTK_WIDGET (self));
}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_edit_profiles1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_go1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}



void
on_history1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_bookmarks1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_add1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_manage1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_tools1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_dom_tree1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  return;
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}


void
on_stop_activate                       (GtkButton       *button,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);

}



void
on_exec_activate                       (GtkButton       *button,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_quit2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_cut2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  g_warning ("%s, user_data: %x", __PRETTY_FUNCTION__, (guint) user_data);
}


void
on_back_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = osb_browser_get_active_engine (self);
  
  gtk_khtml_go_back (engine);
  osb_browser_update_ui(self, engine);
}


void
on_forward_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = osb_browser_get_active_engine (self);
  
  gtk_khtml_go_forward (engine);
  osb_browser_update_ui(self, engine);

}


void
on_stop_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);
  gtk_khtml_stop_load (engine);
  osb_browser_update_ui(self, engine);
}


void
on_refresh_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);
  const gchar * cur_url;
  
  gtk_khtml_stop_load (engine);
  cur_url = normalize_filename(gtk_khtml_get_location (engine));
  if (cur_url) {
      gtk_khtml_refresh (engine);
  }
  osb_browser_update_ui (self, engine);
}


void
on_home_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);
  gtk_khtml_load_url (engine, osb_browser_config_get_string (self, URL_HOME));
  osb_browser_update_ui(self, engine);
}

void
on_exec_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
 OSBBrowser* self = OSB_BROWSER (user_data);
 GtkEntry *entry = GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "location"));

 g_signal_emit_by_name (entry, "activate", user_data);
}


void
on_location_activate           (GtkEntry        *entry,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = osb_browser_get_active_engine (self);
  
  gchar* loc = normalize_filename(gtk_entry_get_text (entry));
  if (loc) {
    gtk_khtml_load_url (engine, loc); 
  }
  g_free(loc);
}

gboolean
on_location_key_release_event   (GtkWidget *widget,
				 GdkEventKey *event,
				 gpointer user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = osb_browser_get_active_engine (self);
  if (event->keyval == GDK_Escape)
    osb_browser_set_location (self, gtk_khtml_get_location (engine));
  return FALSE;  
}

void
on_go_back_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = osb_browser_get_active_engine (self);

  gtk_khtml_go_back (engine);
  osb_browser_update_ui(self, engine);
}


void
on_go_forward_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML *engine = osb_browser_get_active_engine (self);

  gtk_khtml_go_forward (engine);
  osb_browser_update_ui(self, engine);
}


void
on_go_home_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);
  gtk_khtml_load_url (engine, osb_browser_config_get_string (self, URL_HOME));
  osb_browser_update_ui(self, engine);
}



gboolean
on_browser_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  self = self;
  return FALSE;
}


void
on_browser_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (object);

  g_object_unref (self->_priv->xml);

  self->_priv->xml = 0;


  
  osb_browserglobal_browser_destroyed (self->_priv->global, self);
}


void
on_engine_container_switch_page        (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  
  GtkKHTML* engine = GTK_KHTML (gtk_notebook_get_nth_page(self->_priv->tab, page_num));
  osb_browser_update_ui(self, engine);
  osb_browser_update_ui_local(self, engine);
  
}

void
on_rendering_tree_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  osb_browser_print_render_tree(self);
}


gboolean
on_location_location_key_release_event (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{

  return FALSE;
}

#define TEXT_MULTIPLIER_DEF 1.0f
#define TEXT_MULTIPLIER_MIN 0.3f
#define TEXT_MULTIPLIER_MAX 3.0f
#define TEXT_MULTIPLIER_STEP 0.2f

void
on_view_zoom_in_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);

  gfloat mult =
    gtk_khtml_get_text_multiplier(engine);
  
  if (mult < TEXT_MULTIPLIER_MAX)    
    gtk_khtml_set_text_multiplier(engine, mult + TEXT_MULTIPLIER_STEP);
  
}


void
on_view_zoom_out_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);
  
  gfloat mult =
    gtk_khtml_get_text_multiplier(engine);
  
  if (mult > TEXT_MULTIPLIER_MIN)    
    gtk_khtml_set_text_multiplier(engine, mult - TEXT_MULTIPLIER_STEP);
}


void
on_view_zoom_default_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);  
  
  gtk_khtml_set_text_multiplier(engine, TEXT_MULTIPLIER_DEF);
}


void
on_find_close_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  
  GtkDialog *find = GTK_DIALOG (glade_xml_get_widget (self->_priv->xml, "find"));
  gtk_widget_hide (GTK_WIDGET (find));
  g_warning("close");
}


void
on_find_find_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  GtkKHTML* engine = osb_browser_get_active_engine(self);
  gboolean found = FALSE;
  GtkEntry *find_entry = GTK_ENTRY (glade_xml_get_widget (self->_priv->xml, "find_entry"));
  GtkToggleButton *find_dir_down = GTK_TOGGLE_BUTTON (glade_xml_get_widget (self->_priv->xml, "find_dir_down"));
  GtkToggleButton *find_wrap = GTK_TOGGLE_BUTTON (glade_xml_get_widget (self->_priv->xml, "find_wrap"));
  GtkToggleButton *find_case = GTK_TOGGLE_BUTTON (glade_xml_get_widget (self->_priv->xml, "find_case"));

  g_warning("find");
  found = gtk_khtml_find (engine,
			  gtk_entry_get_text (find_entry),
			  gtk_toggle_button_get_active(find_case),
			  gtk_toggle_button_get_active(find_dir_down));

  /* if wrap specified, try to find again */
  if (!found && gtk_toggle_button_get_active(find_wrap)) {
      found = gtk_khtml_find (engine,
			      gtk_entry_get_text (find_entry),
			      gtk_toggle_button_get_active(find_case),
			      gtk_toggle_button_get_active(find_dir_down));
  }

  if (!found) {
    GtkDialog *msg = GTK_DIALOG (gtk_message_dialog_new (GTK_WINDOW (self),
							 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,	
							 GTK_MESSAGE_INFO,
							 GTK_BUTTONS_CLOSE,
							 _("Searched text not found")));
    gtk_dialog_set_default_response (msg, GTK_RESPONSE_CLOSE);
    gtk_dialog_run (msg);
    gtk_widget_destroy (GTK_WIDGET (msg));
  }
}


void
on_edit_find_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  
  GtkDialog *find = GTK_DIALOG (glade_xml_get_widget (self->_priv->xml, "find"));
  gtk_widget_show (GTK_WIDGET (find));
}


void
on_edit_find_again_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  OSBBrowser* self = OSB_BROWSER (user_data);
  self = self;

}


gboolean
on_find_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_widget_hide (widget);
  /* stop propagating*/
  return TRUE;
}

