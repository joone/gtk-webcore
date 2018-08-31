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
#include <string.h> /* memset() */
#include <assert.h>

#include "gtk-khtml.h"

#include "gtk-khtml-private.h"
#include "gtk-khtml-marshal.h"

#include "osb.h"

static void gtk_khtml_class_init (GtkKHTMLClass * c) G_GNUC_UNUSED;
static void gtk_khtml_init (GtkKHTML * self) G_GNUC_UNUSED;
static void _gtk_khtml_finalize (GObject * self) G_GNUC_UNUSED;
static void _gtk_khtml_realize (GtkWidget * widget) G_GNUC_UNUSED;
static void _gtk_khtml_map (GtkWidget * widget) G_GNUC_UNUSED;
static gboolean _gtk_khtml_expose_event (GtkWidget * widget, GdkEventExpose * event) G_GNUC_UNUSED;
static void _gtk_khtml_size_allocate (GtkWidget * widget, GtkAllocation * allocation) G_GNUC_UNUSED;


enum {
    STATUS_TEXT_SIGNAL,
    STATUS_SIGNAL,
    LOCATION_SIGNAL,
    TITLE_SIGNAL,
    PROGRESS_SIGNAL,
    REQ_NEW_WINDOW,
    LOAD_START,
    LOAD_STOP,
    LOAD_ITEM_START,
    LOAD_ITEM_PROGRESS,
    LOAD_ITEM_STOP,
    REQ_AUTH_PROMPT,
    MOUSE_OVER,
    SELECTION,
    LAST_SIGNAL
};

static guint object_signals[LAST_SIGNAL] = {0};

/* pointer to the class of our parent */
static GtkBinClass *parent_class = NULL;

GType
gtk_khtml_get_type (void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
	static const GTypeInfo info = {
	    sizeof (GtkKHTMLClass),
	    (GBaseInitFunc) NULL,
	    (GBaseFinalizeFunc) NULL,
	    (GClassInitFunc) gtk_khtml_class_init,
	    (GClassFinalizeFunc) NULL,
	    NULL /* class_data */,
	    sizeof (GtkKHTML),
	    0 /* n_preallocs */,
	    (GInstanceInitFunc) gtk_khtml_init,
	};

	type = g_type_register_static (GTK_TYPE_BIN, "GtkKHTML", &info, (GTypeFlags)0);
    }

    return type;
}

#undef PARENT_HANDLER
static void 
gtk_khtml_class_init (GtkKHTMLClass * c)
{
    GObjectClass *g_object_class = (GObjectClass*) c;
    GtkWidgetClass *gtk_widget_class = (GtkWidgetClass *)c;

    parent_class = (GtkBinClass *)g_type_class_ref (GTK_TYPE_BIN);

    object_signals[STATUS_TEXT_SIGNAL] =
	g_signal_new ("status_text",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, status_text),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__VOID,
		      G_TYPE_NONE, 0);
    object_signals[STATUS_SIGNAL] =
	g_signal_new ("status",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, status),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__POINTER,
		      G_TYPE_NONE, 1,
		      G_TYPE_POINTER);
    object_signals[LOCATION_SIGNAL] =
	g_signal_new ("location",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, location),
		      NULL, NULL,
		      g_cclosure_marshal_VOID__VOID,
		      G_TYPE_NONE, 0);
    object_signals[TITLE_SIGNAL] =
	g_signal_new ("title",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, title),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__VOID,
		      G_TYPE_NONE, 0);
    object_signals[PROGRESS_SIGNAL] =
	g_signal_new ("progress",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__INT_INT,
		      G_TYPE_NONE, 2,
		      G_TYPE_INT,
		      G_TYPE_INT);
    object_signals[REQ_NEW_WINDOW] =
	g_signal_new ("req-new-window",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_POINTER__POINTER,
		      G_TYPE_POINTER, 1, G_TYPE_POINTER);

    object_signals[LOAD_START] =
	g_signal_new ("load-start",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__VOID,
		      G_TYPE_NONE, 0);

    object_signals[LOAD_STOP] =
	g_signal_new ("load-stop",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__VOID,
		      G_TYPE_NONE, 0);

    object_signals[LOAD_ITEM_START] =
	g_signal_new ("load-item-start",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__INT,
		      G_TYPE_NONE, 0);

    object_signals[LOAD_ITEM_PROGRESS] =
	g_signal_new ("load-item-progress",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__INT_INT,
		      G_TYPE_NONE, 0);

    object_signals[LOAD_ITEM_STOP] =
	g_signal_new ("load-item-stop",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__INT,
		      G_TYPE_NONE, 0);

    object_signals[LOAD_STOP] =
	g_signal_new ("req-js-prompt",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__POINTER,
		      G_TYPE_NONE, 1,
		      G_TYPE_POINTER);
    object_signals[REQ_AUTH_PROMPT] =
	g_signal_new ("req-auth-prompt",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, progress),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__POINTER,
		      G_TYPE_NONE, 1,
		      G_TYPE_POINTER);
    object_signals[MOUSE_OVER] =
	g_signal_new ("mouse-over",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, mouse_over),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__POINTER_POINTER_POINTER_POINTER,
		      G_TYPE_NONE, 4,
		      G_TYPE_POINTER,G_TYPE_POINTER,G_TYPE_POINTER,G_TYPE_POINTER );
    object_signals[SELECTION] =
	g_signal_new ("signal",
		      G_TYPE_FROM_CLASS (g_object_class),
		      (GSignalFlags)(G_SIGNAL_RUN_LAST),
		      G_STRUCT_OFFSET (GtkKHTMLClass, selection),
		      NULL, NULL,
		      gtk_khtml_marshal_VOID__VOID,
		      G_TYPE_NONE, 0);


    c->status_text = NULL;
    c->location = NULL;
    c->title = NULL;
    c->req_new_window = NULL;
    c->load_start = NULL;
    c->load_stop = NULL;
    c->req_js_prompt = NULL;

    g_object_class->finalize = _gtk_khtml_finalize;
    gtk_widget_class->realize = _gtk_khtml_realize;
    gtk_widget_class->map = _gtk_khtml_map;
    gtk_widget_class->expose_event = _gtk_khtml_expose_event;
    gtk_widget_class->size_allocate = _gtk_khtml_size_allocate;
    return;
}

static void
_gtk_khtml_finalize(GObject *objself)
{
    GtkKHTML *self = GTK_KHTML (objself);
    gpointer priv = self->_priv;

    if (self->_priv->location) g_free (self->_priv->location); 
    if (self->_priv->title) g_free (self->_priv->title);
    if (self->_priv->js_input_text) g_free (self->_priv->js_input_text);
    if (self->_priv->auth_username) g_free (self->_priv->auth_username);
    if (self->_priv->auth_password) g_free (self->_priv->auth_password);

    g_free (priv);

#define PARENT_HANDLER(___self) \
	{ if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(___self); }

    PARENT_HANDLER(G_OBJECT (self));
    return;
#undef PARENT_HANDLER
}

static void
_on_gtk_khtml_destroy(GtkObject *object, gpointer user_data)

{
    GtkKHTML* self = GTK_KHTML (object);
    /* GtkBin has removed child from widget structure and decreased its reference count */
    /* destruct structures, which leads to destruction of child and all inner objects */    

    if (self->_priv->browser) delete self->_priv->browser;
    self->_priv->browser = 0;

}

static void 
gtk_khtml_init (GtkKHTML * self)
{
    self->_priv = g_new0 (GtkKHTMLPrivate, 1);

    return;
}


/**
 * gtk_khtml_new:
 *
 * Makes a new #GtkKHTML widget
 *
 * Returns: a new widget
 **/
GtkWidget * 
gtk_khtml_new()
{
    OSB::Preferences *settings;
    GtkKHTML* self = GTK_KHTML (g_object_new (gtk_khtml_get_type(),  NULL));
    MyRoot* browser = new MyRoot(self);
    self->_priv->browser = browser;
    self->_priv->child = self->_priv->browser->mainFrame()->widget();
    settings = self->_priv->browser->preferences();    

    settings->setWillLoadImagesAutomatically(true);
    settings->setJavaScriptEnabled(true);
    settings->setJavaEnabled(false);
    settings->setPluginsEnabled(false);
    settings->setMinimumFontSize(6);
    settings->setDefaultFontSize(14);
    settings->setDefaultFixedFontSize(10);
    settings->setDefaultTextEncoding("ISO-8859-1");
    settings->setSerifFontFamily("serif");
    settings->setSansSerifFontFamily("sans");
    settings->setFixedFontFamily("fixed");
    settings->setStandardFontFamily("times");
    settings->setCursiveFontFamily("times");
    settings->setFantasyFontFamily("lucidabright");

    gtk_container_add (GTK_CONTAINER (self), self->_priv->child);

    /* to destruct structures (which destructs self->_priv->child)*/
    g_signal_connect (G_OBJECT (self),
		      "destroy",
		      G_CALLBACK (_on_gtk_khtml_destroy),
		      self);

    return (GtkWidget*)self;
}

void 
gtk_khtml_load_url (GtkKHTML * self, const gchar * url)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));
	
    self->_priv->browser->mainFrame()->startLoad(url);	
}

void 
gtk_khtml_refresh (GtkKHTML * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));
	
    self->_priv->browser->mainFrame()->refresh();	
}

void 
gtk_khtml_stop_load (GtkKHTML * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));
	
    self->_priv->browser->mainFrame()->stopLoad();
}

gboolean 
gtk_khtml_can_go_back (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, (gboolean )0);
    g_return_val_if_fail (GTK_IS_KHTML (self), (gboolean )0);

    return self->_priv->browser->canGoBack();
}

gboolean 
gtk_khtml_can_go_forward (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, (gboolean )0);
    g_return_val_if_fail (GTK_IS_KHTML (self), (gboolean )0);

    return self->_priv->browser->canGoForward();
}

void 
gtk_khtml_go_back (GtkKHTML * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));

    if ( self->_priv->browser->canGoBack())  
	self->_priv->browser->goBack();

}

void 
gtk_khtml_go_forward (GtkKHTML * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));
	
    if ( self->_priv->browser->canGoForward())  
	self->_priv->browser->goForward();
}

const gchar * 
gtk_khtml_get_location (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, (const gchar * )0);
    g_return_val_if_fail (GTK_IS_KHTML (self), (const gchar * )0);

    return self->_priv->browser->myLocation();
}

const gchar * 
gtk_khtml_get_title (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, (const gchar * )0);
    g_return_val_if_fail (GTK_IS_KHTML (self), (const gchar * )0);
    

    return self->_priv->browser->myTitle();
}


const GtkKHTMLLoadStatus * 
gtk_khtml_get_status (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, (const GtkKHTMLLoadStatus * )0);
    g_return_val_if_fail (GTK_IS_KHTML (self), (const GtkKHTMLLoadStatus * )0);

    return self->_priv->browser->myStatus();
}

void*
gtk_khtml_get_internal (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (GTK_IS_KHTML (self), NULL);
    
    
    return self->_priv->browser;
}


void gtk_khtml_set_settings (GtkKHTML * self, const GtkKHTMLSettings * settings)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));
    OSB::Preferences *s = self->_priv->browser->preferences();

    s->setWillLoadImagesAutomatically (settings->autoload_images);
    s->setJavaScriptEnabled (settings->javascript_enabled);
    s->setJavaEnabled (settings->java_enabled);
    s->setPluginsEnabled (settings->plugins_enabled);
    s->setMinimumFontSize (settings->minimum_font_size);
    s->setDefaultFontSize (settings->default_font_size);
    s->setDefaultFixedFontSize (settings->default_fixed_font_size);

#define CALL_IF_SET(inst, method, var) if ((var)) (inst)->method (var);
    
    CALL_IF_SET(s, setDefaultTextEncoding, settings->default_text_encoding);
    CALL_IF_SET(s, setSerifFontFamily, settings->serif_font_family);
    CALL_IF_SET(s, setSansSerifFontFamily, settings->sans_serif_font_family);
    CALL_IF_SET(s, setFixedFontFamily, settings->fixed_font_family);
    CALL_IF_SET(s, setStandardFontFamily, settings->standard_font_family);
    CALL_IF_SET(s, setCursiveFontFamily, settings->cursive_font_family);
    CALL_IF_SET(s, setFantasyFontFamily, settings->fantasy_font_family);
#undef CALL_IF_SET    

    OSB::ProtocolDelegate* httpprov = self->_priv->browser->protocolDelegateForURL("http://");        
    if (httpprov) {
	httpprov->setProxy(settings->http_proxy);
    }
}

void
gtk_khtml_set_emit_internal_status (GtkKHTML* self, gboolean flag)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));

    self->_priv->browser->setEmitInternal(flag == TRUE);
}

void
gtk_khtml_set_group (GtkKHTML * self, const gchar* group)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));

    OSB::Root* browser = self->_priv->browser;
    browser->setGroup(group);   
}

const gchar * 
gtk_khtml_get_group (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (GTK_IS_KHTML (self), NULL);

    OSB::Root* browser = self->_priv->browser;
    return browser->group();
}

void 
gtk_khtml_set_text_multiplier (GtkKHTML* self, gfloat multiplier)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_KHTML (self));

    OSB::Root* browser = self->_priv->browser;
    browser->setTextSizeMultiplier(multiplier);
}

gfloat
gtk_khtml_get_text_multiplier (GtkKHTML* self)
{
    g_return_val_if_fail (self != NULL, -1.0);
    g_return_val_if_fail (GTK_IS_KHTML (self), -1.0);

    OSB::Root* browser = self->_priv->browser;
    return browser->textSizeMultiplier();
}

gboolean 
gtk_khtml_find (GtkKHTML* self, const gchar* text, gboolean case_sensitive, gboolean dir_down)
{
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KHTML (self), FALSE);

    return self->_priv->browser->findString(text, case_sensitive, dir_down);
}

const gchar* 
gtk_khtml_get_current_selection_as_text(GtkKHTML* self)
{
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (GTK_IS_KHTML (self), NULL);

    return "";
}

static void 
_gtk_khtml_realize (GtkWidget * widget)
#define PARENT_HANDLER(___widget) \
	{ if(GTK_WIDGET_CLASS(parent_class)->realize) \
		(* GTK_WIDGET_CLASS(parent_class)->realize)(___widget); }
{
	
    
    GdkWindowAttr   attributes;
    gint            attributes_mask;

    g_return_if_fail(widget != NULL);
    
    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);
    
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
    
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    
    widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				     &attributes, attributes_mask);
    gdk_window_set_user_data (widget->window, widget);
    
    widget->style = gtk_style_attach(widget->style, widget->window);
    
}
#undef PARENT_HANDLER

static void 
_gtk_khtml_map (GtkWidget * widget)
#define PARENT_HANDLER(___widget) \
	{ if(GTK_WIDGET_CLASS(parent_class)->map) \
		(* GTK_WIDGET_CLASS(parent_class)->map)(___widget); }
{
    
    GtkWidget* child;
    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_KHTML (widget));
    
    child = GTK_BIN(widget)->child;
    
    GTK_WIDGET_SET_FLAGS(widget, GTK_MAPPED);
    
    //NEEDED? 
    if (GTK_WIDGET_VISIBLE(child))
    {
	if (!GTK_WIDGET_MAPPED (child)){
	    gtk_widget_map(child);
	}
    }
    gdk_window_show(widget->window);
}
#undef PARENT_HANDLER

static gboolean 
_gtk_khtml_expose_event (GtkWidget * widget, GdkEventExpose * event)
#define PARENT_HANDLER(___widget,___event) \
	((GTK_WIDGET_CLASS(parent_class)->expose_event)? \
		(* GTK_WIDGET_CLASS(parent_class)->expose_event)(___widget,___event): \
		((gboolean )0))
{
	
    return PARENT_HANDLER(widget,event);   
}

#undef PARENT_HANDLER

static void 
_gtk_khtml_size_allocate (GtkWidget * widget, GtkAllocation * allocation)
#define PARENT_HANDLER(___widget,___allocation) \
	{ if(GTK_WIDGET_CLASS(parent_class)->size_allocate) \
		(* GTK_WIDGET_CLASS(parent_class)->size_allocate)(___widget,___allocation); }
{
	
    GtkWidget* child;
    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_KHTML (widget));
    
    widget->allocation = *allocation;
    
    // NEEDED? Not in Mozilla
    child = GTK_BIN(widget)->child;
    if (child){
	GtkAllocation a;
	GtkRequisition r;
	gtk_widget_size_request(child, &r);
	
	a.x = 0;
	a.y = 0;
	/* a.height = r.height; */
	/* 	a.width = r.width; */
	
	a.width = allocation->width;
	a.height = allocation->height;
	
	gtk_widget_size_allocate(child, &a);
    }
    
    if (GTK_WIDGET_REALIZED(widget))
    {
	gdk_window_move_resize(widget->window,
			       allocation->x, allocation->y,
			       allocation->width, allocation->height);
    }  
}
#undef PARENT_HANDLER

MyRoot::MyRoot(GtkKHTML* agtkkhtml)  
    :listener(this)
    ,gtkkhtml(agtkkhtml)
    ,internal(false)    
{
    setUIDelegate(&listener);
    setFrameLoadDelegate(&listener);
    setResourceLoadDelegate(&listener);
    status = g_new0(GtkKHTMLLoadStatus, 1);
}

MyRoot::~MyRoot()
{
    g_free(status);
}

const GtkKHTMLLoadStatus* MyRoot::myStatus()
{
    return status;
}


const gchar* MyRoot::myLocation()
{
    return mainFrame()->location();
}

const gchar* MyRoot::myTitle()
{
    const gchar* l = listener.title();
    return l;
}

gboolean MyRoot::findString(const gchar* text, gboolean case_sensitive, gboolean dir_down)
{
    return searchFor(text, dir_down, case_sensitive) ? TRUE:FALSE;
}

void MyRoot::internalStatusStart()
{
    if (!internal) return;
    status->status = GTK_KHTML_LOADING_START;
    status->size = 0;
    status->received = 0;
    status->files = 0;
    status->filesWithSize = 0;
    status->ready = 0;
   
    g_signal_emit_by_name(gtkkhtml, "status", status);
}

void MyRoot::internalStatusStop(int statusCode)
{
    if (!internal) return;
    status->status = GTK_KHTML_LOADING_COMPLETE;
    status->statusCode = statusCode;

//    assert(status->ready == status->files);
}

void MyRoot::internalStatusStartItem(const OSB::ResourceStatus * _status)
{
    if (!internal) return;
    status->files++;
    g_signal_emit_by_name(gtkkhtml, "status", status);
}

void MyRoot::internalStatusHeadersItem(const OSB::ResourceStatus * _status)
{
    if (!internal) return;
    if (_status->size()) {
        status->size += _status->size();
        status->totalSize += _status->size(); 
        status->filesWithSize++;
    }
    g_signal_emit_by_name(gtkkhtml, "status", status);
}

void MyRoot::internalStatusProgressItem(const OSB::ResourceStatus * _status)
{
    if (!internal) return;
    status->totalReceived += _status->lastReceived();
    // in case Content-Length -header is not present
    if (!_status->size()) {
        status->totalSize += _status->lastReceived();
    } else {
        status->received += _status->lastReceived();
    }
    g_signal_emit_by_name(gtkkhtml, "status", status);
}

void MyRoot::internalStatusStopItem(const OSB::ResourceStatus * _status)
{
    if (!internal) return;
    
    assert(_status->size() == 0 ||
           _status->error() ||
           _status->size() == _status->received());
    
    status->ready++;
    if (_status->error()) {
        // FIXME: handle errors as well.u
    }

    g_signal_emit_by_name(gtkkhtml, "status", status);
}


MyEventListener::MyEventListener(MyRoot* aroot)
    :root(aroot)
{  
    strSz = 400;
    status = g_new0(gchar, strSz);
    loc = g_new0(gchar, strSz);
    pageTitle = g_new0(gchar, strSz);    
    clearState();
}

MyEventListener::~MyEventListener()
{
    if (status)
	g_free(status);
    if (loc) 
	g_free(loc);
    if (pageTitle)
	g_free(pageTitle);
}

void MyEventListener::clearState()
{
    *status = *loc = *pageTitle = '\0';
}

static gboolean 
_utf8_copy_if_valid_else_set_empty(gchar* dst, const gchar* src, int dstsz)
{
    if (src &&g_utf8_validate(src, -1, NULL)) {
	glong sz = g_utf8_strlen(src, dstsz);
	g_utf8_strncpy(dst, src, sz+1);
	return TRUE;
    } 
    *dst = '\0';
    return FALSE;
}

void MyEventListener::onMouseOverChanged(OSB::Frame* target, OSB::ElementInfo* node)
{ 
    assert(node);
    g_signal_emit_by_name(root->engine(), "mouse-over", 
			  node->linkTitle(), 
			  node->linkLabel(), 
			  node->linkURL(), 
			  node->linkTarget());
}

void MyEventListener::setStatusText(OSB::Frame* target, const gchar* node) 
{ 
    _utf8_copy_if_valid_else_set_empty(status, node, strSz);
    g_signal_emit_by_name(root->engine(), "status", status);
}

void MyEventListener::onTitleChanged(OSB::Frame* frame, const gchar* title) 
{ 
    if (frame == root->mainFrame()) {
	_utf8_copy_if_valid_else_set_empty(pageTitle, title, strSz);
	g_signal_emit_by_name(root->engine(), "title"); 
    }
}

void MyEventListener::onClientRedirectReceived(OSB::Frame* frame,const gchar* to) 
{
    frame = frame;
    to = to;
    /** not used*/
}

void MyEventListener::onServerRedirected(OSB::Frame* frame, const gchar* to) 
{ 
    if (frame == root->mainFrame()) {
	g_signal_emit_by_name(root->engine(), "location"); 
    }
}

void MyEventListener::onCommitLoad(OSB::Frame* frame) 
{
    if (frame == root->mainFrame()) {
	g_signal_emit_by_name(root->engine(), "location"); 
    }
}

#if 0
gint
_convert_status(int status)
{
    /* map OSB status to GtkKHTML status*/
    return (gint) status;
}
#endif

void MyEventListener::onFrameLoadStarted(OSB::Frame* frame) 
{
    if (frame == root->mainFrame()) {
	clearState();
	root->internalStatusStart();
	g_signal_emit_by_name(root->engine(), "load-start");
    }
}

void MyEventListener::onFrameLoadFinished(OSB::Frame* frame, int status) 
{
    if (frame == root->mainFrame()) {
	root->internalStatusStop(status);
	g_signal_emit_by_name(root->engine(), "load-stop");
    } 
}

void MyEventListener::onResourceLoadStarted(OSB::Frame* frame, const OSB::ResourceStatus * status) 
{
    root->internalStatusStartItem(status);
}

void MyEventListener::onResourceLoadHeaders(OSB::Frame* frame, const OSB::ResourceStatus * status) 
{
    root->internalStatusHeadersItem(status);
}

void MyEventListener::onResourceLoadStatus(OSB::Frame* frame, const OSB::ResourceStatus * status) 
{
    root->internalStatusProgressItem(status);
}

void MyEventListener::onResourceLoadFinished(OSB::Frame* frame, const OSB::ResourceStatus * status) 
{
    root->internalStatusStopItem(status);
}

OSB::Root*  MyEventListener::createNewRoot(OSB::Frame* frame, const gchar* url)
{
    GtkKHTML* new_engine =  NULL;
    g_signal_emit_by_name (root->engine(), "req-new-window", url, &new_engine);

    if (!new_engine) 
	return 0;

    OSB::Root*  browser = new_engine->_priv->browser;
    return browser;
}

void MyEventListener::alertPanel(OSB::Frame* frame, const gchar *message) 
{ 
    GtkKHTMLPromptArgs args = {GTK_KHTML_ALERT,0,};
    args.msg = message;

    g_signal_emit_by_name (root->engine(), "req-js-prompt", &args);
    
    return; 
}

bool MyEventListener::confirmPanel(OSB::Frame* frame, const gchar *message) 
{ 
    GtkKHTMLPromptArgs args = {GTK_KHTML_CONFIRM,0,};
    args.msg = message;

    g_signal_emit_by_name (root->engine(), "req-js-prompt", &args);

    return (args.out_ok_pressed == TRUE); 
}

bool MyEventListener::textInputPanel(OSB::Frame* frame, const gchar *prompt, const gchar *defaultText, const gchar **result)
{ 
    GtkKHTMLPromptArgs args = {GTK_KHTML_INPUT, 0,};
    args.msg = prompt;
    
    args.default_input = defaultText;

    g_signal_emit_by_name (root->engine(), "req-js-prompt", &args);
    
    /* clear possible old values */
    if (root->engine()->_priv->js_input_text) { 
	g_free (root->engine()->_priv->js_input_text);
	root->engine()->_priv->js_input_text = NULL;
    }

    /* copy new results
     * GtkKHTMLInputArgs::out_input must be allocated with g_new, and will be owned and released by 
     * us */
    if (args.out_input) 	
	root->engine()->_priv->js_input_text = args.out_input; /* no g_strdup() by api convention*/

    /* propagate results to caller. results will be valid until next call to this method */	
    if (result)
	*result = root->engine()->_priv->js_input_text;

    return (args.out_ok_pressed == TRUE); 

}

bool MyEventListener::authPanel(OSB::Frame* frame, const gchar *realm, const gchar **username, const gchar **password) 
{ 
    GtkKHTMLAuthArgs args = {0,};
    GtkKHTMLPrivate *priv = root->engine()->_priv;
    args.realm = realm;
    /* fixme: do we need to pass default usernames/passwords to browser */
    args.out_username = 0;
    args.out_password = 0;

    g_signal_emit_by_name (root->engine(), "req-auth-prompt", &args);

    /* clear possible old values */
    if (priv->auth_username) {
	g_free(priv->auth_username);
	priv->auth_username = NULL;
    }
    if (priv->auth_password) {
	g_free(priv->auth_password);
	priv->auth_password = NULL;
    }

    /* copy new results. 
     * GtkKHTMLAuthArgs::out_{username,password} must be allocated with g_new, and will be owned by
     * and released by us.*/
    if (args.out_username) 
	priv->auth_username = args.out_username; /* no g_strdup() by api convention*/
    
    if (args.out_password) 
	priv->auth_password = args.out_password; /* no g_strdup() by api convention*/    

    /* propagate results to caller. results will be valid until next call to this method */	
    if (username)
	*username = priv->auth_username;
	
    if (password)
        *password = priv->auth_password;
    
    return (args.out_ok_pressed == TRUE); 
}


const gchar *
gtk_khtml_render_tree (GtkKHTML * self)
{
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (GTK_IS_KHTML (self), NULL);
    
    //    self->_priv->browser->aux().renderTreeString();
    return "";
}

/*
  Local Variables:
  c-basic-offset: 4
  End:
 */
