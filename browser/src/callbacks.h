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
#include <gtk/gtk.h>


void
on_file_new_window_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_new_tab_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_close_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_find1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_find_again1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_profiles1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_go1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_back_clicked                       (GtkButton    *menuitem,
                                        gpointer         user_data);

void
on_forward_clicked                     (GtkButton       *button,
                                        gpointer         user_data);



void
on_home_clicked                       (GtkButton     *menuitem,
                                        gpointer         user_data);

void
on_history1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_bookmarks1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_add1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_manage1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_tools1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dom_tree1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_rendering_tree_activate            (GtkMenuItem     *menuitem,
                                       gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_stop_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_refresh_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_exec_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_quit2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_new_window_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_new_tab_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_close_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_back_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_file_forward_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_go_back_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_go_forward_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_go_home_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_location_activate                   (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_refresh_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_browser_destroy_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_browser_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_browser_destroy_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_browser_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_browser_destroy                     (GtkObject       *object,
                                        gpointer         user_data);

void
on_engine_container_switch_page        (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data);
gboolean
on_location_key_release_event   (GtkWidget *widget,
				 GdkEventKey *event,
				 gpointer user_data);

void
on_rendering_tree_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_location_location_key_release_event (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_view_zoom_in_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_view_zoom_out_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_view_zoom_default_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_find_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_find_again_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_find_close_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_find_find_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_find_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
