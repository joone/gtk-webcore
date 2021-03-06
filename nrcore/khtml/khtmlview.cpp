/* This file is part of the KDE project
 *
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999 Lars Knoll <knoll@kde.org>
 *                     1999 Antti Koivisto <koivisto@kde.org>
 *                     2000 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2004 Apple Computer, Inc.
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
 */
#include "khtmlview.moc"

#include "khtmlview.h"

#include "khtml_part.h"
#include "khtml_events.h"

#include "html/html_documentimpl.h"
#include "html/html_inlineimpl.h"
#include "html/html_formimpl.h"
#include "rendering/render_arena.h"
#include "rendering/render_object.h"
#include "rendering/render_canvas.h"
#include "rendering/render_style.h"
#include "rendering/render_replaced.h"
#include "rendering/render_line.h"
#include "rendering/render_text.h"
#include "xml/dom_nodeimpl.h"
#include "xml/dom_selection.h"
#include "xml/dom2_eventsimpl.h"
#include "css/cssstyleselector.h"
#include "misc/htmlhashes.h"
#include "misc/helper.h"
#include "khtml_settings.h"
#include "khtml_printsettings.h"
#include "khtmlpart_p.h"

#include <kcursor.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kprinter.h>

#include <qtooltip.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <kapplication.h>

#include <kimageio.h>
#include <assert.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <qobjectlist.h>

#if APPLE_CHANGES
#include "KWQAccObjectCache.h"
#endif

#define PAINT_BUFFER_HEIGHT 128

#define INSTRUMENT_LAYOUT_SCHEDULING 0

using namespace DOM;
using namespace khtml;
class KHTMLToolTip;

#ifndef QT_NO_TOOLTIP

class KHTMLToolTip : public QToolTip
{
public:
    KHTMLToolTip(KHTMLView *view,  KHTMLViewPrivate* vp) : QToolTip(view->viewport())
    {
        m_view = view;
        m_viewprivate = vp;
    };

protected:
    virtual void maybeTip(const QPoint &);

private:

    KHTMLView *m_view;
    KHTMLViewPrivate* m_viewprivate;
};

#endif

class KHTMLViewPrivate {
    friend class KHTMLToolTip;
public:
    KHTMLViewPrivate()
    {
        repaintRects = 0;
        underMouse = 0;
        reset();
#if !APPLE_CHANGES
        tp=0;
        paintBuffer=0;
        formCompletions=0;
#endif
        layoutTimerId = 0;
        allDataReceivedWhenTimerSet = false;
        mousePressed = false;
#ifndef QT_NO_TOOLTIP
        tooltip = 0;
#endif
        doFullRepaint = true;
        isTransparent = false;
#if APPLE_CHANGES
        vmode = hmode = QScrollView::Auto;
        firstLayout = true;
        needToInitScrollBars = true;
#else
        prevScrollbarVisible = true;
#endif
    }
    ~KHTMLViewPrivate()
    {
#if !APPLE_CHANGES
        delete formCompletions;
        delete tp; tp = 0;
        delete paintBuffer; paintBuffer =0;
#endif
        
        if (underMouse)
	    underMouse->deref();
#ifndef QT_NO_TOOLTIP
	delete tooltip;
#endif
        delete repaintRects;
    }
    void reset()
    {
        if (underMouse)
	    underMouse->deref();
	underMouse = 0;
        linkPressed = false;
        useSlowRepaints = false;
        originalNode = 0;
        dragTarget = 0;
	borderTouched = false;
#if !APPLE_CHANGES
#ifndef KHTML_NO_SCROLLBARS
        vmode = QScrollView::Auto;
        hmode = QScrollView::Auto;
#else
        vmode = QScrollView::AlwaysOff;
        hmode = QScrollView::AlwaysOff;
#endif
#endif
        scrollBarMoved = false;
        ignoreWheelEvents = false;
	borderX = 30;
	borderY = 30;
	clickX = -1;
	clickY = -1;
        prevMouseX = -1;
        prevMouseY = -1;
	clickCount = 0;
	isDoubleClick = false;
	scrollingSelf = false;
	layoutTimerId = 0;
        allDataReceivedWhenTimerSet = false;
        mousePressed = false;
        doFullRepaint = true;
        layoutSchedulingEnabled = true;
        layoutSuppressed = false;
        layoutCount = 0;
#if APPLE_CHANGES
        firstLayout = true;
#endif
        if (repaintRects)
            repaintRects->clear();
    }

#if !APPLE_CHANGES
    QPainter *tp;
    QPixmap  *paintBuffer;
#endif
    NodeImpl *underMouse;

    // the node that was selected when enter was pressed
    NodeImpl *originalNode;

    bool borderTouched:1;
    bool borderStart:1;
    bool scrollBarMoved:1;
    bool doFullRepaint:1;
    
    QScrollView::ScrollBarMode vmode;
    QScrollView::ScrollBarMode hmode;
#if !APPLE_CHANGES
    bool prevScrollbarVisible;
#endif
    bool linkPressed;
    bool useSlowRepaints;
    bool ignoreWheelEvents;

    int borderX, borderY;
#if !APPLE_CHANGES
    KSimpleConfig *formCompletions;
#endif

    int clickX, clickY, clickCount;
    bool isDoubleClick;

    int prevMouseX, prevMouseY;
    bool scrollingSelf;
    int layoutTimerId;
    bool allDataReceivedWhenTimerSet;
    
    bool layoutSchedulingEnabled;
    bool layoutSuppressed;
    int layoutCount;

#if APPLE_CHANGES
    bool firstLayout;
    bool needToInitScrollBars;
#endif
    bool mousePressed;
    bool isTransparent;
#ifndef QT_NO_TOOLTIP
    KHTMLToolTip *tooltip;
#endif
    
    // Used by objects during layout to communicate repaints that need to take place only
    // after all layout has been completed.
    QPtrList<RenderObject::RepaintInfo>* repaintRects;
    
    Node dragTarget;
};

#ifndef QT_NO_TOOLTIP

void KHTMLToolTip::maybeTip(const QPoint& /*p*/)
{
    DOM::NodeImpl *node = m_viewprivate->underMouse;
    while ( node ) {
        if ( node->isElementNode() ) {
            AtomicString s = static_cast<DOM::ElementImpl*>( node )->getAttribute(ATTR_TITLE);
            if (!s.isEmpty()) {
                QRect r( m_view->contentsToViewport( node->getRect().topLeft() ), node->getRect().size() );
                tip( r,  s.string() );
            }
            break;
        }
        node = node->parentNode();
    }
}
#endif

KHTMLView::KHTMLView( KHTMLPart *part, QWidget *parent, const char *name)
    : QScrollView( parent, name, WResizeNoErase | WRepaintNoErase | WPaintUnclipped ),
      _refCount(1)
{
#if KWIQ
    QOBJECT_TYPE(KHTMLView);
#endif
    m_medium = "screen";

    m_part = part;
#if APPLE_CHANGES
    m_part->ref();
#endif
    d = new KHTMLViewPrivate;

#if !APPLE_CHANGES
    QScrollView::setVScrollBarMode(d->vmode);
    QScrollView::setHScrollBarMode(d->hmode);
#endif
    
    connect(kapp, SIGNAL(kdisplayPaletteChanged()), this, SLOT(slotPaletteChanged()));
    connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotScrollBarMoved()));

    // initialize QScrollview
    enableClipper(true);
    viewport()->setMouseTracking(true);
    viewport()->setBackgroundMode(NoBackground);

    KImageIO::registerFormats();

#ifndef QT_NO_TOOLTIP
    d->tooltip = new KHTMLToolTip( this, d );
#endif

    init();

    viewport()->show();
}

KHTMLView::~KHTMLView()
{
#if APPLE_CHANGES
    resetScrollBars();
#endif

    assert(_refCount == 0);

    if (m_part)
    {
        //WABA: Is this Ok? Do I need to deref it as well?
        //Does this need to be done somewhere else?
        DOM::DocumentImpl *doc = m_part->xmlDocImpl();
        if (doc)
            doc->detach();

#if APPLE_CHANGES
        m_part->deref();
#endif
    }

    delete d; d = 0;
}

void KHTMLView::clearPart()
{
    if (m_part){
        m_part->deref();
        m_part = 0;
    }
}

#if APPLE_CHANGES
void KHTMLView::resetScrollBars()
{
    // Reset the document's scrollbars back to our defaults before we yield the floor.
    d->firstLayout = true;
    suppressScrollBars(true);
    QScrollView::setVScrollBarMode(d->vmode);
    QScrollView::setHScrollBarMode(d->hmode);
    suppressScrollBars(false);
}
#endif

void KHTMLView::init()
{
#if !APPLE_CHANGES
    if(!d->paintBuffer) d->paintBuffer = new QPixmap(PAINT_BUFFER_HEIGHT, PAINT_BUFFER_HEIGHT);
    if(!d->tp) d->tp = new QPainter();
#endif

    setFocusPolicy(QWidget::StrongFocus);
    viewport()->setFocusPolicy( QWidget::WheelFocus );
    viewport()->setFocusProxy(this);

    _marginWidth = -1; // undefined
    _marginHeight = -1;
    _width = 0;
    _height = 0;

    setAcceptDrops(true);
    
#if !APPLE_CHANGES
    resizeContents(visibleWidth(), visibleHeight());
#endif
}

void KHTMLView::clear()
{
//    viewport()->erase();

    setStaticBackground(false);
    
    m_part->clearSelection();

    d->reset();
    killTimers();
    emit cleared();

#if APPLE_CHANGES
    suppressScrollBars(true);
#else
    QScrollView::setHScrollBarMode(d->hmode);
    if (d->vmode==Auto)
        QScrollView::setVScrollBarMode(d->prevScrollbarVisible?AlwaysOn:Auto);
    else
        QScrollView::setVScrollBarMode(d->vmode);
    resizeContents(visibleWidth(), visibleHeight());
#endif
}

void KHTMLView::hideEvent(QHideEvent* e)
{
//    viewport()->setBackgroundMode(PaletteBase);
    QScrollView::hideEvent(e);
}

void KHTMLView::showEvent(QShowEvent* e)
{
//    viewport()->setBackgroundMode(NoBackground);
    QScrollView::showEvent(e);
}

void KHTMLView::resizeEvent (QResizeEvent* e)
{
    QScrollView::resizeEvent(e);

#if !APPLE_CHANGES
    int w = visibleWidth();
    int h = visibleHeight();

    layout();

    //  this is to make sure we get the right width even if the scrolbar has dissappeared
    // due to the size change.
    if(visibleHeight() != h || visibleWidth() != w)
        layout();
#endif
    if ( m_part && m_part->xmlDocImpl() )
        m_part->xmlDocImpl()->dispatchWindowEvent( EventImpl::RESIZE_EVENT, false, false );

    KApplication::sendPostedEvents(viewport(), QEvent::Paint);
}

#if APPLE_CHANGES
void KHTMLView::initScrollBars()
{
    if (!d->needToInitScrollBars)
        return;
    d->needToInitScrollBars = false;
    setScrollBarsMode(hScrollBarMode());
}
#endif

#if !APPLE_CHANGES
// this is to get rid of a compiler virtual overload mismatch warning. do not remove
void KHTMLView::drawContents( QPainter*)
{
}

void KHTMLView::drawContents( QPainter *p, int ex, int ey, int ew, int eh )
{
    //kdDebug( 6000 ) << "drawContents x=" << ex << ",y=" << ey << ",w=" << ew << ",h=" << eh << endl;
    if(!m_part->xmlDocImpl() || !m_part->xmlDocImpl()->renderer()) {
        p->fillRect(ex, ey, ew, eh, palette().normal().brush(QColorGroup::Base));
        return;
    }
    if ( d->paintBuffer->width() < visibleWidth() )
        d->paintBuffer->resize(visibleWidth(),PAINT_BUFFER_HEIGHT);

    int py=0;
    while (py < eh) {
        int ph = eh-py < PAINT_BUFFER_HEIGHT ? eh-py : PAINT_BUFFER_HEIGHT;
        d->tp->begin(d->paintBuffer);
        d->tp->translate(-ex, -ey-py);
        d->tp->fillRect(ex, ey+py, ew, ph, palette().normal().brush(QColorGroup::Base));
        m_part->xmlDocImpl()->renderer()->print(d->tp, ex, ey+py, ew, ph, 0, 0);
#ifdef BOX_DEBUG
	if (m_part->xmlDocImpl()->focusNode())
	{
	    d->tp->setBrush(Qt::NoBrush);
	    d->tp->drawRect(m_part->xmlDocImpl()->focusNode()->getRect());
	}
#endif
        d->tp->end();

        p->drawPixmap(ex, ey+py, *d->paintBuffer, 0, 0, ew, ph);
        py += PAINT_BUFFER_HEIGHT;
    }

    // EDIT FIXME: KDE needs to draw the caret here.

    khtml::DrawContentsEvent event( p, ex, ey, ew, eh );
    QApplication::sendEvent( m_part, &event );

}

#endif // !APPLE_CHANGES

void KHTMLView::setMarginWidth(int w)
{
    // make it update the rendering area when set
    _marginWidth = w;
}

void KHTMLView::setMarginHeight(int h)
{
    // make it update the rendering area when set
    _marginHeight = h;
}


void KHTMLView::adjustViewSize()
{
    if( m_part->xmlDocImpl() ) {
        DOM::DocumentImpl *document = m_part->xmlDocImpl();

        khtml::RenderCanvas* root = static_cast<khtml::RenderCanvas *>(document->renderer());
        if ( !root )
            return;
        
        int docw = root->docWidth();
        int doch = root->docHeight();
    
        resizeContents(docw, doch);
    }
}

void KHTMLView::applyBodyScrollQuirk(khtml::RenderObject* o, ScrollBarMode& hMode, ScrollBarMode& vMode)
{
    // Handle the overflow:hidden/scroll quirk for the body elements.  WinIE treats
    // overflow:hidden and overflow:scroll on <body> as applying to the document's
    // scrollbars.  The CSS2.1 draft has even added a sentence, "HTML UAs may apply overflow
    // specified on the body or HTML elements to the viewport."  Since WinIE and Mozilla both
    // do it, we will do it too for <body> elements.
    switch(o->style()->overflow()) {
        case OHIDDEN:
            hMode = vMode = AlwaysOff;
            break;
        case OSCROLL:
            hMode = vMode = AlwaysOn;
            break;
        case OAUTO:
	    hMode = vMode = Auto;
            break;
        default:
            // Don't set it at all.
            ;
    }
}

bool KHTMLView::inLayout() const
{
    return d->layoutSuppressed;
}

int KHTMLView::layoutCount() const
{
    return d->layoutCount;
}

bool KHTMLView::needsFullRepaint() const
{
    return d->doFullRepaint;
}

void KHTMLView::addRepaintInfo(RenderObject* o, const QRect& r)
{
    if (!d->repaintRects) {
        d->repaintRects = new QPtrList<RenderObject::RepaintInfo>;
        d->repaintRects->setAutoDelete(true);
    }
    
    d->repaintRects->append(new RenderObject::RepaintInfo(o, r));
}

void KHTMLView::layout()
{
    if (d->layoutSuppressed)
        return;
    
    d->layoutSchedulingEnabled=false;
    killTimer(d->layoutTimerId);
    d->layoutTimerId = 0;
    d->allDataReceivedWhenTimerSet = false;

    if (!m_part) {
        // FIXME: Do we need to set _width here?
        // FIXME: Should we set _height here too?
        _width = visibleWidth();
        return;
    }

    DOM::DocumentImpl* document = m_part->xmlDocImpl();
    if (!document) {
        // FIXME: Should we set _height here too?
        _width = visibleWidth();
        return;
    }

    khtml::RenderCanvas* root = static_cast<khtml::RenderCanvas*>(document->renderer());
    if (!root) {
        // FIXME: Do we need to set _width or _height here?
        return;
    }

    ScrollBarMode hMode = d->hmode;
    ScrollBarMode vMode = d->vmode;
    
    if (document->isHTMLDocument()) {
        NodeImpl *body = static_cast<HTMLDocumentImpl*>(document)->body();
        if (body && body->renderer()) {
            if (body->id() == ID_FRAMESET) {
                body->renderer()->setNeedsLayout(true);
                vMode = AlwaysOff;
                hMode = AlwaysOff;
            }
            else if (body->id() == ID_BODY)
                applyBodyScrollQuirk(body->renderer(), hMode, vMode); // Only applies to HTML UAs, not to XML/XHTML UAs
        }
    }

#ifdef INSTRUMENT_LAYOUT_SCHEDULING
    if (d->firstLayout && !document->ownerElement())
        printf("Elapsed time before first layout: %d\n", document->elapsedTime());
#endif

    d->doFullRepaint = d->firstLayout || root->printingMode();
    if (d->repaintRects)
        d->repaintRects->clear();

#if APPLE_CHANGES
    // Now set our scrollbar state for the layout.
    ScrollBarMode currentHMode = hScrollBarMode();
    ScrollBarMode currentVMode = vScrollBarMode();

    if (d->firstLayout || (hMode != currentHMode || vMode != currentVMode)) {
        suppressScrollBars(true);
        if (d->firstLayout) {
            d->firstLayout = false;
            
            // Set the initial vMode to AlwaysOn if we're auto.
            if (vMode == Auto)
                QScrollView::setVScrollBarMode(AlwaysOn); // This causes a vertical scrollbar to appear.
            // Set the initial hMode to AlwaysOff if we're auto.
            if (hMode == Auto)
                QScrollView::setHScrollBarMode(AlwaysOff); // This causes a horizontal scrollbar to disappear.
        }
        
        if (hMode == vMode)
            QScrollView::setScrollBarsMode(hMode);
        else {
            QScrollView::setHScrollBarMode(hMode);
            QScrollView::setVScrollBarMode(vMode);
        }

        suppressScrollBars(false, true);
    }
#else
    QScrollView::setHScrollBarMode(hMode);
    QScrollView::setVScrollBarMode(vMode);
#endif

    int oldHeight = _height;
    int oldWidth = _width;
    
    _height = visibleHeight();
    _width = visibleWidth();

    if (oldHeight != _height || oldWidth != _width)
        d->doFullRepaint = true;
    
    RenderLayer* layer = root->layer();
     
    if (!d->doFullRepaint) {
        layer->computeRepaintRects();
        root->repaintObjectsBeforeLayout();
    }

    root->layout();

    m_part->invalidateSelection();
        
    //kdDebug( 6000 ) << "TIME: layout() dt=" << qt.elapsed() << endl;
   
    d->layoutSchedulingEnabled=true;
    d->layoutSuppressed = false;

    if (!root->printingMode())
        resizeContents(layer->width(), layer->height());

    // Now update the positions of all layers.
    layer->updateLayerPositions(d->doFullRepaint);

#if APPLE_CHANGES
    // We update our widget positions right after doing a layout.
    root->updateWidgetPositions();
#endif
    
    if (d->repaintRects && !d->repaintRects->isEmpty()) {
        // FIXME: Could optimize this and have objects removed from this list
        // if they ever do full repaints.
        RenderObject::RepaintInfo* r;
        QPtrListIterator<RenderObject::RepaintInfo> it(*d->repaintRects);
        for ( ; (r = it.current()); ++it)
            r->m_object->repaintRectangle(r->m_repaintRect);
        d->repaintRects->clear();
    }
    
    d->layoutCount++;
#if APPLE_CHANGES
    if (KWQAccObjectCache::accessibilityEnabled())
        root->document()->getOrCreateAccObjectCache()->postNotification(root, "AXLayoutComplete");
#endif

    if (root->needsLayout()) {
        //qDebug("needs layout, delaying repaint");
        scheduleRelayout();
        return;
    }
    setStaticBackground(d->useSlowRepaints);
}

//
// Event Handling
//
/////////////////

void KHTMLView::viewportMousePressEvent( QMouseEvent *_mouse )
{
    if(!m_part->xmlDocImpl()) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    //kdDebug( 6000 ) << "\nmousePressEvent: x=" << xm << ", y=" << ym << endl;

    d->isDoubleClick = false;
    d->mousePressed = true;

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MousePress );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );

#if !APPLE_CHANGES
    if (d->clickCount > 0 &&
        QPoint(d->clickX-xm,d->clickY-ym).manhattanLength() <= QApplication::startDragDistance())
	d->clickCount++;
    else {
	d->clickCount = 1;
	d->clickX = xm;
	d->clickY = ym;
    }
#else
    if (KWQ(m_part)->passSubframeEventToSubframe(mev))
        return;

    d->clickX = xm;
    d->clickY = ym;
    d->clickCount = _mouse->clickCount();
#endif    

    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEDOWN_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,true,DOM::NodeImpl::MousePress);

    if (!swallowEvent) {
	khtml::MousePressEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event );
#if APPLE_CHANGES 
        // Many AK widgets run their own event loops and consume events while the mouse is down.
        // When they finish, currentEvent is the mouseUp that they exited on.  We need to update
        // the khtml state with this mouseUp, which khtml never saw.
        // If this event isn't a mouseUp, we assume that the mouseUp will be coming later.  There
        // is a hole here if the widget consumes the mouseUp and subsequent events.
        if (KWQ(m_part)->lastEventIsMouseUp()) {
            d->mousePressed = false;
        }
#endif        
	emit m_part->nodeActivated(mev.innerNode);
    }
}

void KHTMLView::viewportMouseDoubleClickEvent( QMouseEvent *_mouse )
{
    if(!m_part->xmlDocImpl()) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    //kdDebug( 6000 ) << "mouseDblClickEvent: x=" << xm << ", y=" << ym << endl;

    d->isDoubleClick = true;
#if APPLE_CHANGES
    // We get this instead of a second mouse-up 
    d->mousePressed = false;
#endif

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MouseDblClick );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );

#if APPLE_CHANGES
    if (KWQ(m_part)->passSubframeEventToSubframe(mev))
        return;

    d->clickCount = _mouse->clickCount();
    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEUP_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,false,DOM::NodeImpl::MouseRelease);
    
    dispatchMouseEvent(EventImpl::CLICK_EVENT,mev.innerNode.handle(),true,
			   d->clickCount,_mouse,true,DOM::NodeImpl::MouseRelease);

    // Qt delivers a release event AND a double click event.
    if (!swallowEvent) {
	khtml::MouseReleaseEvent event1( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event1 );

	khtml::MouseDoubleClickEvent event2( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event2 );
    }

#else
    // We do the same thing as viewportMousePressEvent() here, since the DOM does not treat
    // single and double-click events as separate (only the detail, i.e. number of clicks differs)
    // In other words an even detail value for a mouse click event means a double click, and an
    // odd detail value means a single click
    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEDOWN_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,true,DOM::NodeImpl::MouseDblClick);

    if (!swallowEvent) {
	khtml::MouseDoubleClickEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event );

	// ###
	//if ( url.length() )
	//emit doubleClick( url.string(), _mouse->button() );
    }
#endif    
}

static bool isSubmitImage(DOM::NodeImpl *node)
{
    return node
        && node->isHTMLElement()
        && node->id() == ID_INPUT
        && static_cast<HTMLInputElementImpl*>(node)->inputType() == HTMLInputElementImpl::IMAGE;
}

void KHTMLView::viewportMouseMoveEvent( QMouseEvent * _mouse )
{
    if(!m_part->xmlDocImpl()) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    // Treat mouse move events while the mouse is pressed as "read-only" in prepareMouseEvent.
    // This means that :hover and :active freeze in the state they were in when the mouse
    // was pressed, rather than updating for nodes the mouse moves over as you hold the mouse down.
    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MouseMove );
    m_part->xmlDocImpl()->prepareMouseEvent( d->mousePressed, xm, ym, &mev );
#if APPLE_CHANGES
    if (KWQ(m_part)->passSubframeEventToSubframe(mev))
        return;
#endif

    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEMOVE_EVENT,mev.innerNode.handle(),false,
                                           0,_mouse,true,DOM::NodeImpl::MouseMove);

#if !APPLE_CHANGES
    if (d->clickCount > 0 &&
        QPoint(d->clickX-xm,d->clickY-ym).manhattanLength() > QApplication::startDragDistance()) {
	d->clickCount = 0;  // moving the mouse outside the threshold invalidates the click
    }
#endif

    // execute the scheduled script. This is to make sure the mouseover events come after the mouseout events
    m_part->executeScheduledScript();

    khtml::RenderStyle* style = (mev.innerNode.handle() && mev.innerNode.handle()->renderer() &&
                                 mev.innerNode.handle()->renderer()->style()) ? mev.innerNode.handle()->renderer()->style() : 0;
    QCursor c;
    if (style && style->cursor() == CURSOR_AUTO && style->cursorImage()
        && !(style->cursorImage()->pixmap().isNull())) {
        /* First of all it works: Check out http://www.iam.unibe.ch/~schlpbch/cursor.html
         *
         * But, I don't know what exactly we have to do here: rescale to 32*32, change to monochrome..
         */
        //kdDebug( 6000 ) << "using custom cursor" << endl;
        const QPixmap p = style->cursorImage()->pixmap();
        // ### fix
        c = QCursor(p);
    }

    switch ( style ? style->cursor() : CURSOR_AUTO) {
    case CURSOR_AUTO:
        if ( d->mousePressed && m_part->hasSelection() )
	    // during selection, use an IBeam no matter what we're over
	    c = KCursor::ibeamCursor();
        else if ( (!mev.url.isNull() || isSubmitImage(mev.innerNode.handle()))
                  && m_part->settings()->changeCursor() )
            c = m_part->urlCursor();
        else if ( !mev.innerNode.isNull()
                  && (mev.innerNode.nodeType() == Node::TEXT_NODE
                      || mev.innerNode.nodeType() == Node::CDATA_SECTION_NODE
                      || (mev.innerNode.isContentEditable())))
            c = KCursor::ibeamCursor();
        break;
    case CURSOR_CROSS:
        c = KCursor::crossCursor();
        break;
    case CURSOR_POINTER:
        c = m_part->urlCursor();
        break;
    case CURSOR_MOVE:
        c = KCursor::sizeAllCursor();
        break;
    case CURSOR_E_RESIZE:
#if APPLE_CHANGES
        c = KCursor::eastResizeCursor();
        break;
#endif
    case CURSOR_W_RESIZE:
#if APPLE_CHANGES
        c = KCursor::westResizeCursor();
#else
        c = KCursor::sizeHorCursor();
#endif
        break;
    case CURSOR_N_RESIZE:
#if APPLE_CHANGES
        c = KCursor::northResizeCursor();
        break;
#endif
    case CURSOR_S_RESIZE:
#if APPLE_CHANGES
        c = KCursor::southResizeCursor();
#else
        c = KCursor::sizeVerCursor();
#endif
        break;
    case CURSOR_NE_RESIZE:
#if APPLE_CHANGES
        c = KCursor::northEastResizeCursor();
        break;
#endif
    case CURSOR_SW_RESIZE:
#if APPLE_CHANGES
        c = KCursor::southWestResizeCursor();
#else
        c = KCursor::sizeBDiagCursor();
#endif
        break;
    case CURSOR_NW_RESIZE:
#if APPLE_CHANGES
        c = KCursor::northWestResizeCursor();
        break;
#endif
    case CURSOR_SE_RESIZE:
#if APPLE_CHANGES
        c = KCursor::southEastResizeCursor();
#else
        c = KCursor::sizeFDiagCursor();
#endif
        break;
    case CURSOR_TEXT:
        c = KCursor::ibeamCursor();
        break;
    case CURSOR_WAIT:
        c = KCursor::waitCursor();
        break;
    case CURSOR_HELP:
        c = KCursor::whatsThisCursor();
        break;
    case CURSOR_DEFAULT:
        break;
    }

    QWidget *vp = viewport();
    if ( vp->cursor().handle() != c.handle() ) {
        if( c.handle() == KCursor::arrowCursor().handle())
            vp->unsetCursor();
        else
            vp->setCursor( c );
    }
    d->prevMouseX = xm;
    d->prevMouseY = ym;

    if (!swallowEvent) {
        khtml::MouseMoveEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
        QApplication::sendEvent( m_part, &event );
    }
}

void KHTMLView::resetCursor()
{
    viewport()->unsetCursor();
}

void KHTMLView::invalidateClick()
{
    d->clickCount = 0;
}

void KHTMLView::viewportMouseReleaseEvent( QMouseEvent * _mouse )
{
    if ( !m_part->xmlDocImpl() ) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    d->mousePressed = false;

    //kdDebug( 6000 ) << "\nmouseReleaseEvent: x=" << xm << ", y=" << ym << endl;

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MouseRelease );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );
#if APPLE_CHANGES
    if (KWQ(m_part)->passSubframeEventToSubframe(mev))
        return;
#endif

    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEUP_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,false,DOM::NodeImpl::MouseRelease);

    if (d->clickCount > 0
#if !APPLE_CHANGES
            && QPoint(d->clickX-xm,d->clickY-ym).manhattanLength() <= QApplication::startDragDistance()
#endif
        )
	dispatchMouseEvent(EventImpl::CLICK_EVENT,mev.innerNode.handle(),true,
			   d->clickCount,_mouse,true,DOM::NodeImpl::MouseRelease);

    if (!swallowEvent) {
	khtml::MouseReleaseEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event );
    }
}

void KHTMLView::keyPressEvent( QKeyEvent *_ke )
{
    if (m_part->xmlDocImpl() && m_part->xmlDocImpl()->focusNode()) {
        if (m_part->xmlDocImpl()->focusNode()->dispatchKeyEvent(_ke))
        {
            _ke->accept();
            return;
        }
    }

#if !APPLE_CHANGES
    int offs = (clipper()->height() < 30) ? clipper()->height() : 30;
    if (_ke->state()&ShiftButton)
      switch(_ke->key())
        {
        case Key_Space:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, -clipper()->height() - offs );
            break;
        }
    else
        switch ( _ke->key() )
        {
        case Key_Down:
        case Key_J:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, 10 );
            break;

        case Key_Space:
        case Key_Next:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, clipper()->height() - offs );
            break;

        case Key_Up:
        case Key_K:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, -10 );
            break;

        case Key_Prior:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, -clipper()->height() + offs );
            break;
        case Key_Right:
        case Key_L:
            if ( d->hmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 10, 0 );
            break;
        case Key_Left:
        case Key_H:
            if ( d->hmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( -10, 0 );
            break;
        case Key_Enter:
        case Key_Return:
	    // ### FIXME:
	    // or even better to HTMLAnchorElementImpl::event()
            if (m_part->xmlDocImpl()) {
		NodeImpl *n = m_part->xmlDocImpl()->focusNode();
		if (n)
		    n->setActive();
		d->originalNode = n;
	    }
            break;
        case Key_Home:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                setContentsPos( 0, 0 );
            break;
        case Key_End:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                setContentsPos( 0, contentsHeight() - visibleHeight() );
            break;
        default:
	    _ke->ignore();
            return;
        }
    _ke->accept();
#endif
}

void KHTMLView::keyReleaseEvent(QKeyEvent *_ke)
{
    if(m_part->xmlDocImpl() && m_part->xmlDocImpl()->focusNode()) {
        // Qt is damn buggy. we receive release events from our child
        // widgets. therefore, do not support keyrelease event on generic
        // nodes for now until we found  a workaround for the Qt bugs. (Dirk)
//         if (m_part->xmlDocImpl()->focusNode()->dispatchKeyEvent(_ke)) {
//             _ke->accept();
//             return;
//         }
//        QScrollView::keyReleaseEvent(_ke);
        Q_UNUSED(_ke);
    }
}

void KHTMLView::contentsContextMenuEvent ( QContextMenuEvent *_ce )
{
// ### what kind of c*** is that ?
#if 0
    if (!m_part->xmlDocImpl()) return;
    int xm = _ce->x();
    int ym = _ce->y();

    DOM::NodeImpl::MouseEvent mev( _ce->state(), DOM::NodeImpl::MouseMove ); // ### not a mouse event!
    m_part->xmlDocImpl()->prepareMouseEvent( xm, ym, &mev );

    NodeImpl *targetNode = mev.innerNode.handle();
    if (targetNode && targetNode->renderer() && targetNode->renderer()->isWidget()) {
        int absx = 0;
        int absy = 0;
        targetNode->renderer()->absolutePosition(absx,absy);
        QPoint pos(xm-absx,ym-absy);

        QWidget *w = static_cast<RenderWidget*>(targetNode->renderer())->widget();
        QContextMenuEvent cme(_ce->reason(),pos,_ce->globalPos(),_ce->state());
        setIgnoreEvents(true);
        QApplication::sendEvent(w,&cme);
        setIgnoreEvents(false);
    }
#endif
}

bool KHTMLView::dispatchDragEvent(int eventId, DOM::NodeImpl *dragTarget, const QPoint &loc, DOM::ClipboardImpl *clipboard)
{
    int clientX, clientY;
    viewportToContents(loc.x(), loc.y(), clientX, clientY);
    // Typically we'd use the mouse event's globalX/Y, but we have no mouse event to query, and in practice
    // the globalX/Y fields are the window level coords anyway.
    int screenX = loc.x();
    int screenY = loc.y();
    bool ctrlKey = 0;   // FIXME - set up modifiers, grab from AK or CG
    bool altKey = 0;
    bool shiftKey = 0;
    bool metaKey = 0;
    
    MouseEventImpl *me = new MouseEventImpl(static_cast<EventImpl::EventId>(eventId),
                                            true, true, m_part->xmlDocImpl()->defaultView(),
                                            0, screenX, screenY, clientX, clientY,
                                            ctrlKey, altKey, shiftKey, metaKey,
                                            0, 0, clipboard);
    me->ref();
    int exceptioncode = 0;
    dragTarget->dispatchEvent(me, exceptioncode, true);
    bool accept = me->defaultPrevented();
    me->deref();
    return accept;
}

bool KHTMLView::updateDragAndDrop(const QPoint &loc, DOM::ClipboardImpl *clipboard)
{
    bool accept = false;
    int xm, ym;
    viewportToContents(loc.x(), loc.y(), xm, ym);
    DOM::NodeImpl::MouseEvent mev(0, DOM::NodeImpl::MouseMove);
    m_part->xmlDocImpl()->prepareMouseEvent(true, xm, ym, &mev);
    DOM::Node newTarget = mev.innerNode;

    // Drag events should never go to text nodes (following IE, and proper mouseover/out dispatch)
    if (newTarget.nodeType() == Node::TEXT_NODE) {
        newTarget = newTarget.parentNode();
    }

    if (d->dragTarget != newTarget) {
        // note this ordering is explicitly chosen to match WinIE
        if (!newTarget.isNull()) {
            accept = dispatchDragEvent(EventImpl::DRAGENTER_EVENT, newTarget.handle(), loc, clipboard);
        }
        if (!d->dragTarget.isNull()) {
            dispatchDragEvent(EventImpl::DRAGLEAVE_EVENT, d->dragTarget.handle(), loc, clipboard);
        }
    } else if (!newTarget.isNull()) {
        accept = dispatchDragEvent(EventImpl::DRAGOVER_EVENT, newTarget.handle(), loc, clipboard);
    }
    d->dragTarget = newTarget;

    return accept;
}

void KHTMLView::cancelDragAndDrop(const QPoint &loc, DOM::ClipboardImpl *clipboard)
{
    if (!d->dragTarget.isNull()) {
        dispatchDragEvent(EventImpl::DRAGLEAVE_EVENT, d->dragTarget.handle(), loc, clipboard);
    }
    d->dragTarget = 0;
}

bool KHTMLView::performDragAndDrop(const QPoint &loc, DOM::ClipboardImpl *clipboard)
{
    bool accept = false;
    if (!d->dragTarget.isNull()) {
        accept = dispatchDragEvent(EventImpl::DROP_EVENT, d->dragTarget.handle(), loc, clipboard);
    }
    d->dragTarget = 0;
    return accept;
}

#if !APPLE_CHANGES

bool KHTMLView::focusNextPrevChild( bool next )
{
    // Now try to find the next child
    if (m_part->xmlDocImpl()) {
        focusNextPrevNode(next);
        if (m_part->xmlDocImpl()->focusNode() != 0)
            return true; // focus node found
    }

    // If we get here, there is no next/previous child to go to, so pass up to the next/previous child in our parent
    if (m_part->parentPart() && m_part->parentPart()->view()) {
        return m_part->parentPart()->view()->focusNextPrevChild(next);
    }

    return QWidget::focusNextPrevChild(next);
}

void KHTMLView::doAutoScroll()
{
    QPoint pos = QCursor::pos();
    pos = viewport()->mapFromGlobal( pos );

    int xm, ym;
    viewportToContents(pos.x(), pos.y(), xm, ym);

    pos = QPoint(pos.x() - viewport()->x(), pos.y() - viewport()->y());
    if ( (pos.y() < 0) || (pos.y() > visibleHeight()) ||
         (pos.x() < 0) || (pos.x() > visibleWidth()) )
    {
        ensureVisible( xm, ym, 0, 5 );
    }
}

#endif

DOM::NodeImpl *KHTMLView::nodeUnderMouse() const
{
    return d->underMouse;
}

bool KHTMLView::scrollTo(const QRect &bounds)
{
    d->scrollingSelf = true; // so scroll events get ignored

    int x, y, xe, ye;
    x = bounds.left();
    y = bounds.top();
    xe = bounds.right();
    ye = bounds.bottom();

    //kdDebug(6000)<<"scrolling coords: x="<<x<<" y="<<y<<" width="<<xe-x<<" height="<<ye-y<<endl;

    int deltax;
    int deltay;

    int curHeight = visibleHeight();
    int curWidth = visibleWidth();

    if (ye-y>curHeight-d->borderY)
	ye  = y + curHeight - d->borderY;

    if (xe-x>curWidth-d->borderX)
	xe = x + curWidth - d->borderX;

    // is xpos of target left of the view's border?
    if (x < contentsX() + d->borderX )
            deltax = x - contentsX() - d->borderX;
    // is xpos of target right of the view's right border?
    else if (xe + d->borderX > contentsX() + curWidth)
            deltax = xe + d->borderX - ( contentsX() + curWidth );
    else
        deltax = 0;

    // is ypos of target above upper border?
    if (y < contentsY() + d->borderY)
            deltay = y - contentsY() - d->borderY;
    // is ypos of target below lower border?
    else if (ye + d->borderY > contentsY() + curHeight)
            deltay = ye + d->borderY - ( contentsY() + curHeight );
    else
        deltay = 0;

    int maxx = curWidth-d->borderX;
    int maxy = curHeight-d->borderY;

    int scrollX,scrollY;

    scrollX = deltax > 0 ? (deltax > maxx ? maxx : deltax) : deltax == 0 ? 0 : (deltax>-maxx ? deltax : -maxx);
    scrollY = deltay > 0 ? (deltay > maxy ? maxy : deltay) : deltay == 0 ? 0 : (deltay>-maxy ? deltay : -maxy);

    if (contentsX() + scrollX < 0)
	scrollX = -contentsX();
    else if (contentsWidth() - visibleWidth() - contentsX() < scrollX)
	scrollX = contentsWidth() - visibleWidth() - contentsX();

    if (contentsY() + scrollY < 0)
	scrollY = -contentsY();
    else if (contentsHeight() - visibleHeight() - contentsY() < scrollY)
	scrollY = contentsHeight() - visibleHeight() - contentsY();

    scrollBy(scrollX, scrollY);



    // generate abs(scroll.)
    if (scrollX<0)
        scrollX=-scrollX;
    if (scrollY<0)
        scrollY=-scrollY;

    d->scrollingSelf = false;

    if ( (scrollX!=maxx) && (scrollY!=maxy) )
	return true;
    else return false;

}

void KHTMLView::focusNextPrevNode(bool next)
{
    // Sets the focus node of the document to be the node after (or if next is false, before) the current focus node.
    // Only nodes that are selectable (i.e. for which isSelectable() returns true) are taken into account, and the order
    // used is that specified in the HTML spec (see DocumentImpl::nextFocusNode() and DocumentImpl::previousFocusNode()
    // for details).

    DocumentImpl *doc = m_part->xmlDocImpl();
    NodeImpl *oldFocusNode = doc->focusNode();
    NodeImpl *newFocusNode;

    // Find the next/previous node from the current one
    if (next)
        newFocusNode = doc->nextFocusNode(oldFocusNode);
    else
        newFocusNode = doc->previousFocusNode(oldFocusNode);

    // If there was previously no focus node and the user has scrolled the document, then instead of picking the first
    // focusable node in the document, use the first one that lies within the visible area (if possible).
    if (!oldFocusNode && newFocusNode && d->scrollBarMoved) {

      kdDebug(6000) << " searching for visible link" << endl;

        bool visible = false;
        NodeImpl *toFocus = newFocusNode;
        while (!visible && toFocus) {
            QRect focusNodeRect = toFocus->getRect();
            if ((focusNodeRect.left() > contentsX()) && (focusNodeRect.right() < contentsX() + visibleWidth()) &&
                (focusNodeRect.top() > contentsY()) && (focusNodeRect.bottom() < contentsY() + visibleHeight())) {
                // toFocus is visible in the contents area
                visible = true;
            }
            else {
                // toFocus is _not_ visible in the contents area, pick the next node
                if (next)
                    toFocus = doc->nextFocusNode(toFocus);
                else
                    toFocus = doc->previousFocusNode(toFocus);
            }
        }

        if (toFocus)
            newFocusNode = toFocus;
    }

    d->scrollBarMoved = false;

    if (!newFocusNode)
      {
        // No new focus node, scroll to bottom or top depending on next
        if (next)
            scrollTo(QRect(contentsX()+visibleWidth()/2,contentsHeight(),0,0));
        else
            scrollTo(QRect(contentsX()+visibleWidth()/2,0,0,0));
    }
    else {
        // EDIT FIXME: if it's an editable element, activate the caret
        // otherwise, hide it
        if (newFocusNode->isContentEditable()) {
            // make caret visible
        } 
        else {
            // hide caret
        }

        // Scroll the view as necessary to ensure that the new focus node is visible
        if (oldFocusNode) {
            if (!scrollTo(newFocusNode->getRect()))
                return;
        }
        else {
            ensureVisible(contentsX(), next ? 0: contentsHeight());
        }
    }
    // Set focus node on the document
    m_part->xmlDocImpl()->setFocusNode(newFocusNode);
    emit m_part->nodeActivated(Node(newFocusNode));

#if 0
    if (newFocusNode) {

        // this does not belong here. it should run a query on the tree (Dirk)
        // I'll fix this very particular part of the code soon when I cleaned
        // up the positioning code
        // If the newly focussed node is a link, notify the part

        HTMLAnchorElementImpl *anchor = 0;
        if ((newFocusNode->id() == ID_A || newFocusNode->id() == ID_AREA))
            anchor = static_cast<HTMLAnchorElementImpl *>(newFocusNode);

        if (anchor && !anchor->areaHref().isNull())
            m_part->overURL(anchor->areaHref().string(), 0);
        else
            m_part->overURL(QString(), 0);
    }
#endif
}

void KHTMLView::setMediaType( const QString &medium )
{
    m_medium = medium;
}

QString KHTMLView::mediaType() const
{
#if APPLE_CHANGES
    // See if we have an override type.
    QString overrideType = KWQ(m_part)->overrideMediaType();
    if (!overrideType.isNull())
        return overrideType;
#endif
    return m_medium;
}

#if !APPLE_CHANGES

void KHTMLView::print()
{
    if(!m_part->xmlDocImpl()) return;
    khtml::RenderCanvas *root = static_cast<khtml::RenderCanvas *>(m_part->xmlDocImpl()->renderer());
    if(!root) return;

    // this only works on Unix - we assume 72dpi
    KPrinter *printer = new KPrinter(QPrinter::PrinterResolution);
    printer->addDialogPage(new KHTMLPrintSettings());
    if(printer->setup(this)) {
        viewport()->setCursor( waitCursor ); // only viewport(), no QApplication::, otherwise we get the busy cursor in kdeprint's dialogs
        // set up KPrinter
        printer->setFullPage(false);
        printer->setCreator("KDE 3.0 HTML Library");
        QString docname = m_part->xmlDocImpl()->URL();
        if ( !docname.isEmpty() )
	    printer->setDocName(docname);

        QPainter *p = new QPainter;
        p->begin( printer );
        khtml::setPrintPainter( p );

        m_part->xmlDocImpl()->setPaintDevice( printer );
        QString oldMediaType = mediaType();
        setMediaType( "print" );
        // We ignore margin settings for html and body when printing
        // and use the default margins from the print-system
        // (In Qt 3.0.x the default margins are hardcoded in Qt)
        m_part->xmlDocImpl()->setPrintStyleSheet( printer->option("kde-khtml-printfriendly") == "true" ?
                                                  "* { background-image: none !important;"
                                                  "    background-color: white !important;"
                                                  "    color: black !important; }"
                                                  "body { margin: 0px !important; }"
                                                  "html { margin: 0px !important; }" :
                                                  "body { margin: 0px !important; }"
                                                  "html { margin: 0px !important; }"
                                                  );


        QPaintDeviceMetrics metrics( printer );

        // this is a simple approximation... we layout the document
        // according to the width of the page, then just cut
        // pages without caring about the content. We should do better
        // in the future, but for the moment this is better than no
        // printing support
        kdDebug(6000) << "printing: physical page width = " << metrics.width()
                      << " height = " << metrics.height() << endl;
        root->setPrintingMode(true);
        root->setWidth(metrics.width());

        m_part->xmlDocImpl()->styleSelector()->computeFontSizes(&metrics);
        m_part->xmlDocImpl()->updateStyleSelector();
        root->setPrintImages( printer->option("kde-khtml-printimages") == "true");
        root->setNeedsLayoutAndMinMaxRecalc();
        root->layout();

        // ok. now print the pages.
        kdDebug(6000) << "printing: html page width = " << root->docWidth()
                      << " height = " << root->docHeight() << endl;
        kdDebug(6000) << "printing: margins left = " << printer->margins().width()
                      << " top = " << printer->margins().height() << endl;
        kdDebug(6000) << "printing: paper width = " << metrics.width()
                      << " height = " << metrics.height() << endl;
        // if the width is too large to fit on the paper we just scale
        // the whole thing.
        int pageHeight = metrics.height();
        int pageWidth = metrics.width();
        p->setClipRect(0,0, pageWidth, pageHeight);
        if(root->docWidth() > metrics.width()) {
            double scale = ((double) metrics.width())/((double) root->docWidth());
#ifndef QT_NO_TRANSFORMATIONS
            p->scale(scale, scale);
#endif
            pageHeight = (int) (pageHeight/scale);
            pageWidth = (int) (pageWidth/scale);
        }
        kdDebug(6000) << "printing: scaled html width = " << pageWidth
                      << " height = " << pageHeight << endl;
        int top = 0;
        while(top < root->docHeight()) {
            if(top > 0) printer->newPage();
            root->setTruncatedAt(top+pageHeight);

            root->print(p, 0, top, pageWidth, pageHeight, 0, 0);
            if (top + pageHeight >= root->docHeight())
                break; // Stop if we have printed everything

            p->translate(0, top - root->truncatedAt());
            top = root->truncatedAt();
        }

        p->end();
        delete p;

        // and now reset the layout to the usual one...
        root->setPrintingMode(false);
        khtml::setPrintPainter( 0 );
        setMediaType( oldMediaType );
        m_part->xmlDocImpl()->setPaintDevice( this );
        m_part->xmlDocImpl()->styleSelector()->computeFontSizes(m_part->xmlDocImpl()->paintDeviceMetrics());
        m_part->xmlDocImpl()->updateStyleSelector();
        viewport()->unsetCursor();
    }
    delete printer;
}

void KHTMLView::slotPaletteChanged()
{
    if(!m_part->xmlDocImpl()) return;
    DOM::DocumentImpl *document = m_part->xmlDocImpl();
    if (!document->isHTMLDocument()) return;
    khtml::RenderCanvas *root = static_cast<khtml::RenderCanvas *>(document->renderer());
    if(!root) return;
    root->style()->resetPalette();
    NodeImpl *body = static_cast<HTMLDocumentImpl*>(document)->body();
    if(!body) return;
    body->setChanged(true);
    body->recalcStyle( NodeImpl::Force );
}

void KHTMLView::paint(QPainter *p, const QRect &rc, int yOff, bool *more)
{
    if(!m_part->xmlDocImpl()) return;
    khtml::RenderCanvas *root = static_cast<khtml::RenderCanvas *>(m_part->xmlDocImpl()->renderer());
    if(!root) return;

    m_part->xmlDocImpl()->setPaintDevice(p->device());
    root->setPrintingMode(true);
    root->setWidth(rc.width());

    p->save();
    p->setClipRect(rc);
    p->translate(rc.left(), rc.top());
    double scale = ((double) rc.width()/(double) root->docWidth());
    int height = (int) ((double) rc.height() / scale);
#ifndef QT_NO_TRANSFORMATIONS
    p->scale(scale, scale);
#endif

    root->print(p, 0, yOff, root->docWidth(), height, 0, 0);
    if (more)
        *more = yOff + height < root->docHeight();
    p->restore();

    root->setPrintingMode(false);
    m_part->xmlDocImpl()->setPaintDevice( this );
}

#endif // !APPLE_CHANGES

void KHTMLView::useSlowRepaints()
{
    kdDebug(0) << "slow repaints requested" << endl;
    d->useSlowRepaints = true;
    setStaticBackground(true);
}

void KHTMLView::setScrollBarsMode ( ScrollBarMode mode )
{
#ifndef KHTML_NO_SCROLLBARS
    d->vmode = mode;
    d->hmode = mode;
    
#if APPLE_CHANGES
    QScrollView::setScrollBarsMode(mode);
#else
    QScrollView::setVScrollBarMode(mode);
    QScrollView::setHScrollBarMode(mode);
#endif
#else
    Q_UNUSED( mode );
#endif
}

void KHTMLView::setVScrollBarMode ( ScrollBarMode mode )
{
#ifndef KHTML_NO_SCROLLBARS
    d->vmode = mode;
    QScrollView::setVScrollBarMode(mode);
#else
    Q_UNUSED( mode );
#endif
}

void KHTMLView::setHScrollBarMode ( ScrollBarMode mode )
{
#ifndef KHTML_NO_SCROLLBARS
    d->hmode = mode;
    QScrollView::setHScrollBarMode(mode);
#else
    Q_UNUSED( mode );
#endif
}

void KHTMLView::restoreScrollBar ( )
{
#if APPLE_CHANGES
    suppressScrollBars(false);
#else
    int ow = visibleWidth();
    QScrollView::setVScrollBarMode(d->vmode);
    if (visibleWidth() != ow)
        layout();
    d->prevScrollbarVisible = verticalScrollBar()->isVisible();
#endif
}

#if !APPLE_CHANGES

QStringList KHTMLView::formCompletionItems(const QString &name) const
{
    if (!m_part->settings()->isFormCompletionEnabled())
        return QStringList();
    if (!d->formCompletions)
        d->formCompletions = new KSimpleConfig(locateLocal("data", "khtml/formcompletions"));
    return d->formCompletions->readListEntry(name);
}

void KHTMLView::addFormCompletionItem(const QString &name, const QString &value)
{
    if (!m_part->settings()->isFormCompletionEnabled())
        return;
    // don't store values that are all numbers or just numbers with
    // dashes or spaces as those are likely credit card numbers or
    // something similar
    bool cc_number(true);
    for (unsigned int i = 0; i < value.length(); ++i)
    {
      QChar c(value[i]);
      if (!c.isNumber() && c != '-' && !c.isSpace())
      {
        cc_number = false;
        break;
      }
    }
    if (cc_number)
      return;
    QStringList items = formCompletionItems(name);
    if (!items.contains(value))
        items.prepend(value);
    while ((int)items.count() > m_part->settings()->maxFormCompletionItems())
        items.remove(items.fromLast());
    d->formCompletions->writeEntry(name, items);
}

#endif

bool KHTMLView::dispatchMouseEvent(int eventId, DOM::NodeImpl *targetNode, bool cancelable,
				   int detail,QMouseEvent *_mouse, bool setUnder,
				   int mouseEventType)
{
    if (d->underMouse)
	d->underMouse->deref();
    d->underMouse = targetNode;
    if (d->underMouse)
	d->underMouse->ref();

    int exceptioncode = 0;
    int clientX, clientY;
    viewportToContents(_mouse->x(), _mouse->y(), clientX, clientY);
    int screenX = _mouse->globalX();
    int screenY = _mouse->globalY();
    int button = -1;
    switch (_mouse->button()) {
	case LeftButton:
	    button = 0;
	    break;
	case MidButton:
	    button = 1;
	    break;
	case RightButton:
	    button = 2;
	    break;
	default:
	    break;
    }
    bool ctrlKey = (_mouse->state() & ControlButton);
    bool altKey = (_mouse->state() & AltButton);
    bool shiftKey = (_mouse->state() & ShiftButton);
    bool metaKey = (_mouse->state() & MetaButton);

    // mouseout/mouseover
    if (setUnder && (d->prevMouseX != clientX || d->prevMouseY != clientY)) {

        // ### this code sucks. we should save the oldUnder instead of calculating
        // it again. calculating is expensive! (Dirk)
        NodeImpl *oldUnder = 0;
	if (d->prevMouseX >= 0 && d->prevMouseY >= 0) {
	    NodeImpl::MouseEvent mev( _mouse->stateAfter(), static_cast<NodeImpl::MouseEventType>(mouseEventType));
	    m_part->xmlDocImpl()->prepareMouseEvent( true, d->prevMouseX, d->prevMouseY, &mev );
	    oldUnder = mev.innerNode.handle();
	}
	if (oldUnder != targetNode) {
	    // send mouseout event to the old node
	    if (oldUnder){
		oldUnder->ref();
		MouseEventImpl *me = new MouseEventImpl(EventImpl::MOUSEOUT_EVENT,
							true,true,m_part->xmlDocImpl()->defaultView(),
							0,screenX,screenY,clientX,clientY,
							ctrlKey,altKey,shiftKey,metaKey,
							button,targetNode);
		me->ref();
		oldUnder->dispatchEvent(me,exceptioncode,true);
		me->deref();
	    }

	    // send mouseover event to the new node
	    if (targetNode) {
		MouseEventImpl *me = new MouseEventImpl(EventImpl::MOUSEOVER_EVENT,
							true,true,m_part->xmlDocImpl()->defaultView(),
							0,screenX,screenY,clientX,clientY,
							ctrlKey,altKey,shiftKey,metaKey,
							button,oldUnder);

		me->ref();
		targetNode->dispatchEvent(me,exceptioncode,true);
		me->deref();
	    }

            if (oldUnder)
                oldUnder->deref();
        }
    }

    bool swallowEvent = false;

    if (targetNode) {
	// send the actual event
	MouseEventImpl *me = new MouseEventImpl(static_cast<EventImpl::EventId>(eventId),
						true,cancelable,m_part->xmlDocImpl()->defaultView(),
						detail,screenX,screenY,clientX,clientY,
						ctrlKey,altKey,shiftKey,metaKey,
						button,0);
	me->ref();
	targetNode->dispatchEvent(me,exceptioncode,true);
	bool defaultHandled = me->defaultHandled();
        if (me->defaultHandled() || me->defaultPrevented())
            swallowEvent = true;
	me->deref();

	// Special case: If it's a click event, we also send the KHTML_CLICK or KHTML_DBLCLICK event. This is not part
	// of the DOM specs, but is used for compatibility with the traditional onclick="" and ondblclick="" attributes,
	// as there is no way to tell the difference between single & double clicks using DOM (only the click count is
	// stored, which is not necessarily the same)
	if (eventId == EventImpl::CLICK_EVENT) {
	    me = new MouseEventImpl(d->isDoubleClick ? EventImpl::KHTML_DBLCLICK_EVENT : EventImpl::KHTML_CLICK_EVENT,
				    true,cancelable,m_part->xmlDocImpl()->defaultView(),
				    detail,screenX,screenY,clientX,clientY,
				    ctrlKey,altKey,shiftKey,metaKey,
				    button,0);

	    me->ref();
	    if (defaultHandled)
		me->setDefaultHandled();
	    targetNode->dispatchEvent(me,exceptioncode,true);
            if (me->defaultHandled() || me->defaultPrevented())
                swallowEvent = true;
	    me->deref();
        }
        else if (eventId == EventImpl::MOUSEDOWN_EVENT) {
            // Focus should be shifted on mouse down, not on a click.  -dwh
            // Blur current focus node when a link/button is clicked; this
            // is expected by some sites that rely on onChange handlers running
            // from form fields before the button click is processed.
	    DOM::NodeImpl* nodeImpl = targetNode;
	    for ( ; nodeImpl && !nodeImpl->isFocusable(); nodeImpl = nodeImpl->parentNode());
            if (nodeImpl && nodeImpl->isMouseFocusable())
                swallowEvent = !m_part->xmlDocImpl()->setFocusNode(nodeImpl);
            else if (!nodeImpl || !nodeImpl->focused())
                swallowEvent = !m_part->xmlDocImpl()->setFocusNode(0);
        }
    }

    return swallowEvent;
}

void KHTMLView::setIgnoreWheelEvents( bool e )
{
    d->ignoreWheelEvents = e;
}

#ifndef QT_NO_WHEELEVENT

void KHTMLView::viewportWheelEvent(QWheelEvent* e)
{
#if !APPLE_CHANGES
    if ( d->ignoreWheelEvents && !verticalScrollBar()->isVisible() && m_part->parentPart() ) {
        if ( m_part->parentPart()->view() )
            m_part->parentPart()->view()->wheelEvent( e );
        e->ignore();
    }
    else if ( d->vmode == QScrollView::AlwaysOff ) {
        e->accept();
    }
    else {
        d->scrollBarMoved = true;
        QScrollView::viewportWheelEvent( e );
    }
#endif
}
#endif

#if !APPLE_CHANGES
void KHTMLView::dragEnterEvent( QDragEnterEvent* ev )
{
    // Handle drops onto frames (#16820)
    // Drops on the main html part is handled by Konqueror (and shouldn't do anything
    // in e.g. kmail, so not handled here).
    if ( m_part->parentPart() )
    {
        // Duplicated from KonqView::eventFilter
        if ( QUriDrag::canDecode( ev ) )
        {
            KURL::List lstDragURLs;
            bool ok = KURLDrag::decode( ev, lstDragURLs );
            QObjectList *children = this->queryList( "QWidget" );

            if ( ok &&
                 !lstDragURLs.first().url().contains( "javascript:", false ) && // ### this looks like a hack to me
                 ev->source() != this &&
                 children &&
                 children->findRef( ev->source() ) == -1 )
                ev->acceptAction();

            delete children;
        }
    }
    QScrollView::dragEnterEvent( ev );
}

void KHTMLView::dropEvent( QDropEvent *ev )
{
    // Handle drops onto frames (#16820)
    // Drops on the main html part is handled by Konqueror (and shouldn't do anything
    // in e.g. kmail, so not handled here).
    if ( m_part->parentPart() )
    {
        KURL::List lstDragURLs;
        bool ok = KURLDrag::decode( ev, lstDragURLs );

        KHTMLPart* part = m_part->parentPart();
        while ( part && part->parentPart() )
            part = part->parentPart();
        KParts::BrowserExtension *ext = part->browserExtension();
        if ( ok && ext && lstDragURLs.first().isValid() )
            emit ext->openURLRequest( lstDragURLs.first() );
    }
    QScrollView::dropEvent( ev );
}
#endif // !APPLE_CHANGES

void KHTMLView::focusInEvent( QFocusEvent *e )
{
    m_part->setSelectionVisible();
    QScrollView::focusInEvent( e );
}

void KHTMLView::focusOutEvent( QFocusEvent *e )
{
    m_part->stopAutoScroll();
    m_part->setSelectionVisible(false);
    QScrollView::focusOutEvent( e );
}

void KHTMLView::slotScrollBarMoved()
{
    if (!d->scrollingSelf)
        d->scrollBarMoved = true;
}

void KHTMLView::repaintRectangle(const QRect& r, bool immediate)
{
    updateContents(r, immediate);
}

void KHTMLView::timerEvent ( QTimerEvent *e )
{
    if (e->timerId()==d->layoutTimerId)
        layout();
}

void KHTMLView::scheduleRelayout()
{
    if (!d->layoutSchedulingEnabled)
        return;

    if (d->layoutTimerId || (m_part->xmlDocImpl() && !m_part->xmlDocImpl()->shouldScheduleLayout()))
        return;

    d->allDataReceivedWhenTimerSet = m_part->xmlDocImpl() && !m_part->xmlDocImpl()->allDataReceived();

#ifdef INSTRUMENT_LAYOUT_SCHEDULING
    if (!m_part->xmlDocImpl()->ownerElement())
        printf("Scheduling layout for %d\n", m_part->xmlDocImpl()->minimumLayoutDelay());
#endif

    d->layoutTimerId = startTimer(m_part->xmlDocImpl() ? m_part->xmlDocImpl()->minimumLayoutDelay() : 0);
}

bool KHTMLView::haveDelayedLayoutScheduled()
{
    return d->layoutTimerId && d->allDataReceivedWhenTimerSet;
}

void KHTMLView::unscheduleRelayout()
{
    if (!d->layoutTimerId)
        return;

    killTimer(d->layoutTimerId);
    d->layoutTimerId = 0;
    d->allDataReceivedWhenTimerSet = false;
}

bool KHTMLView::isTransparent() const
{
    return d->isTransparent;
}

void KHTMLView::setTransparent(bool isTransparent)
{
    d->isTransparent = isTransparent;
}

