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

#ifndef QWIDGET_H_
#define QWIDGET_H_
#include <gdk/gdk.h>

#include "KWQObject.h"
#include "KWQPaintDevice.h"
#include "KWQPainter.h"
#include "KWQPointArray.h"
#include "KWQSize.h"
#include "KWQPalette.h"
#include "KWQFont.h"
#include "KWQCursor.h"
#include "KWQEvent.h"
#include "KWQStyle.h"


typedef struct _GtkWidget GtkWidget;
typedef struct _PangoFontDescription PangoFontDescription;

PangoFontDescription* createPangoFontDescription(const QFont* font) ;
    
class QWidgetPrivate;

class QWidget : public QObject, public QPaintDevice {
public:
  
    enum WidgetFlags {
	WResizeNoErase = 1,
	WRepaintNoErase = 1 << 1,
	WPaintUnclipped = 1 << 2,
    };
    
    enum FocusPolicy {
        NoFocus = 0,
        TabFocus = 0x1,
        ClickFocus = 0x2,
        StrongFocus = 0x3,
        WheelFocus = 0x7
    };


    QWidget(QWidget * parent = 0, const char * name = 0, int f = 0);
    QWidget(GtkWidget*);

    virtual ~QWidget();

    virtual QSize sizeHint() const;
    
    virtual void setEnabled(bool);
    virtual bool isEnabled() const;
    
    void setActiveWindow();

    void setAutoMask(bool) { }
    void setMouseTracking(bool) { }

    long winId() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
    QSize size() const;
    virtual void resize(int,int);
    void resize(const QSize &);
    QPoint pos() const;
    void move(int, int);
    void move(const QPoint &);

    virtual void paint(QPainter *, const QRect &);
    
    virtual QRect frameGeometry() const;
    virtual void setFrameGeometry(const QRect &);

    virtual int baselinePosition(int height) const; // relative to the top of the widget

    virtual QPoint mapFromGlobal(const QPoint &) const;

    virtual bool hasFocus() const;
    virtual void setFocus();
    
    void clearFocus();
    virtual bool checksDescendantsForFocus() const;
    
    virtual FocusPolicy focusPolicy() const;
    void setFocusPolicy(FocusPolicy) {};
    
    virtual void setFocusProxy(QWidget *) {};

    const QPalette& palette() const;
    virtual void setPalette(const QPalette &);
    void unsetPalette();
    
    QStyle &style() const;
    void setStyle(QStyle *);
    
    QFont font() const;
    virtual void setFont(const QFont &);
    
    void constPolish() const;
    bool isVisible() const;
    void setCursor(const QCursor &);
    QCursor cursor();
    void unsetCursor();
    virtual bool focusNextPrevChild(bool);
    bool hasMouseTracking() const;

    void show();
    void hide();

    void showEvent(QShowEvent *) { }
    void hideEvent(QHideEvent *) { }
    void wheelEvent(QWheelEvent *) { }
    void keyPressEvent(QKeyEvent *) { }
    void keyReleaseEvent(QKeyEvent *) { }
    void focusInEvent(QFocusEvent *) { }
    void focusOutEvent(QFocusEvent *) { }

    enum BackgroundMode { NoBackground };    
    void setBackgroundMode(BackgroundMode) { }

    void setAcceptDrops(bool) { }

    void setIsSelected(bool) { }
    
    void setGtkWidget(GtkWidget* widget);
    GtkWidget* getGtkWidget() const { return _widget;}
    
    void displayRect(int x, int y, int w, int h);
    void lockDrawingFocus();
    void unlockDrawingFocus();
    void enableFlushDrawing();
    void disableFlushDrawing();
    void setDrawingAlpha(float alpha);

    void sendConsumedMouseUp();
    
    virtual void setWritingDirection(QPainter::TextDirection direction);
    
private:
    QWidgetPrivate *data;
    GtkWidget*  _widget;

    
};

#endif
