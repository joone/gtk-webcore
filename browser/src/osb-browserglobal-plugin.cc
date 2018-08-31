
extern "C" {
#include "osb-browserglobal.h"
#include "osb-browserglobal-private.h"
}

#include <glib.h>
#include <gtk/gtk.h>


#if 0 // plugins disabled
#include <gtk-nswidget.h>
#include <gtk-nswidget-registry.h>

#include <osb/osb.h>

class NSPlugin : public OSB::Plugin, public OSB::DataConsumer {
    GtkNSWidget* plugin;
public:
    NSPlugin(GtkNSWidget* aplugin);
    ~NSPlugin();

    /** OSB::DataConsumer*/
    void start();

    void end();

    void cancel();

    void metaData(const gchar* key, const gchar* value);

    void data(const gchar* data, guint len);


    /* OSB::Plugin*/

    virtual OSB::DataConsumer* getConsumer();
    virtual GtkWidget* getGtkWidget();
};


class NSPluginFactory : public OSB::PluginFactory
{
    GtkNSWidgetRegistry* registry;
    GSList* mimetypes;

public:
    NSPluginFactory();
    ~NSPluginFactory();

    void addSharedLibrary(const gchar* path);

    /** OSB::Plugin interface methods */
    bool provides(const gchar* mimeType);
    
    OSB::Plugin* create(const gchar* mimetype);
    void release(OSB::Plugin* plugin);

};


NSPluginFactory::NSPluginFactory()
    :mimetypes(0)
{
    registry = gtk_nswidget_registry_create();        
}

extern "C" {
static void
_mimetypes_delete_cb(gpointer data, gpointer user_data)
{
    /** empty for now */
}
}

NSPluginFactory::~NSPluginFactory()
{
    g_slist_foreach (mimetypes, _mimetypes_delete_cb, this);
    g_slist_free (mimetypes);
    gtk_nswidget_registry_destroy (registry);
}

void NSPluginFactory::addSharedLibrary(const gchar* path)
{
    GtkNSWidgetLibrary* lib;
    int i;
    lib = gtk_nswidget_registry_load_library(registry, path);

    if (!lib) 
	return;

    for (i=0;lib->mimetypev[i];i++)
	g_slist_append (mimetypes, lib);
}

bool NSPluginFactory::provides(const gchar* mimeType)
{
    GSList* l = mimetypes;
    GtkNSWidgetLibrary* lib;
    int i;

    while (l) {
	lib = (GtkNSWidgetLibrary*) l->data;
	for (i=0;lib->mimetypev[i];i++) {
	    if (g_ascii_strcasecmp(mimeType, lib->mimetypev[i]) == 0)
		return true;
	}
	l = l->next;
    }
    return false;    
}

OSB::Plugin* NSPluginFactory::create(const gchar* mimetype)
{
    GtkWidget *widget = gtk_nswidget_new(registry, mimetype);

    if (!widget)
	return 0;

    NSPlugin *plugin = new NSPlugin(GTK_NSWIDGET (widget));
    return plugin;
}

void NSPluginFactory::release(OSB::Plugin* plugin)
{
    g_return_if_fail (plugin);

    delete plugin;
}

static
NSPluginFactory& NSPluginFactoryS()
{
    static NSPluginFactory singleton;
    static int added = 0;
    if (!added) {
	OSB::EnvironmentF().addPluginFactory(&singleton);
    }
    return singleton;
}


/**
 * NSPlugin
 */

NSPlugin::NSPlugin(GtkNSWidget* aplugin)
    :plugin(aplugin)
{
    g_object_ref(plugin);
}

NSPlugin::~NSPlugin()
{
    g_object_unref(plugin);
}

OSB::DataConsumer* NSPlugin::getConsumer()
{
    return this;
}

GtkWidget* NSPlugin::getGtkWidget()
{
    return GTK_WIDGET (plugin);
}

void NSPlugin::start()
{
}

void NSPlugin::end()
{
}

void NSPlugin::cancel()
{
}

void NSPlugin::metaData(const gchar* key, const gchar* value)
{
}

void NSPlugin::data(const gchar* data, guint len)
{
};

#endif

extern "C" {
gint
osb_browserglobal_register_plugin (OSBBrowserGlobal* self, const gchar* path)
{
    GtkWidget *plugin;

    g_return_val_if_fail (self, 0);
    g_return_val_if_fail (path, 0);
#if 0
    NSPluginFactoryS().addSharedLibrary( path);
#endif

    return 1;
}

void
osb_browserglobal_unregister_plugins (OSBBrowserGlobal* self)
{
    
}

}
