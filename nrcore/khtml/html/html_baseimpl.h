/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2003 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef HTML_BASEIMPL_H
#define HTML_BASEIMPL_H

#include "html/dtd.h"
#include "html/html_elementimpl.h"
#include "misc/khtmllayout.h"

#include <qscrollview.h>

class KHTMLView;

namespace khtml {
    class RenderFrameSet;
    class RenderFrame;
    class RenderPartObject;
}

namespace DOM {

class DOMString;
class CSSStyleSheetImpl;
class HTMLFrameElement;

// -------------------------------------------------------------------------

class HTMLBodyElementImpl : public HTMLElementImpl
{
public:
    HTMLBodyElementImpl(DocumentPtr *doc);
    ~HTMLBodyElementImpl();

    virtual Id id() const;

    virtual bool mapToEntry(NodeImpl::Id attr, MappedAttributeEntry& result) const;
    virtual void parseHTMLAttribute(HTMLAttributeImpl *);

    virtual void insertedIntoDocument();

    void createLinkDecl();
    
    virtual bool isURLAttribute(AttributeImpl *attr) const;

protected:
    CSSStyleDeclarationImpl* m_linkDecl;
};

// -------------------------------------------------------------------------

class HTMLFrameElementImpl : public HTMLElementImpl
{
    friend class khtml::RenderFrame;
    friend class khtml::RenderPartObject;

public:
    HTMLFrameElementImpl(DocumentPtr *doc);

    ~HTMLFrameElementImpl();

    virtual Id id() const;

    virtual void parseHTMLAttribute(HTMLAttributeImpl *);
    virtual void attach();
    virtual void detach();
    virtual bool rendererIsNeeded(khtml::RenderStyle *);
    virtual khtml::RenderObject *createRenderer(RenderArena *, khtml::RenderStyle *);

    bool noResize() { return noresize; }
    void setLocation( const DOMString& str );

    virtual bool isFocusable() const;
    virtual void setFocus(bool);

    DocumentImpl* contentDocument() const;
    
    virtual bool isURLAttribute(AttributeImpl *attr) const;

#if APPLE_CHANGES
    QScrollView::ScrollBarMode scrollingMode() const { return scrolling; }
    int getMarginWidth() const { return marginWidth; }
    int getMarginHeight() const { return marginHeight; }
#endif

protected:
    bool isURLAllowed(const AtomicString &) const;
    virtual void openURL();

    AtomicString url;
    AtomicString name;

    int marginWidth;
    int marginHeight;
    QScrollView::ScrollBarMode scrolling;

    bool frameBorder : 1;
    bool frameBorderSet : 1;
    bool noresize : 1;

private:
    void updateForNewURL();
};

// -------------------------------------------------------------------------

class HTMLFrameSetElementImpl : public HTMLElementImpl
{
    friend class khtml::RenderFrameSet;
public:
    HTMLFrameSetElementImpl(DocumentPtr *doc);

    ~HTMLFrameSetElementImpl();

    virtual Id id() const;

    virtual void parseHTMLAttribute(HTMLAttributeImpl *);
    virtual void attach();
    virtual bool rendererIsNeeded(khtml::RenderStyle *);
    virtual khtml::RenderObject *createRenderer(RenderArena *, khtml::RenderStyle *);

    virtual void defaultEventHandler(EventImpl *evt);

    bool frameBorder() { return frameborder; }
    bool noResize() { return noresize; }

    int totalRows() const { return m_totalRows; }
    int totalCols() const { return m_totalCols; }
    int border() const { return m_border; }
    virtual void detach();

    virtual void recalcStyle( StyleChange ch );
    
protected:
    khtml::Length* m_rows;
    khtml::Length* m_cols;

    int m_totalRows;
    int m_totalCols;
    int m_border;

    bool frameborder : 1;
    bool frameBorderSet : 1;
    bool noresize : 1;
    bool m_resizing : 1;  // is the user resizing currently
};

// -------------------------------------------------------------------------

class HTMLHeadElementImpl : public HTMLElementImpl
{
public:
    HTMLHeadElementImpl(DocumentPtr *doc);

    ~HTMLHeadElementImpl();

    virtual Id id() const;
};

// -------------------------------------------------------------------------

class HTMLHtmlElementImpl : public HTMLElementImpl
{
public:
    HTMLHtmlElementImpl(DocumentPtr *doc);
    ~HTMLHtmlElementImpl();

    virtual Id id() const;
};


// -------------------------------------------------------------------------

class HTMLIFrameElementImpl : public HTMLFrameElementImpl
{
public:
    HTMLIFrameElementImpl(DocumentPtr *doc);

    ~HTMLIFrameElementImpl();

    virtual Id id() const;

    virtual bool mapToEntry(NodeImpl::Id attr, MappedAttributeEntry& result) const;
    virtual void parseHTMLAttribute(HTMLAttributeImpl *attr);

    virtual void attach();
    virtual bool rendererIsNeeded(khtml::RenderStyle *);
    virtual khtml::RenderObject *createRenderer(RenderArena *, khtml::RenderStyle *);
    virtual void recalcStyle( StyleChange ch );
    
    virtual bool isURLAttribute(AttributeImpl *attr) const;

protected:
    virtual void openURL();

    bool needWidgetUpdate;
};


} //namespace

#endif
