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
#include <glib.h>
#include <glib-object.h>
#ifndef __GTK_KHTML_H__
#define __GTK_KHTML_H__

/**
 *  Gtk API for KHTML library.
 * 
 * Straight C API to use KHTML library.
 * work in progress, suggestions and patches welcome.
 *
 * currently coding standards follow gtk coding standards
 * except for indenting (gnu style)
 *
 * current documentation written in doxygen format, but
 * may be chaged to glib/gtk format.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#include <gtk/gtk.h>


/*
 * Type checking and casting macros
 */
#define GTK_TYPE_KHTML	(gtk_khtml_get_type())
#define GTK_KHTML(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), gtk_khtml_get_type(), GtkKHTML)
#define GTK_KHTML_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), gtk_khtml_get_type(), GtkKHTML const)
#define GTK_KHTML_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), gtk_khtml_get_type(), GtkKHTMLClass)
#define GTK_IS_KHTML(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), gtk_khtml_get_type ())

#define GTK_KHTML_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), gtk_khtml_get_type(), GtkKHTMLClass)

/* Private structure type */
typedef struct _GtkKHTMLPrivate GtkKHTMLPrivate;

/*
 * Main object structure
 */
typedef struct _GtkKHTML GtkKHTML;

struct _GtkKHTML {
  GtkBin __parent__;
  /*< private >*/
  GtkKHTMLPrivate *_priv;
};

/** Rendering engine settings structure
 *  Rendering engine settings structure. 
 */

typedef struct _GtkKHTMLSettings GtkKHTMLSettings;
struct _GtkKHTMLSettings {
  gboolean javascript_enabled;
  gboolean java_enabled;
  gboolean plugins_enabled;
  gboolean autoload_images;
  gfloat minimum_font_size;
  gfloat default_font_size;
  gfloat default_fixed_font_size;
  const gchar* default_text_encoding; 
  const gchar* serif_font_family; 
  const gchar* sans_serif_font_family; 
  const gchar* fixed_font_family; 
  const gchar* standard_font_family; 
  const gchar* cursive_font_family; 
  const gchar* fantasy_font_family;

  /** Full url (including port) of the proxy to use. If NULL, empty (""),
   */
  const gchar* http_proxy; 
};

typedef struct _GtkKHTMLLoadStatus GtkKHTMLLoadStatus;

/** Type of the status message. */
typedef enum
{
  /** Report start loading a resource */
  GTK_KHTML_LOADING_START,
  /** Report progress in loading a resource */
  GTK_KHTML_LOADING,
  /** Report a successful resource load */
  GTK_KHTML_LOADING_COMPLETE,
  /** Report an error in resource loading. */
  GTK_KHTML_LOADING_ERROR  
} GtkKHTMLStatus;

/** Status message. */
struct _GtkKHTMLLoadStatus {
  /** Status of the loading process. */
  GtkKHTMLStatus status;

  /** Number of files to download */
  guint files;

  /** Number of files received with Content-Length set*/
  guint filesWithSize;

  /** Number of files received */
  guint ready;

  /** Bytes available for resource. */
  guint size;

  /** Bytes received from resource. */
  guint received;

  /** Total size of the resources including those that didn't have
      Content-Length set. */
  guint totalSize;

  /** Bytes received total. */
  guint totalReceived;

  /** Error code. */
  gint statusCode;
};


  
typedef struct _GtkKHTMLPromptArgs GtkKHTMLPromptArgs;


/** Type of message prompt if message prompt was an alert (ok button), confirm (yes/no) or input (textfield, ok, cancel).
 */
typedef enum
{
  /** alert (ok button)*/
  GTK_KHTML_ALERT,
  /**confirm (yes/no)*/
  GTK_KHTML_CONFIRM,
  /**or input (textfield, ok, cancel) */
  GTK_KHTML_INPUT
} GtkKHTMLPromptType;

/** Structure used to pass arguments of javascript prompt messages
 */
struct _GtkKHTMLPromptArgs
{
  /*<public>*/
  
  /** Flag specifying if message prompt was an alert (ok button), confirm (yes/no) or
   *  input (textfield, ok, cancel). */
  GtkKHTMLPromptType type;

  /** The prompt message*/
  const gchar* msg;
  
  /**  default input text.
   *  if type == GTK_KHTML_INPUT contains default input text to be shown in input field */
  const gchar* default_input; 

  /**  text entered in the prompt textfield
  *   should be set if type == GTK_KHTML_INPUT
   *  string will be deallocated by signaller with g_free().
   *  ie. there's an ownership change. */
  gchar* out_input;
  
  /** flag specifying if the button pressed was ok or cancel, yes or no
   * \TRUE means ok or yes
   * \FALSE means cancel or no
   */
  gboolean out_ok_pressed;    
};

typedef struct _GtkKHTMLAuthArgs GtkKHTMLAuthArgs;

/** Structure used to pass arguments of javascript prompt messages
 */
struct _GtkKHTMLAuthArgs
{
  /*< public >*/
  /** realm of authentication */
  const gchar* realm;

  /** Out parameter containing username from user.
   *  string will be deallocated by caller with g_free().
   *  ie. there's an ownership change.*/
  gchar* out_username;

  /** Out parameter containing password from user.
   *  string will be deallocated by caller with g_free()
   *  ie. there's an ownership change. */
  
  gchar* out_password;
  
  /** flag specifying if the button pressed was ok or cancel
   * \TRUE means ok
   * \FALSE otherwise  */
  gboolean out_ok_pressed;    
};

  
/*
 * Class definition
 */
typedef struct _GtkKHTMLClass GtkKHTMLClass;
struct _GtkKHTMLClass {
  GtkBinClass __parent__;
  
  /*signals*/
  
 
  /** Location change indication signal.
   * use \gtk_khtml_get_location() to get the url string
   * @emited when current page changes.
   * @param self the engine which sent the signal   
   */
  void (* location) (GtkKHTML * self);
  
  /** Title change indication signal.
   * use \gtk_khtml_get_title() to get the title string
   * @emited when title of current page changes.
   * @param self the engine which sent the signal   
   */
  void (* title) (GtkKHTML * self);

  /** Progress change indication signal.
   * FIXME: NOT EMITED AT THE MOMENT.
   * might change in the future
   *
   * @emited when there has been progres in the loading of current page.
   * @param self the engine which sent the signal
   * @param curprogress the amount of already loaded data
   * @param maxprogress the amount of data the page contains
   */
  void (* progress) (GtkKHTML * self, gint curprogress, gint maxprogress);
  
  /** Load start indication signal.
   * @emited when page loading is started
   * @param self the engine which sent the signal
   */
  void (* load_start) (GtkKHTML * self);
  
  /** Load start indication signal.
   * @emited when page loading is stopped.
   * @emited on error   
   * @param self the engine which sent the signal
   */
  void (* load_stop) (GtkKHTML * self);

  void (* load_item_start) (GtkKHTML* self, int item);
  void (* load_item_progress) (GtkKHTML* self, int item);  
  void (* load_item_stop) (GtkKHTML* self, int item, int status);
  
  /** javascript status change indication signal
   * use \gtk_khtml_get_status_text() to get the status string
   *
   * @emited when javascript status changes.
   * @param self the engine which sent the signal
   */
  void (* status_text) (GtkKHTML * self);

  /** Page load status change indication signal
   *
   * @emited when page load status changes.
   * @param self the engine which sent the signal
   * @param status status of the engine.
   */
  void (* status) (GtkKHTML * self, GtkKHTMLLoadStatus * status);

  /** javascript prompt and alert request signal
   * must be synchronous
   * @emited when javascript generates an alert 
   * @emited when javascript generates an prompt
   * 
   * @param self the engine which sent the signal
   * @param args used to pass information between browser and engine (in and out)
   *        see #GtkKHTMLPromptAgs
   */  
  void (* req_js_prompt) (GtkKHTML *self, GtkKHTMLPromptArgs* args);

    /** HTTP Auth
   * must be synchronous
   * @emited when javascript generates an alert 
   * @emited when javascript generates an prompt
   * 
   * @param self the engine which sent the signal
   * @param args used to pass information between browser and engine (in and out)
   *        see #GtkKHTMLPromptAgs
   */  
  void (* req_auth_prompt) (GtkKHTML *self, GtkKHTMLAuthArgs* args);

  /** javascript/html new window request signal
   * must be synchronous
   *
   * @emited when javascript requests to create a new window
   * @emited when html link contains target="_blank" -attribute   *
   * @param self the engine which sent the signal
   * @param newengine The new engine of the new window
   *        or \NULL if new window creation is not allowed
   * @return \FALSE if don't allow window to be opened
   *         \TRUE if window was opened
   */    
  gboolean (* req_new_window) (GtkKHTML *self, GtkKHTML* newengine);
  
  void (* mouse_over) (GtkKHTML *self, const gchar* link_title, const gchar* link_label, const gchar* link_url, const gchar* link_target);

  /** selection change signal*/
  void (* selection) (GtkKHTML *self);
#if 0
  /* not implemented at the moment */
  void (* req_file_attach) (GtkKHTML *self);  
  void (* file_upload_finished) (GtkKHTML *self);
  void (* file_upload_finished) (GtkKHTML *self);
#endif  

};


/*
 * Public methods
 */


/** Returns GObject type for KHTML html renderer class
 * Returns GObject type for KHTML html renderer class.
 *
 * @return type for KHTML html renderer class 
 */
GType gtk_khtml_get_type ();


/** Creates new html rendering engine
 * Creates new html rendering engine widget. To be able to view web pages,
 * add returned widget to a container and show the container.
 * 
 * @return html rendering engine widget 
 */
GtkWidget * gtk_khtml_new ();

/** Starts loading of a new url
 * Starts loading of a new url. Loading is asynchronous.
 *
 * @emit "load-start" signal on start
 * @emit "load-stop"  signal when loading stops, ie. succesfully loaded page, or error
 * @param self the engine to use
 * @param url the url to load
 */
void gtk_khtml_load_url (GtkKHTML * self, const gchar * url);

/** Reloads current url
 * reloads current url
 */
void gtk_khtml_refresh (GtkKHTML * self);

/** Cancels the load currently in progress, if any
 * Cancels the load currently in progress, if any.
 * @emit "load-stop"
 *
 * @param self the engine to use
 */
void gtk_khtml_stop_load (GtkKHTML * self);

/** Checks the browsing history position relative to the beginning of the history
 * Checks if the engine is at the beginning of browsing history.
 * @return \TRUE if browsing history has previous elements
 *         \FALSE otherwise
 */
gboolean gtk_khtml_can_go_back (GtkKHTML * self);


/** Checks the browsing history position relative to the end of the history
 * Checks if the engine is at the end of browsing history.
 * @return \TRUE if browsing history has successive elements
 *         \FALSE otherwise
 */
gboolean gtk_khtml_can_go_forward (GtkKHTML * self);

/** Directs browser engine to the previous url in the browsing history
 * Directs browser engine to the previous url in the browsing history.
 * @emit "load-start" see \gtk_khtml_load_url
 * @emit "load-stop" see \gtk_khtml_load_url
 */
void gtk_khtml_go_back (GtkKHTML * self);

/** Directs browser engine to the next url in the browsing history
 * Directs browser engine to the next url in the browsing history.
 *
 * @emit "load-start" see \gtk_khtml_load_url
 * @emit "load-stop" see \gtk_khtml_load_url
 */
void gtk_khtml_go_forward (GtkKHTML * self);

/** Returns the url of the currently loaded page
 * Returns the url of the currently loaded page.
 * The string contains the full url used, including
 * the protocol. It can be absolute or relative.
 * The string must not be freed (const).
 *
 * @return  the url string of the currently loaded page.
 */
const gchar* gtk_khtml_get_location (GtkKHTML * self);

/** Returns the title of the currently loaded page
 * Returns the title of the currently loaded page.
 * If the page contains a frameset, title is the title of the
 * frameset.
 *
 * @return the title of the currently loaded page.
 */
const gchar* gtk_khtml_get_title (GtkKHTML * self);

/** Returns a status string set by javascript
 * Returns a string that javascript has set to be used
 * usually in the statusbar of the browser.
 *
 * @return javascript status string
 */
const gchar* gtk_khtml_get_status_text (GtkKHTML * self);

/** Returns internal representation of the engine
 * Returns internal representation of the engine.
 * Can be used in C++ code. Exposes the public NRCore api
 * to the user. This may be needed if certain functionality
 * is not implemented in C interface.
 * usage not recommended
 * Used with:
 * OSBB::Bridge* engine = 0;
 * GtkKHTML* khtml_engine = gtk_khtml_new();
 * engine = static_cast<OSB::Bridge*>(gtk_khtml_get_internal (engine));
 *
 */
void* gtk_khtml_get_internal (GtkKHTML * self);
  
/** Sets the settings of the rendering engine
 * Sets the settings of the rendering engine.
 * 
 */
void gtk_khtml_set_settings (GtkKHTML * self, const GtkKHTMLSettings* settings);

/** Returns the current settings of the rendering engine
 * Returns the current settings of the rendering engine. Fields
 * are read only and must not b
 */ 
const GtkKHTMLSettings* gtk_khtml_get_settings (GtkKHTML * self);

/**  Prints the internal render tree of the engine.
 * 
 * Can be used in C++ debug code. This 
 * usage not recommended
 * Used with:
 * GtkKHTML* khtml_engine = gtk_khtml_new();
 * const gchar * url = "http://www.mysite.com/";
 * const ghar* presentation = gtk_khtml_render_tree (engine, url));
 *
 */
const gchar* gtk_khtml_render_tree (GtkKHTML * self);

/** Toggles the emission of internal constructed load status messages
 * Sets the emission of internal constructed load status messages
 * Signalö emitted is status-text
 *
 * @param flag \TRUE if you want to receive internal status messages
                     for loading
	       \FALSE if not
 */
void gtk_khtml_set_emit_internal_status (GtkKHTML * self, gboolean flag);


void gtk_khtml_set_group (GtkKHTML* self, const gchar* group);

const gchar* gtk_khtml_get_group (GtkKHTML* self);

void gtk_khtml_set_text_multiplier (GtkKHTML* self, gfloat multiplier);
gfloat gtk_khtml_get_text_multiplier (GtkKHTML* self);

gboolean gtk_khtml_find (GtkKHTML* self, const gchar* text, gboolean case_sensitive, gboolean dir_down);

const gchar* gtk_khtml_get_current_selection_as_text(GtkKHTML* self);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
