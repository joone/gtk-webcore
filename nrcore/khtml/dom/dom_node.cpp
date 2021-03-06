/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
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

#include "dom/dom_doc.h"
#include "dom/dom_exception.h"
#include "dom/dom2_events.h"
#include "xml/dom_docimpl.h"
#include "xml/dom_elementimpl.h"
#include "xml/dom2_eventsimpl.h"

#include <qrect.h>

using namespace DOM;

#if KHTML_NO_EXCEPTIONS    
int DOM::_exceptioncode = 0;
#endif    

NamedNodeMap::NamedNodeMap()
{
    impl = 0;
}

NamedNodeMap::NamedNodeMap(const NamedNodeMap &other)
{
    impl = other.impl;
    if (impl) impl->ref();
}

NamedNodeMap::NamedNodeMap(NamedNodeMapImpl *i)
{
    impl = i;
    if (impl) impl->ref();
}

NamedNodeMap &NamedNodeMap::operator = (const NamedNodeMap &other)
{
    if ( impl != other.impl ) {
    if(impl) impl->deref();
    impl = other.impl;
    if(impl) impl->ref();
    }
    return *this;
}

NamedNodeMap::~NamedNodeMap()
{
    if(impl) impl->deref();
}

Node NamedNodeMap::getNamedItem( const DOMString &name ) const
{
    return getNamedItemNS(DOMString(), name);
}

Node NamedNodeMap::setNamedItem( const Node &arg )
{
    return setNamedItemNS(arg);
}

Node NamedNodeMap::removeNamedItem( const DOMString &name )
{
    return removeNamedItemNS(DOMString(), name);
}

Node NamedNodeMap::item( unsigned long index ) const
{
    if (!impl) return 0;
    return impl->item(index);
}

Node NamedNodeMap::getNamedItemNS( const DOMString &namespaceURI, const DOMString &localName ) const
{
    if (!impl) return 0;
    return impl->getNamedItem(impl->mapId(namespaceURI, localName, true));
}

Node NamedNodeMap::setNamedItemNS( const Node &arg )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    Node r = impl->setNamedItem(arg.impl, exceptioncode);
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return Node(); }
#else
        throw DOMException(exceptioncode);
#endif    
    return r;
}

Node NamedNodeMap::removeNamedItemNS( const DOMString &namespaceURI, const DOMString &localName )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    Node r = impl->removeNamedItem(impl->mapId(namespaceURI, localName, true), exceptioncode);
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return Node(); }
#else
        throw DOMException(exceptioncode);
#endif    
    return r;
}

unsigned long NamedNodeMap::length() const
{
    if (!impl) return 0;
    return impl->length();
}

#if KHTML_NO_EXCEPTIONS    
NamedNodeMapImpl *NamedNodeMap::handle() const
#else
NamedNodeMapImpl *NamedNodeMap::handle() const throw()
#endif    
{
    return impl;
}

#if KHTML_NO_EXCEPTIONS    
bool NamedNodeMap::isNull() const
#else
bool NamedNodeMap::isNull() const throw()
#endif    
{
    return (impl == 0);
}

// ---------------------------------------------------------------------------

Node::Node()
{
    impl = 0;
}

Node::Node(const Node &other)
{
    impl = other.impl;
    if(impl) impl->ref();
}

Node::Node( NodeImpl *i )
{
    impl = i;
    if(impl) impl->ref();
}

Node &Node::operator = (const Node &other)
{
    if(impl != other.impl) {
    if(impl) impl->deref();
    impl = other.impl;
    if(impl) impl->ref();
    }
    return *this;
}

bool Node::operator == (const Node &other)
{
    return (impl == other.impl);
}

bool Node::operator != (const Node &other)
{
    return !(impl == other.impl);
}

Node::~Node()
{
    if(impl) impl->deref();
}

DOMString Node::nodeName() const
{
    if(impl) return impl->nodeName();
    return DOMString();
}

DOMString Node::nodeValue() const
{
    // ### should throw exception on plain node ?
    if(impl) return impl->nodeValue();
    return DOMString();
}

void Node::setNodeValue( const DOMString &_str )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return; }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    

    int exceptioncode = 0;
    if(impl) impl->setNodeValue( _str,exceptioncode );
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return; }
#else
	throw DOMException(exceptioncode);
#endif    
}

unsigned short Node::nodeType() const
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return 0; }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->nodeType();
}

Node Node::parentNode() const
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->parentNode();
}

NodeList Node::childNodes() const
{
    if (!impl) return 0;
    return impl->childNodes();
}

Node Node::firstChild() const
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->firstChild();
}

Node Node::lastChild() const
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->lastChild();
}

Node Node::previousSibling() const
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->previousSibling();
}

Node Node::nextSibling() const
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->nextSibling();
}

NamedNodeMap Node::attributes() const
{
    if (!impl || !impl->isElementNode()) return 0;
    return static_cast<ElementImpl*>(impl)->attributes();
}

Document Node::ownerDocument() const
{
    // braindead DOM spec says that ownerDocument
    // should return null if called on the document node
    // we don't do that in the *impl tree to avoid excessive if()'s
    // so we simply hack it here in one central place.
    if (!impl || impl->getDocument() == impl) return Document(false);

    return impl->getDocument();
}

Node Node::insertBefore( const Node &newChild, const Node &refChild )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    NodeImpl *r = impl->insertBefore( newChild.impl, refChild.impl, exceptioncode );
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return Node(); }
#else
	throw DOMException(exceptioncode);
#endif    
    return r;
}

Node Node::replaceChild( const Node &newChild, const Node &oldChild )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    NodeImpl *r = impl->replaceChild( newChild.impl, oldChild.impl, exceptioncode );
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return Node(); }
#else
	throw DOMException(exceptioncode);
#endif    
    return r;
}

Node Node::removeChild( const Node &oldChild )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    NodeImpl *r = impl->removeChild( oldChild.impl, exceptioncode );
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return Node(); }
#else
	throw DOMException(exceptioncode);
#endif    
    return r;
}

Node Node::appendChild( const Node &newChild )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return Node(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    NodeImpl *r = impl->appendChild( newChild.impl, exceptioncode );
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return Node(); }
#else
	throw DOMException(exceptioncode);
#endif    
    return r;
}

bool Node::hasAttributes()
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return false; }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    if (!impl->isElementNode()) return false;
    ElementImpl* e = static_cast<ElementImpl*>(impl);
    return e->attributes(true) && e->attributes(true)->length();
}

bool Node::hasChildNodes(  )
{
    if (!impl) return false;
    return impl->hasChildNodes();
}

Node Node::cloneNode( bool deep )
{
    if (!impl) return 0;
    return impl->cloneNode( deep  );
}

void Node::normalize (  )
{
    if (!impl) return;
    impl->normalize();
}

bool Node::isSupported( const DOMString &feature,
                        const DOMString &version ) const
{
    DOMString upFeature = feature.upper();
    return (upFeature == "HTML" ||
            upFeature == "XML" ||
            upFeature == "CORE");
}

DOMString Node::namespaceURI(  ) const
{
    if (!impl) return DOMString();
    return impl->getDocument()->namespaceURI(impl->id());
}

DOMString Node::prefix(  ) const
{
    if (!impl) return DOMString();
    return impl->prefix();
}

void Node::setPrefix(const DOMString &prefix )
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return; }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    int exceptioncode = 0;
    impl->setPrefix(prefix,exceptioncode);
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return; }
#else
        throw DOMException(exceptioncode);
#endif    
}

DOMString Node::localName(  ) const
{
    if (!impl) return DOMString();
    return impl->localName();
}

void Node::addEventListener(const DOMString &type,
			  EventListener *listener,
			  const bool useCapture)
{
    if (!impl) return;
    impl->addEventListener(EventImpl::typeToId(type),listener,useCapture);
}

void Node::removeEventListener(const DOMString &type,
			     EventListener *listener,
			     bool useCapture)
{
    if (!impl) return;
    impl->removeEventListener(EventImpl::typeToId(type),listener,useCapture);
}

bool Node::dispatchEvent(const Event &evt)
{
    if (!impl)
#if KHTML_NO_EXCEPTIONS    
	{ _exceptioncode = DOMException::INVALID_STATE_ERR; return false; }
#else
	throw DOMException(DOMException::INVALID_STATE_ERR);
#endif    

    int exceptioncode = 0;
    bool r = impl->dispatchEvent(evt.handle(),exceptioncode);
    if (exceptioncode)
#if KHTML_NO_EXCEPTIONS    
        { _exceptioncode = exceptioncode; return false; }
#else
	throw DOMException(exceptioncode);
#endif    
    return r;
}


unsigned int Node::elementId() const
{
    if (!impl) return 0;
    return impl->id();
}

unsigned long Node::index() const
{
    if (!impl) return 0;
    return impl->nodeIndex();
}

QString Node::toHTML()
{
    if (!impl) return QString::null;
    return impl->toHTML();
}

void Node::applyChanges()
{
    if (!impl) return;
    impl->recalcStyle( NodeImpl::Inherit );
}

QRect Node::getRect()
{
#if KHTML_NO_EXCEPTIONS    
    if (!impl) { _exceptioncode = DOMException::NOT_FOUND_ERR; return QRect(); }
#else
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
#endif    
    return impl->getRect();
}

bool Node::isNull() const
{
    return (impl == 0);
}

NodeImpl *Node::handle() const
{
    return impl;
}

bool Node::isContentEditable() const
{
    if (!impl) return false;
    return impl->isContentEditable();
}

//-----------------------------------------------------------------------------

NodeList::NodeList()
{
    impl = 0;
}

NodeList::NodeList(const NodeList &other)
{
    impl = other.impl;
    if(impl) impl->ref();
}

NodeList::NodeList(const NodeListImpl *i)
{
    impl = const_cast<NodeListImpl *>(i);
    if(impl) impl->ref();
}

NodeList &NodeList::operator = (const NodeList &other)
{
    if ( impl != other.impl ) {
    if(impl) impl->deref();
    impl = other.impl;
    if(impl) impl->ref();
    }
    return *this;
}

NodeList::~NodeList()
{
    if(impl) impl->deref();
}

Node NodeList::item( unsigned long index ) const
{
    if (!impl) return 0;
    return impl->item(index);
}

unsigned long NodeList::length() const
{
    if (!impl) return 0;
    return impl->length();
}

NodeListImpl *NodeList::handle() const
{
    return impl;
}

bool NodeList::isNull() const
{
    return (impl == 0);
}

