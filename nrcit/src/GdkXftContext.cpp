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
#include <assert.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "GdkXftContext.h"
#include "CGContextProvider.h"

/* 
 * invariants: (xftdraw != 0) == (xftGdkDraw != 0)
 *
 */

static
void 
setXftClip(XftDraw* draw, GdkRegion* r, gint xoff, gint yoff)
{
    assert(draw);
    assert(r);

    GdkRectangle* rects = 0;
    gint n_rects = 0;
    gdk_region_get_rectangles(r, &rects, &n_rects);
    XRectangle* xrects;
    
    if (n_rects) {
	xrects = g_new(XRectangle, n_rects);
	int i;
	for (i=0;i<n_rects;i++) {
	    xrects[i].x = rects[i].x - xoff;
	    xrects[i].y = rects[i].y - yoff;
	    xrects[i].width = rects[i].width;
	    xrects[i].height = rects[i].height;
	}

	XftDrawSetClipRectangles(draw, 0,0, xrects, n_rects);
	g_free(xrects);
    }

    if (rects) 
	g_free(rects);
}


GdkXftContext::GdkXftContext(CGContextProvider* aprovider, GdkDrawable *adrawable)
    :savedClip(0)
    ,xftdraw(0)     
    ,xftGdkDraw(0)
    ,xcmap(0)
    ,xvisual(0)
    ,xftxoff(0)
    ,xftyoff(0)
    ,iswindow(false)
    ,provider(aprovider)
{
    assert(adrawable);
    assert(provider);
    clip = 0;

    drawable = adrawable;
    g_object_ref(drawable);
    gc = gdk_gc_new(drawable); // not reffed, gdk_gc_new does initial reffing

    updateXftDraw();
}

GdkXftContext::~GdkXftContext()
{
    if (clip)
	gdk_region_destroy(clip);
    
    if (savedClip)
	gdk_region_destroy(savedClip);
    
    if (xftdraw) {
	XftDrawDestroy(xftdraw);
	g_object_unref(xftGdkDraw);
    }

    g_object_unref(gc);    
    g_object_unref(drawable);

}

void GdkXftContext::updateXftDraw()
{
    assert(drawable);
    assert(gc);

    GdkDrawable *real_drawable = drawable;

    // if doublebuffering is on, drawable is actually window and internal 
    // paint info contains the correct buffer.
    GdkWindow* window = GDK_IS_WINDOW(drawable) ? GDK_WINDOW(drawable) : 0;
    if (window) {
	gdk_window_get_internal_paint_info(window, &real_drawable,  &xftxoff, &xftyoff);
	iswindow = true;
    } else {
	iswindow = false;
	xftxoff = xftyoff = 0;
    }

    g_object_ref(real_drawable);

    Colormap xcmap_new = GDK_COLORMAP_XCOLORMAP(gdk_drawable_get_colormap(real_drawable));
    Visual *xvisual_new = GDK_VISUAL_XVISUAL(gdk_drawable_get_visual(real_drawable));

    if (xftdraw && xcmap_new == xcmap && xvisual_new == xvisual) {
	XftDrawChange(xftdraw, GDK_DRAWABLE_XID(real_drawable));
	g_object_unref(xftGdkDraw);
    } else { 
	if (xftdraw) {
	    XftDrawDestroy(xftdraw);
	    g_object_unref(xftGdkDraw);
	}

	xcmap = xcmap_new;
	xvisual = xvisual_new;	
	xftdraw = XftDrawCreate(GDK_DRAWABLE_XDISPLAY(real_drawable),
				GDK_DRAWABLE_XID(real_drawable),    
				xvisual,
				xcmap);
	
    }

    xftGdkDraw = real_drawable;
}

void GdkXftContext::xftTranslate(int *x, int* y)
{
    assert(x);
    assert(y);
    if (iswindow) {
	gdk_window_get_internal_paint_info(drawable, NULL,  &xftxoff, &xftyoff);
	*x += xftxoff;
	*y += xftyoff;
    }
}
    
void GdkXftContext::clearClip()
{
    if (clip) {
        gdk_gc_set_clip_region(gc, NULL);
        gdk_region_destroy(clip);
        clip = 0;
    }
}
     
void GdkXftContext::addClip(GdkRectangle* rect)
{
    if (!clip) {
        clip = gdk_region_rectangle(rect);
    } else {
        gdk_region_union_with_rect(clip, rect);
    }

    gdk_gc_set_clip_origin(gc, 0, 0);
    gdk_gc_set_clip_region(gc, clip);

    setXftClip(xftdraw, clip, xftxoff, xftyoff);
}

void GdkXftContext::saveGraphicsState()
{
    assert(!savedClip);
    
    if (clip)
        savedClip = gdk_region_copy(clip);
}


void GdkXftContext::restoreGraphicsState()
{
    if (clip)
        gdk_region_destroy(clip);

    clip = savedClip;
    savedClip = 0;

    gdk_gc_set_clip_region(gc, clip);
}

void GdkXftContext::regionExpiresAt(GTimeVal* moment, GdkRectangle* rect)
{
    provider->regionExpiresAt(moment, rect, this);
}
