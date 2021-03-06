/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 2001 Peter Kelly (pmk@post.com)
 * (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
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
 */

#include "dom/dom2_views.h"

#include "xml/dom2_eventsimpl.h"
#include "xml/dom_stringimpl.h"
#include "xml/dom_nodeimpl.h"
#include "xml/dom_docimpl.h"
#include "rendering/render_object.h"
#include "rendering/render_layer.h"

#include <kdebug.h>

using namespace DOM;

EventImpl::EventImpl()
{
    m_type = 0;
    m_canBubble = false;
    m_cancelable = false;

    m_propagationStopped = false;
    m_defaultPrevented = false;
    m_cancelBubble = false;
    m_id = UNKNOWN_EVENT;
    m_currentTarget = 0;
    m_eventPhase = 0;
    m_target = 0;
    m_createTime = QDateTime::currentDateTime();
    m_defaultHandled = false;
}

EventImpl::EventImpl(EventId _id, bool canBubbleArg, bool cancelableArg)
{
    DOMString t = EventImpl::idToType(_id);
    m_type = t.implementation();
    if (m_type)
	m_type->ref();
    m_canBubble = canBubbleArg;
    m_cancelable = cancelableArg;

    m_propagationStopped = false;
    m_defaultPrevented = false;
    m_cancelBubble = false;
    m_id = _id;
    m_currentTarget = 0;
    m_eventPhase = 0;
    m_target = 0;
    m_createTime = QDateTime::currentDateTime();
    m_defaultHandled = false;
}

EventImpl::~EventImpl()
{
    if (m_type)
        m_type->deref();
    if (m_target)
        m_target->deref();
}

DOMString EventImpl::type() const
{
    return m_type;
}

NodeImpl *EventImpl::target() const
{
    return m_target;
}

void EventImpl::setTarget(NodeImpl *_target)
{
    if (m_target)
        m_target->deref();
    m_target = _target;
    if (m_target)
        m_target->ref();
}

NodeImpl *EventImpl::currentTarget() const
{
    return m_currentTarget;
}

void EventImpl::setCurrentTarget(NodeImpl *_currentTarget)
{
    m_currentTarget = _currentTarget;
}

unsigned short EventImpl::eventPhase() const
{
    return m_eventPhase;
}

void EventImpl::setEventPhase(unsigned short _eventPhase)
{
    m_eventPhase = _eventPhase;
}

bool EventImpl::bubbles() const
{
    return m_canBubble;
}

bool EventImpl::cancelable() const
{
    return m_cancelable;
}

DOMTimeStamp EventImpl::timeStamp()
{
    QDateTime epoch(QDate(1970,1,1),QTime(0,0));
    // ### kjs does not yet support long long (?) so the value wraps around
    return epoch.secsTo(m_createTime)*1000+m_createTime.time().msec();
}

void EventImpl::stopPropagation()
{
    m_propagationStopped = true;
}

void EventImpl::preventDefault()
{
    if (m_cancelable)
	m_defaultPrevented = true;
}

void EventImpl::initEvent(const DOMString &eventTypeArg, bool canBubbleArg, bool cancelableArg)
{
    // ### ensure this is not called after we have been dispatched (also for subclasses)

    if (m_type)
	m_type->deref();

    m_type = eventTypeArg.implementation();
    if (m_type)
	m_type->ref();

    m_id = typeToId(eventTypeArg);

    m_canBubble = canBubbleArg;
    m_cancelable = cancelableArg;
}

EventImpl::EventId EventImpl::typeToId(DOMString type)
{
    if (type == "DOMFocusIn")
	return DOMFOCUSIN_EVENT;
    else if (type == "DOMFocusOut")
	return DOMFOCUSOUT_EVENT;
    else if (type == "DOMActivate")
	return DOMACTIVATE_EVENT;
    else if (type == "click")
	return CLICK_EVENT;
    else if (type == "mousedown")
	return MOUSEDOWN_EVENT;
    else if (type == "mouseup")
	return MOUSEUP_EVENT;
    else if (type == "mouseover")
	return MOUSEOVER_EVENT;
    else if (type == "mousemove")
	return MOUSEMOVE_EVENT;
    else if (type == "mouseout")
	return MOUSEOUT_EVENT;
    else if (type == "onbeforecut")
	return BEFORECUT_EVENT;
    else if (type == "oncut")
	return CUT_EVENT;
    else if (type == "onbeforecopy")
	return BEFORECOPY_EVENT;
    else if (type == "oncopy")
	return COPY_EVENT;
    else if (type == "onbeforepaste")
	return BEFOREPASTE_EVENT;
    else if (type == "onpaste")
	return PASTE_EVENT;
    else if (type == "dragenter")
	return DRAGENTER_EVENT;
    else if (type == "dragover")
	return DRAGOVER_EVENT;
    else if (type == "dragleave")
	return DRAGLEAVE_EVENT;
    else if (type == "drop")
	return DROP_EVENT;
    else if (type == "dragstart")
	return DRAGSTART_EVENT;
    else if (type == "drag")
	return DRAG_EVENT;
    else if (type == "dragend")
	return DRAGEND_EVENT;
    else if (type == "selectstart")
	return SELECTSTART_EVENT;
    else if (type == "DOMSubtreeModified")
	return DOMSUBTREEMODIFIED_EVENT;
    else if (type == "DOMNodeInserted")
	return DOMNODEINSERTED_EVENT;
    else if (type == "DOMNodeRemoved")
	return DOMNODEREMOVED_EVENT;
    else if (type == "DOMNodeRemovedFromDocument")
	return DOMNODEREMOVEDFROMDOCUMENT_EVENT;
    else if (type == "DOMNodeInsertedIntoDocument")
	return DOMNODEINSERTEDINTODOCUMENT_EVENT;
    else if (type == "DOMAttrModified")
	return DOMATTRMODIFIED_EVENT;
    else if (type == "DOMCharacterDataModified")
	return DOMCHARACTERDATAMODIFIED_EVENT;
    else if (type == "load")
	return LOAD_EVENT;
    else if (type == "unload")
	return UNLOAD_EVENT;
    else if (type == "abort")
	return ABORT_EVENT;
    else if (type == "error")
	return ERROR_EVENT;
    else if (type == "select")
	return SELECT_EVENT;
    else if (type == "change")
	return CHANGE_EVENT;
    else if (type == "submit")
	return SUBMIT_EVENT;
    else if (type == "reset")
	return RESET_EVENT;
    else if (type == "focus")
	return FOCUS_EVENT;
    else if (type == "blur")
	return BLUR_EVENT;
    else if (type == "resize")
	return RESIZE_EVENT;
    else if (type == "scroll")
	return SCROLL_EVENT;
    else if (type == "contextmenu")
	return CONTEXTMENU_EVENT;
    else if (type == "keydown")
	return KEYDOWN_EVENT;
    else if (type == "keyup")
	return KEYUP_EVENT;
#if APPLE_CHANGES
    else if (type == "search")
	return SEARCH_EVENT;
#endif
    else if (type == "input")
        return INPUT_EVENT;
    else if (type == "textInput")
	return TEXTINPUT_EVENT;
    else if (type == "readystatechange")
	return KHTML_READYSTATECHANGE_EVENT;
    // ignore: KHTML_DBLCLICK_EVENT
    // ignore: KHTML_CLICK_EVENT
    return UNKNOWN_EVENT;
}

DOMString EventImpl::idToType(EventImpl::EventId id)
{
    switch (id) {
	case DOMFOCUSIN_EVENT:
	    return "DOMFocusIn";
	case DOMFOCUSOUT_EVENT:
	    return "DOMFocusOut";
	case DOMACTIVATE_EVENT:
	    return "DOMActivate";
	case CLICK_EVENT:
	    return "click";
	case MOUSEDOWN_EVENT:
	    return "mousedown";
	case MOUSEUP_EVENT:
	    return "mouseup";
	case MOUSEOVER_EVENT:
	    return "mouseover";
	case MOUSEMOVE_EVENT:
	    return "mousemove";
	case MOUSEOUT_EVENT:
	    return "mouseout";
        case BEFORECUT_EVENT:
            return "onbeforecut";
	case CUT_EVENT:
            return "oncut";
	case BEFORECOPY_EVENT:
            return "onbeforecopy";
	case COPY_EVENT:
            return "oncopy";
	case BEFOREPASTE_EVENT:
            return "onbeforepaste";
	case PASTE_EVENT:
            return "onpaste";
	case DRAGENTER_EVENT:
            return "dragenter";
	case DRAGOVER_EVENT:
            return "dragover";
	case DRAGLEAVE_EVENT:
            return "dragleave";
	case DROP_EVENT:
	    return "drop";
	case DRAGSTART_EVENT:
	    return "dragstart";
	case DRAG_EVENT:
	    return "drag";
	case DRAGEND_EVENT:
	    return "dragend";
	case SELECTSTART_EVENT:
	    return "selectstart";
	case DOMSUBTREEMODIFIED_EVENT:
	    return "DOMSubtreeModified";
	case DOMNODEINSERTED_EVENT:
	    return "DOMNodeInserted";
	case DOMNODEREMOVED_EVENT:
	    return "DOMNodeRemoved";
	case DOMNODEREMOVEDFROMDOCUMENT_EVENT:
	    return "DOMNodeRemovedFromDocument";
	case DOMNODEINSERTEDINTODOCUMENT_EVENT:
	    return "DOMNodeInsertedIntoDocument";
	case DOMATTRMODIFIED_EVENT:
	    return "DOMAttrModified";
	case DOMCHARACTERDATAMODIFIED_EVENT:
	    return "DOMCharacterDataModified";
	case LOAD_EVENT:
	    return "load";
	case UNLOAD_EVENT:
	    return "unload";
	case ABORT_EVENT:
	    return "abort";
	case ERROR_EVENT:
	    return "error";
	case SELECT_EVENT:
	    return "select";
	case CHANGE_EVENT:
	    return "change";
	case SUBMIT_EVENT:
	    return "submit";
	case RESET_EVENT:
	    return "reset";
	case FOCUS_EVENT:
	    return "focus";
	case BLUR_EVENT:
	    return "blur";
	case RESIZE_EVENT:
	    return "resize";
	case SCROLL_EVENT:
	    return "scroll";
        case CONTEXTMENU_EVENT:
            return "contextmenu";
	case KEYDOWN_EVENT:
            return "keydown";
	case KEYUP_EVENT:
            return "keyup";
        case KEYPRESS_EVENT:
            return "keypress";
	case TEXTINPUT_EVENT:
            return "textInput";
#if APPLE_CHANGES
        case SEARCH_EVENT:
            return "search";
#endif
        case INPUT_EVENT:
            return "input";
	// khtml extensions
	case KHTML_DBLCLICK_EVENT:
            return "dblclick";
	case KHTML_CLICK_EVENT:
            return "click";
	case KHTML_DRAGDROP_EVENT:
            return "khtml_dragdrop";
	case KHTML_ERROR_EVENT:
            return "khtml_error";
	case KHTML_MOVE_EVENT:
            return "khtml_move";
        case KHTML_ORIGCLICK_MOUSEUP_EVENT:
            return "khtml_origclick_mouseup_event";
        case KHTML_READYSTATECHANGE_EVENT:
	    return "readystatechange";
	default:
	    return DOMString();
	    break;
    }
}

bool EventImpl::isUIEvent() const
{
    return false;
}

bool EventImpl::isMouseEvent() const
{
    return false;
}

bool EventImpl::isMutationEvent() const
{
    return false;
}

bool EventImpl::isKeyboardEvent() const
{
    return false;
}

bool EventImpl::isDragEvent() const
{
    return false;
}

bool EventImpl::isClipboardEvent() const
{
    return false;
}

// -----------------------------------------------------------------------------

UIEventImpl::UIEventImpl()
{
    m_view = 0;
    m_detail = 0;
}

UIEventImpl::UIEventImpl(EventId _id, bool canBubbleArg, bool cancelableArg,
		AbstractViewImpl *viewArg, long detailArg)
		: EventImpl(_id,canBubbleArg,cancelableArg)
{
    m_view = viewArg;
    if (m_view)
        m_view->ref();
    m_detail = detailArg;
}

UIEventImpl::~UIEventImpl()
{
    if (m_view)
        m_view->deref();
}

void UIEventImpl::initUIEvent(const DOMString &typeArg,
			      bool canBubbleArg,
			      bool cancelableArg,
			      const AbstractView &viewArg,
			      long detailArg)
{
    EventImpl::initEvent(typeArg,canBubbleArg,cancelableArg);

    if (m_view)
	m_view->deref();

    m_view = viewArg.handle();
    if (m_view)
	m_view->ref();
    m_detail = detailArg;
}

bool UIEventImpl::isUIEvent() const
{
    return true;
}

// -----------------------------------------------------------------------------

MouseEventImpl::MouseEventImpl()
{
    m_screenX = 0;
    m_screenY = 0;
    m_clientX = 0;
    m_clientY = 0;
    m_ctrlKey = false;
    m_altKey = false;
    m_shiftKey = false;
    m_metaKey = false;
    m_button = 0;
    m_relatedTarget = 0;
    m_clipboard = 0;
}

MouseEventImpl::MouseEventImpl(EventId _id,
			       bool canBubbleArg,
			       bool cancelableArg,
			       AbstractViewImpl *viewArg,
			       long detailArg,
			       long screenXArg,
			       long screenYArg,
			       long clientXArg,
			       long clientYArg,
			       bool ctrlKeyArg,
			       bool altKeyArg,
			       bool shiftKeyArg,
			       bool metaKeyArg,
			       unsigned short buttonArg,
			       NodeImpl *relatedTargetArg,
                               ClipboardImpl *clipboardArg)
		   : UIEventImpl(_id,canBubbleArg,cancelableArg,viewArg,detailArg)
{
    m_screenX = screenXArg;
    m_screenY = screenYArg;
    m_clientX = clientXArg;
    m_clientY = clientYArg;
    m_ctrlKey = ctrlKeyArg;
    m_altKey = altKeyArg;
    m_shiftKey = shiftKeyArg;
    m_metaKey = metaKeyArg;
    m_button = buttonArg;
    m_relatedTarget = relatedTargetArg;
    if (m_relatedTarget)
	m_relatedTarget->ref();
    m_clipboard = clipboardArg;
    if (m_clipboard)
	m_clipboard->ref();
    computeLayerPos();
}

void MouseEventImpl::computeLayerPos()
{
    m_layerX = m_clientX;
    m_layerY = m_clientY;

    DocumentImpl *doc = view()->document();

    if (!doc) {
	return;
    }

    khtml::RenderObject::NodeInfo renderInfo(true, false);
    doc->renderer()->layer()->nodeAtPoint(renderInfo, m_clientX, m_clientY);

    NodeImpl *node = renderInfo.innerNonSharedNode();
    while (node && !node->renderer()) {
	node = node->parent();
    }

    if (!node) {
	return;
    }

    node->renderer()->enclosingLayer()->updateLayerPosition();
    
    for (khtml::RenderLayer *layer = node->renderer()->enclosingLayer(); layer != NULL; layer = layer->parent()) {
	m_layerX -= layer->xPos();
	m_layerY -= layer->yPos();
    }
}

MouseEventImpl::~MouseEventImpl()
{
    if (m_relatedTarget)
	m_relatedTarget->deref();
    if (m_clipboard)
	m_clipboard->deref();
}

void MouseEventImpl::initMouseEvent(const DOMString &typeArg,
                                    bool canBubbleArg,
                                    bool cancelableArg,
                                    const AbstractView &viewArg,
                                    long detailArg,
                                    long screenXArg,
                                    long screenYArg,
                                    long clientXArg,
                                    long clientYArg,
                                    bool ctrlKeyArg,
                                    bool altKeyArg,
                                    bool shiftKeyArg,
                                    bool metaKeyArg,
                                    unsigned short buttonArg,
                                    const Node &relatedTargetArg)
{
    UIEventImpl::initUIEvent(typeArg,canBubbleArg,cancelableArg,viewArg,detailArg);

    if (m_relatedTarget)
	m_relatedTarget->deref();

    m_screenX = screenXArg;
    m_screenY = screenYArg;
    m_clientX = clientXArg;
    m_clientY = clientYArg;
    m_ctrlKey = ctrlKeyArg;
    m_altKey = altKeyArg;
    m_shiftKey = shiftKeyArg;
    m_metaKey = metaKeyArg;
    m_button = buttonArg;
    m_relatedTarget = relatedTargetArg.handle();
    if (m_relatedTarget)
	m_relatedTarget->ref();
    computeLayerPos();
}

bool MouseEventImpl::isMouseEvent() const
{
    return true;
}

bool MouseEventImpl::isDragEvent() const
{
    return (m_id == EventImpl::DRAGENTER_EVENT || m_id == EventImpl::DRAGOVER_EVENT
            || m_id == EventImpl::DRAGLEAVE_EVENT || m_id == EventImpl::DROP_EVENT 
            || m_id == EventImpl::DRAGSTART_EVENT || m_id == EventImpl::DRAG_EVENT
            || m_id == EventImpl::DRAGEND_EVENT);
}

//---------------------------------------------------------------------------------------------

KeyboardEventImpl::KeyboardEventImpl()
{
  m_keyEvent = 0;
  m_keyIdentifier = 0;
  m_keyLocation = KeyboardEvent::DOM_KEY_LOCATION_STANDARD;
  m_ctrlKey = false;
  m_shiftKey = false;
  m_altKey = false;
  m_metaKey = false;
  m_altGraphKey = false;
}

KeyboardEventImpl::KeyboardEventImpl(QKeyEvent *key, AbstractViewImpl *view)
  : UIEventImpl(key->type() == QEvent::KeyRelease ? KEYUP_EVENT : key->isAutoRepeat() ? KEYPRESS_EVENT : KEYDOWN_EVENT,
                true,true,view,0)
{
#if APPLE_CHANGES
    m_keyEvent = new QKeyEvent(*key);
#else
    m_keyEvent = new QKeyEvent(key->type(), key->key(), key->ascii(), key->state(), key->text(), key->isAutoRepeat(), key->count());
#endif

#if APPLE_CHANGES
    DOMString identifier(key->keyIdentifier());
    m_keyIdentifier = identifier.implementation();
    m_keyIdentifier->ref();
#else
    m_keyIdentifier = 0;
    // need the equivalent of the above for KDE
#endif

    int keyState = key->state();

    m_ctrlKey = keyState & Qt::ControlButton;
    m_shiftKey = keyState & Qt::ShiftButton;
    m_altKey = keyState & Qt::AltButton;
    m_metaKey = keyState & Qt::MetaButton;
    m_altGraphKey = false; // altGraphKey is not supported by Qt.
    
    // Note: we only support testing for num pad
    m_keyLocation = (keyState & Qt::Keypad) ? KeyboardEvent::DOM_KEY_LOCATION_NUMPAD : KeyboardEvent::DOM_KEY_LOCATION_STANDARD;
}

KeyboardEventImpl::KeyboardEventImpl(EventId _id,
                                        bool canBubbleArg,
                                        bool cancelableArg,
                                        AbstractViewImpl *viewArg, 
                                        const DOMString &keyIdentifierArg, 
                                        unsigned long keyLocationArg, 
                                        bool ctrlKeyArg, 
                                        bool shiftKeyArg, 
                                        bool altKeyArg, 
                                        bool metaKeyArg, 
                                        bool altGraphKeyArg)
  : UIEventImpl(_id,canBubbleArg,cancelableArg,viewArg,0)
{
    m_keyEvent = 0;
    m_keyIdentifier = keyIdentifierArg.implementation();
    if (m_keyIdentifier)
        m_keyIdentifier->ref();
    m_keyLocation = keyLocationArg;
    m_ctrlKey = ctrlKeyArg;
    m_shiftKey = shiftKeyArg;
    m_altKey = altKeyArg;
    m_metaKey = metaKeyArg;
    m_altGraphKey = altGraphKeyArg;
}

KeyboardEventImpl::~KeyboardEventImpl()
{
    delete m_keyEvent;
    if (m_keyIdentifier)
        m_keyIdentifier->deref();
}

void KeyboardEventImpl::initKeyboardEvent(const DOMString &typeArg,
                        bool canBubbleArg,
                        bool cancelableArg,
                        const AbstractView &viewArg, 
                        const DOMString &keyIdentifierArg, 
                        unsigned long keyLocationArg, 
                        bool ctrlKeyArg, 
                        bool shiftKeyArg, 
                        bool altKeyArg, 
                        bool metaKeyArg, 
                        bool altGraphKeyArg)
{
    if (m_keyIdentifier)
        m_keyIdentifier->deref();

    UIEventImpl::initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, 0);
    m_keyIdentifier = keyIdentifierArg.implementation();
    if (m_keyIdentifier)
        m_keyIdentifier->ref();
    m_keyLocation = keyLocationArg;
    m_ctrlKey = ctrlKeyArg;
    m_shiftKey = shiftKeyArg;
    m_altKey = altKeyArg;
    m_metaKey = metaKeyArg;
    m_altGraphKey = altGraphKeyArg;
}

int KeyboardEventImpl::keyCode() const
{
    if (!m_keyEvent) {
        return 0;
    }
    switch (m_id) {
        case KEYDOWN_EVENT:
        case KEYUP_EVENT:
#if APPLE_CHANGES
            return m_keyEvent->WindowsKeyCode();
#else
            // need the equivalent of the above for KDE
#endif
        default:
            return charCode();
    }
}

int KeyboardEventImpl::charCode() const
{
    if (!m_keyEvent) {
        return 0;
    }
    QString text = m_keyEvent->text();
    if (text.length() != 1) {
        return 0;
    }
    return text[0].unicode();
}

bool KeyboardEventImpl::isKeyboardEvent() const
{
    return true;
}

// -----------------------------------------------------------------------------

MutationEventImpl::MutationEventImpl()
{
    m_relatedNode = 0;
    m_prevValue = 0;
    m_newValue = 0;
    m_attrName = 0;
    m_attrChange = 0;
}

MutationEventImpl::MutationEventImpl(EventId _id,
				     bool canBubbleArg,
				     bool cancelableArg,
				     const Node &relatedNodeArg,
				     const DOMString &prevValueArg,
				     const DOMString &newValueArg,
				     const DOMString &attrNameArg,
				     unsigned short attrChangeArg)
		      : EventImpl(_id,canBubbleArg,cancelableArg)
{
    m_relatedNode = relatedNodeArg.handle();
    if (m_relatedNode)
	m_relatedNode->ref();
    m_prevValue = prevValueArg.implementation();
    if (m_prevValue)
	m_prevValue->ref();
    m_newValue = newValueArg.implementation();
    if (m_newValue)
	m_newValue->ref();
    m_attrName = attrNameArg.implementation();
    if (m_newValue)
	m_newValue->ref();
    m_attrChange = attrChangeArg;
}

MutationEventImpl::~MutationEventImpl()
{
    if (m_relatedNode)
	m_relatedNode->deref();
    if (m_prevValue)
	m_prevValue->deref();
    if (m_newValue)
	m_newValue->deref();
    if (m_attrName)
	m_attrName->deref();
}

void MutationEventImpl::initMutationEvent(const DOMString &typeArg,
					  bool canBubbleArg,
					  bool cancelableArg,
					  const Node &relatedNodeArg,
					  const DOMString &prevValueArg,
					  const DOMString &newValueArg,
					  const DOMString &attrNameArg,
					  unsigned short attrChangeArg)
{
    EventImpl::initEvent(typeArg,canBubbleArg,cancelableArg);

    if (m_relatedNode)
	m_relatedNode->deref();
    if (m_prevValue)
	m_prevValue->deref();
    if (m_newValue)
	m_newValue->deref();
    if (m_attrName)
	m_attrName->deref();

    m_relatedNode = relatedNodeArg.handle();
    if (m_relatedNode)
	m_relatedNode->ref();
    m_prevValue = prevValueArg.implementation();
    if (m_prevValue)
	m_prevValue->ref();
    m_newValue = newValueArg.implementation();
    if (m_newValue)
	m_newValue->ref();
    m_attrName = attrNameArg.implementation();
    if (m_newValue)
	m_newValue->ref();
    m_attrChange = attrChangeArg;
}

bool MutationEventImpl::isMutationEvent() const
{
    return true;
}

// -----------------------------------------------------------------------------

ClipboardEventImpl::ClipboardEventImpl()
{
    m_clipboard = 0;
}

ClipboardEventImpl::ClipboardEventImpl(EventId _id, bool canBubbleArg, bool cancelableArg, ClipboardImpl *clipboardArg)
  : EventImpl(_id, canBubbleArg, cancelableArg), m_clipboard(clipboardArg)
{
      if (m_clipboard)
          m_clipboard->ref();
}

ClipboardEventImpl::~ClipboardEventImpl()
{
    if (m_clipboard)
        m_clipboard->deref();
}

bool ClipboardEventImpl::isClipboardEvent() const
{
    return true;
}

// -----------------------------------------------------------------------------

RegisteredEventListener::RegisteredEventListener(EventImpl::EventId _id, EventListener *_listener, bool _useCapture)
{
    id = _id;
    listener = _listener;
    useCapture = _useCapture;
    listener->ref();
}

RegisteredEventListener::~RegisteredEventListener() {
    listener->deref();
}

bool RegisteredEventListener::operator==(const RegisteredEventListener &other)
{
    return (id == other.id &&
	    listener == other.listener &&
	    useCapture == other.useCapture);
}

// -----------------------------------------------------------------------------

ClipboardImpl::ClipboardImpl()
{
}

ClipboardImpl::~ClipboardImpl()
{
}
