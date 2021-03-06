/*
 * Copyright (C) 2003 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef QSCROLLVIEW_H_
#define QSCROLLVIEW_H_

#ifdef signals
#undef signals
#endif
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#define signals protected
#include "KWQFrame.h"


class QScrollViewPrivate;

class QScrollView : public QFrame {
public:
    enum ScrollBarMode { Auto, AlwaysOff, AlwaysOn };
    
    QScrollView(QWidget *parent = 0, const char *name = 0, int flags = 0);
    virtual ~QScrollView();    
    virtual void resize(int,int);
    void setGtkWidget(GtkWidget* widget);

    QWidget *viewport() const;
    int visibleWidth() const;
    int visibleHeight() const;
    int contentsWidth() const;
    int contentsHeight() const;
    int contentsX() const;
    int contentsY() const;
    void scrollBy(int dx, int dy);

    void setContentsPos(int x, int y);
    void setContentsPosRecursive(int,int);

    virtual void setVScrollBarMode(ScrollBarMode vMode);
    virtual void setHScrollBarMode(ScrollBarMode hMode);

    // The following method is not defined in Qt, but we provide it so that we can
    // set the mode for both scrollbars at once.
    virtual void setScrollBarsMode(ScrollBarMode mode);

    // This method is also not defined in Qt.  It gives us a means of blocking painting on our
    // scrollbars until the first layout has occurred.
    void suppressScrollBars(bool suppressed, bool repaintOnUnsuppress=false);
    
    ScrollBarMode vScrollBarMode() const;
    ScrollBarMode hScrollBarMode() const;

    bool hasVerticalScrollBar() const;
    bool hasHorizontalScrollBar() const;
    
    void addChild(QWidget *child, int x = 0, int y = 0);
    void removeChild(QWidget *child);
    int childX(QWidget *child);
    int childY(QWidget *child);

    virtual void resizeContents(int w, int h);
    void updateContents(int x, int y, int w, int h, bool now=false);
    void updateContents(const QRect &r, bool now=false);
    void repaintContents(int x, int y, int w, int h, bool erase = true);
    QPoint contentsToViewport(const QPoint &);
    void contentsToViewport(int x, int y, int& vx, int& vy);
    void viewportToContents(int vx, int vy, int& x, int& y);
    QWidget *clipper() /*const*/ { return this; } /** KWIQ:FIXME: const removed*/
    void enableClipper(bool) { }

    void setStaticBackground(bool);
    bool hasStaticBackground() { return _staticBackground; }

    virtual void resizeEvent(QResizeEvent *);

    void ensureVisible(int,int);
    void ensureVisible(int,int,int,int);

    void ensureRectVisibleCentered(const QRect &r);
    
    GtkWidget* getDocumentView() const;
 public:
    // these methods are protected in QT
    virtual void viewportMousePressEvent( QMouseEvent * ) {};
    virtual void focusInEvent( QFocusEvent * ) {};
    virtual void focusOutEvent( QFocusEvent * ) {};
    virtual void viewportMouseDoubleClickEvent( QMouseEvent * ) {};
    virtual void viewportMouseMoveEvent(QMouseEvent *) {};
    virtual void viewportMouseReleaseEvent(QMouseEvent *) {};
    virtual void keyPressEvent( QKeyEvent *_ke ) {};
    virtual void keyReleaseEvent ( QKeyEvent *_ke ) {};
    virtual bool focusNextPrevChild( bool next ) {return false;};
private:
    GtkAdjustment *_ha, *_va;
    bool _staticBackground;
    QScrollViewPrivate* d;
    
    void removeInnerGtkWidget();
    
    friend class QScrollViewPrivate;
    
};

#endif
