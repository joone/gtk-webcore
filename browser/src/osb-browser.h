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
#ifndef __OSB_BROWSER_H__
#define __OSB_BROWSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>


/*
 * class OSBBrowser
 *
 * Class to represent a browser window
 *  - inherits from GtkWindow
 *  - can contain multiple tabs
 *
 * Purpose is to demonstrate usage of GtkKHTML interface,
 * an API to the OSB HTML Rendering Engine.
 *
 * todo:
 *   - refactor to OSBBrowser and OSBBrowserView (view == one tab)?
 *   - implement easier access to distinct engines, ie. provide
 *     list of GtkKHTML*. Now the distinct rendering engines are
 *     accessed through GtkNotebook interface
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <osb-browserglobal.h>


/*
 * Type checking and casting macros
 */
#define OSB_TYPE_BROWSER	(osb_browser_get_type())
#define OSB_BROWSER(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), osb_browser_get_type(), OSBBrowser)
#define OSB_BROWSER_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), osb_browser_get_type(), OSBBrowser const)
#define OSB_BROWSER_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), osb_browser_get_type(), OSBBrowserClass)
#define OSB_IS_BROWSER(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), osb_browser_get_type ())

#define OSB_BROWSER_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), osb_browser_get_type(), OSBBrowserClass)

/* Private structure type */
typedef struct _OSBBrowserPrivate OSBBrowserPrivate;

/*
 * Main object structure
 */
typedef struct _OSBBrowser OSBBrowser;
struct _OSBBrowser {
	GtkWindow __parent__;
	/*< private >*/
	OSBBrowserPrivate *_priv;
};

/*
 * Class definition
 */
typedef struct _OSBBrowserClass OSBBrowserClass;
struct _OSBBrowserClass {
	GtkWindowClass __parent__;
};


/*
 * Public methods
 */
GType osb_browser_get_type (void);
GtkWidget* osb_browser_new (OSBBrowserGlobal * global);
void osb_browser_open_url(OSBBrowser*, const gchar*);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
