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

#include "osb-browserglobal.h"
#include "osb-browser.h"
#include <assert.h>

/* #include <unistd.h> /\* getopt *\| */
#include <getopt.h> /* getopt_long_only */


/** */
static int gnum_browsers = 0;


static void
on_browser_created (OSBBrowserGlobal* self, gpointer user_data)
{
  assert (gnum_browsers>=0);
  gnum_browsers++;
}

static void
on_browser_destroyed (OSBBrowserGlobal* self, gpointer user_data)
{
  assert (gnum_browsers>0);
  gnum_browsers--;
  if (gnum_browsers == 0){
    gtk_main_quit();
  }    
}

int
main(int argc, char *argv[]) 
{
  OSBBrowserGlobal* browser_global;
  OSBBrowser* browser;
  
  gtk_init (&argc, &argv);  
  
  browser_global = (OSBBrowserGlobal*) osb_browserglobal_new ();
  browser_global->renderOnly = FALSE;
  /* parse options */
  if (argc > 1) {
    int c = 0;
    
    while (1) {
      int option_index = 0;
      static struct option long_options[] = {
        {"render-only", 0, 0, 0},
        {0, 0, 0, 0}
      };
      
      c = getopt_long (argc, argv, "r",
                       long_options, &option_index);
      if (c == -1)
        break;
      
      switch (c) {
      case 0:
      case 'r':
        browser_global->renderOnly = TRUE;
        break;
      }                                                                                                                                 
    }
  }

  g_signal_connect (G_OBJECT (browser_global),
                    "browser-created",
                    G_CALLBACK(on_browser_created),
                    browser_global);
  
  g_signal_connect (G_OBJECT (browser_global),
                    "browser-destroyed",
                    G_CALLBACK(on_browser_destroyed),
                    browser_global);

  
  browser = (OSBBrowser*) osb_browser_new (browser_global);
  
  gtk_widget_show (GTK_WIDGET (browser));
  /*g_object_ref(browser);*/	/* browser object is destroyed when it is closed */
  
  /* parse options */
  if ((browser_global->renderOnly && argc > 2) || argc > 1) {
    osb_browser_open_url (browser, argv[browser_global->renderOnly ? 2 : 1]);    
  }
  gtk_main();
  
  /* start the event loop */
  osb_browserglobal_destroy_all_browsers (browser_global);
  g_object_unref(browser_global); /* destroy the global object */
  return 0;
}
