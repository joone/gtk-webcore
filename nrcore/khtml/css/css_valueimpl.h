/*
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2002 Apple Computer, Inc.
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
 * $Id: css_valueimpl.h,v 1.1.1.1 2004/09/23 08:27:53 kimkinnu Exp $
 */
#ifndef _CSS_css_valueimpl_h_
#define _CSS_css_valueimpl_h_

#include "dom/css_value.h"
#include "dom/dom_string.h"
#include "css/css_base.h"
#include "misc/loader_client.h"
#include "misc/shared.h"

#include <qintdict.h>

namespace khtml {
    class RenderStyle;
    class CachedImage;
    class DocLoader;
}

namespace DOM {

class CSSRuleImpl;
class CSSValueImpl;
class NodeImpl;
class CounterImpl;

class CSSStyleDeclarationImpl : public StyleBaseImpl
{
public:
    CSSStyleDeclarationImpl(CSSRuleImpl *parentRule);
    CSSStyleDeclarationImpl(CSSRuleImpl *parentRule, QPtrList<CSSProperty> *lstValues);
    virtual ~CSSStyleDeclarationImpl();

    CSSStyleDeclarationImpl& operator=( const CSSStyleDeclarationImpl&);

    unsigned long length() const;
    CSSRuleImpl *parentRule() const;
    virtual DOM::DOMString removeProperty(int propertyID, bool notifyChanged = true);
    virtual bool setProperty(int propertyId, const DOM::DOMString &value, bool important = false, bool notifyChanged = true);
    virtual void setProperty(int propertyId, int value, bool important = false, bool notifyChanged = true);
    // this treats integers as pixels!
    // needed for conversion of html attributes
    virtual void setLengthProperty(int id, const DOM::DOMString &value, bool important,bool multiLength = false);
    virtual void setStringProperty(int propertyId, const DOM::DOMString &value, DOM::CSSPrimitiveValue::UnitTypes, bool important = false); // parsed string value
    virtual void setImageProperty(int propertyId, const DOM::DOMString &URL, bool important = false);

    // add a whole, unparsed property
    virtual void setProperty ( const DOMString &propertyString);
    virtual DOM::DOMString item ( unsigned long index );

    virtual DOM::DOMString cssText() const;
    virtual void setCssText(const DOM::DOMString& str);

    virtual bool isStyleDeclaration() { return true; }

    virtual bool parseString( const DOMString &string, bool = false );

    virtual CSSValueImpl *getPropertyCSSValue( int propertyID ) const;
    virtual DOMString getPropertyValue( int propertyID ) const;
    virtual bool getPropertyPriority( int propertyID ) const;

    QPtrList<CSSProperty> *values() { return m_lstValues; }
    void setNode(NodeImpl *_node) { m_node = _node; }
    NodeImpl* node() const { return m_node; }

    void setChanged();

protected:
    DOMString getShortHandValue( const int* properties, int number ) const;
    DOMString get4Values( const int* properties ) const;

    QPtrList<CSSProperty> *m_lstValues;
    NodeImpl *m_node;

private:
    // currently not needed - make sure its not used
    CSSStyleDeclarationImpl(const CSSStyleDeclarationImpl& o);
};

class CSSValueImpl : public StyleBaseImpl
{
public:
    CSSValueImpl();

    virtual ~CSSValueImpl();

    virtual unsigned short cssValueType() const = 0;

    virtual DOM::DOMString cssText() const = 0;

    virtual bool isValue() { return true; }
    virtual bool isFontValue() { return false; }
};

class CSSInheritedValueImpl : public CSSValueImpl
{
public:
    CSSInheritedValueImpl() : CSSValueImpl() {}
    virtual ~CSSInheritedValueImpl() {}

    virtual unsigned short cssValueType() const;
    virtual DOM::DOMString cssText() const;
};

class CSSInitialValueImpl : public CSSValueImpl
{
public:
    virtual unsigned short cssValueType() const;
    virtual DOM::DOMString cssText() const;
};

class CSSValueListImpl : public CSSValueImpl
{
public:
    CSSValueListImpl();

    virtual ~CSSValueListImpl();

    unsigned long length() const { return m_values.count(); }
    CSSValueImpl *item ( unsigned long index ) { return m_values.at(index); }

    virtual bool isValueList() { return true; }

    virtual unsigned short cssValueType() const;

    void append(CSSValueImpl *val);
    virtual DOM::DOMString cssText() const;

protected:
    QPtrList<CSSValueImpl> m_values;
};


class Counter;
class RGBColor;
class Rect;

class CSSPrimitiveValueImpl : public CSSValueImpl
{
public:
    CSSPrimitiveValueImpl();
    CSSPrimitiveValueImpl(int ident);
    CSSPrimitiveValueImpl(double num, CSSPrimitiveValue::UnitTypes type);
    CSSPrimitiveValueImpl(const DOMString &str, CSSPrimitiveValue::UnitTypes type);
    CSSPrimitiveValueImpl(const Counter &c);
    CSSPrimitiveValueImpl( RectImpl *r);
    CSSPrimitiveValueImpl(QRgb color);

    virtual ~CSSPrimitiveValueImpl();

    void cleanup();

    unsigned short primitiveType() const {
	    return m_type;
    }

    /*
     * computes a length in pixels out of the given CSSValue. Need the RenderStyle to get
     * the fontinfo in case val is defined in em or ex.
     *
     * The metrics have to be a bit different for screen and printer output.
     * For screen output we assume 1 inch == 72 px, for printer we assume 300 dpi
     *
     * this is screen/printer dependent, so we probably need a config option for this,
     * and some tool to calibrate.
     */
    int computeLength( khtml::RenderStyle *style, QPaintDeviceMetrics *devMetrics );
    int computeLength( khtml::RenderStyle *style, QPaintDeviceMetrics *devMetrics, double multiplier );
    double computeLengthFloat( khtml::RenderStyle *style, QPaintDeviceMetrics *devMetrics,
                               bool applyZoomFactor = true );

    // use with care!!!
    void setPrimitiveType(unsigned short type) { m_type = type; }
    void setFloatValue ( unsigned short unitType, double floatValue, int &exceptioncode );
    double getFloatValue ( unsigned short/* unitType */) const {
	return m_value.num;
    }

    void setStringValue ( unsigned short stringType, const DOM::DOMString &stringValue, int &exceptioncode );
    DOM::DOMStringImpl *getStringValue () const {
	return ( ( m_type < CSSPrimitiveValue::CSS_STRING ||
		   m_type > CSSPrimitiveValue::CSS_ATTR ||
		   m_type == CSSPrimitiveValue::CSS_IDENT ) ? // fix IDENT
		 0 : m_value.string );
    }
    CounterImpl *getCounterValue () const {
        return ( m_type != CSSPrimitiveValue::CSS_COUNTER ? 0 : m_value.counter );
    }

    RectImpl *getRectValue () const {
	return ( m_type != CSSPrimitiveValue::CSS_RECT ? 0 : m_value.rect );
    }

    QRgb getRGBColorValue () const {
	return ( m_type != CSSPrimitiveValue::CSS_RGBCOLOR ? 0 : m_value.rgbcolor );
    }

    virtual bool isPrimitiveValue() const { return true; }
    virtual unsigned short cssValueType() const;

    int getIdent();

    virtual bool parseString( const DOMString &string, bool = false);
    virtual DOM::DOMString cssText() const;

    virtual bool isQuirkValue() { return false; }

protected:
    int m_type;
    union {
	int ident;
	double num;
	DOM::DOMStringImpl *string;
	CounterImpl *counter;
	RectImpl *rect;
        QRgb rgbcolor;
    } m_value;
};

// This value is used to handle quirky margins in reflow roots (body, td, and th) like WinIE.
// The basic idea is that a stylesheet can use the value __qem (for quirky em) instead of em
// in a stylesheet.  When the quirky value is used, if you're in quirks mode, the margin will
// collapse away inside a table cell.
class CSSQuirkPrimitiveValueImpl : public CSSPrimitiveValueImpl
{
public:
    CSSQuirkPrimitiveValueImpl(double num, CSSPrimitiveValue::UnitTypes type)
      :CSSPrimitiveValueImpl(num, type) {}

    virtual ~CSSQuirkPrimitiveValueImpl() {}

    virtual bool isQuirkValue() { return true; }
};

class CounterImpl : public khtml::Shared<CounterImpl> {
public:
    CounterImpl() { }
    DOMString identifier() const { return m_identifier; }
    DOMString listStyle() const { return m_listStyle; }
    DOMString separator() const { return m_separator; }

    DOMString m_identifier;
    DOMString m_listStyle;
    DOMString m_separator;
};

class RectImpl : public khtml::Shared<RectImpl> {
public:
    RectImpl();
    ~RectImpl();

    CSSPrimitiveValueImpl *top() { return m_top; }
    CSSPrimitiveValueImpl *right() { return m_right; }
    CSSPrimitiveValueImpl *bottom() { return m_bottom; }
    CSSPrimitiveValueImpl *left() { return m_left; }

    void setTop( CSSPrimitiveValueImpl *top );
    void setRight( CSSPrimitiveValueImpl *right );
    void setBottom( CSSPrimitiveValueImpl *bottom );
    void setLeft( CSSPrimitiveValueImpl *left );
protected:
    CSSPrimitiveValueImpl *m_top;
    CSSPrimitiveValueImpl *m_right;
    CSSPrimitiveValueImpl *m_bottom;
    CSSPrimitiveValueImpl *m_left;
};

class CSSImageValueImpl : public CSSPrimitiveValueImpl, public khtml::CachedObjectClient
{
public:
    CSSImageValueImpl(const DOMString &url, StyleBaseImpl *style);
    CSSImageValueImpl();
    virtual ~CSSImageValueImpl();

    khtml::CachedImage *image(khtml::DocLoader* loader);

protected:
    khtml::CachedImage* m_image;
    bool m_accessedImage;
};

class FontFamilyValueImpl : public CSSPrimitiveValueImpl
{
public:
    FontFamilyValueImpl( const QString &string);
    const QString &fontName() const { return parsedFontName; }
    int genericFamilyType() const { return _genericFamilyType; }

    virtual DOM::DOMString cssText() const;

    QString parsedFontName;
private:
    int _genericFamilyType;
};

class FontValueImpl : public CSSValueImpl
{
public:
    FontValueImpl();
    virtual ~FontValueImpl();

    virtual unsigned short cssValueType() const { return CSSValue::CSS_CUSTOM; }
    
    virtual DOM::DOMString cssText() const;
    
    virtual bool isFontValue() { return true; }

    CSSPrimitiveValueImpl *style;
    CSSPrimitiveValueImpl *variant;
    CSSPrimitiveValueImpl *weight;
    CSSPrimitiveValueImpl *size;
    CSSPrimitiveValueImpl *lineHeight;
    CSSValueListImpl *family;
};

// Used for text-shadow and box-shadow
class ShadowValueImpl : public CSSValueImpl
{
public:
    ShadowValueImpl(CSSPrimitiveValueImpl* _x, CSSPrimitiveValueImpl* _y,
                    CSSPrimitiveValueImpl* _blur, CSSPrimitiveValueImpl* _color);
    virtual ~ShadowValueImpl();

    virtual unsigned short cssValueType() const { return CSSValue::CSS_CUSTOM; }

    virtual DOM::DOMString cssText() const;

    CSSPrimitiveValueImpl* x;
    CSSPrimitiveValueImpl* y;
    CSSPrimitiveValueImpl* blur;
    CSSPrimitiveValueImpl* color;
};

// Used by box-flex-group-transition
class FlexGroupTransitionValueImpl : public CSSValueImpl
{
public:
    FlexGroupTransitionValueImpl();
    FlexGroupTransitionValueImpl(unsigned int _group1, 
                                 unsigned int _group2,
                                 CSSPrimitiveValueImpl* _length);
    virtual ~FlexGroupTransitionValueImpl();
    
    virtual unsigned short cssValueType() const { return CSSValue::CSS_CUSTOM; }
    
    virtual DOM::DOMString cssText() const;
    
    bool isAuto() const { return autoValue; }

    bool autoValue;
    unsigned int group1;
    unsigned int group2;
    CSSPrimitiveValueImpl* length;
};

// ------------------------------------------------------------------------------

// another helper class
class CSSProperty
{
public:
    CSSProperty()
    {
	m_id = -1;
	m_bImportant = false;
	m_value = 0;
    }
    CSSProperty(const CSSProperty& o)
    {
        m_id = o.m_id;
        m_bImportant = o.m_bImportant;
        m_value = o.m_value;
        if (m_value) m_value->ref();
    }
    ~CSSProperty() {
	if(m_value) m_value->deref();
    }

    void setValue(CSSValueImpl *val) {
	if ( val != m_value ) {
	    if(m_value) m_value->deref();
	    m_value = val;
	    if(m_value) m_value->ref();
	}
    }

    int id() const { return m_id; }
    bool isImportant() const { return m_bImportant; }
    
    CSSValueImpl *value() const { return m_value; }
    
    DOM::DOMString cssText() const;

    // make sure the following fits in 4 bytes.
    int  m_id;
    bool m_bImportant;
protected:
    CSSValueImpl *m_value;

private:
    CSSProperty &operator=(const CSSProperty&);
};


} // namespace

#endif
