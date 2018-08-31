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

#ifndef QCOLOR_H_
#define QCOLOR_H_

#include <gdk/gdk.h>

#include "KWQString.h"


typedef unsigned int QRgb;			// RGBA quadruplet

QRgb qRgb(int r, int g, int b);
QRgb qRgba(int r, int g, int b, int a);
int qAlpha(QRgb rgba);

class QColor {
public:
    QColor() : color(0), valid(false) { }
    QColor(QRgb col) : color(col), valid(true) { }
    // FIXME: GdkColor -range is from 0 to 65536 instead of 0 -> 256
    QColor(GdkColor * c) : color(qRgb(c->red/256, c->green/256, c->blue/256)) {}
    QColor(int r, int g, int b) : color(qRgb(r, g, b)), valid(true) { }
    explicit QColor(const char *);
    
    QString name() const;
    void setNamedColor(const QString&);

    bool isValid() const { return valid; }

    int red() const { return (color >> 16) & 0xFF; }
    int green() const { return (color >> 8) & 0xFF; }
    int blue() const { return color & 0xFF; }
    QRgb rgb() const { return color & 0xFFFFFFFF; } // Preserve the alpha.
    void setRgb(int r, int g, int b) { color = qRgb(r, g, b); valid = true; }
    void setRgb(int rgb) { color = rgb; valid = true; /* Alpha may be set. Preserve it. */ }

    void hsv(int *, int *, int *) const;
    void setHsv(int h, int s, int v);

    QColor light(int f = 150) const;
    QColor dark(int f = 200) const;

    friend bool operator==(const QColor &a, const QColor &b);
    friend bool operator!=(const QColor &a, const QColor &b);

    GdkColor* getAsGdkColor() const;

    /** Sets the color parameter.
     * @param color  color to set [out]
     */
    void getGdkColor(GdkColor * color) const;
private:
    QRgb color;
    bool valid : 1;
};

inline bool operator==(const QColor &a, const QColor &b)
{
    return a.color == b.color && a.valid == b.valid;
}

inline bool operator!=(const QColor &a, const QColor &b)
{
    return a.color != b.color || a.valid != b.valid;
}

#endif
