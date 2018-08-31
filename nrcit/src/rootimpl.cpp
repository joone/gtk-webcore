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
#include "rootimpl.h"
#include "frameimpl.h"
#include "osbimpl.h"
#include "prefimpl.h"

#include "NRCore/WebCoreSettings.h"

class InternalHttpProtoDelegate  : public OSB::ProtocolDelegate
{
public:
    void setProxy(const gchar* proxy) {
	HttpFactory().setProxy("http://", proxy);
    }

    bool provides(const gchar* protocol) {
	return HttpFactory().canProvide(protocol);
    }
};

static
void setDefaultWebCoreSettings(WebCoreSettings* s)
{
    s->setSerifFontFamily("serif");
    s->setSansSerifFontFamily("sans");
    s->setFixedFontFamily("fixed");
    s->setStandardFontFamily("times");
    s->setCursiveFontFamily("times");
    s->setFantasyFontFamily("times");
    s->setWillLoadImagesAutomatically(true);
    s->setJavaScriptEnabled(true);
    s->setJavaEnabled(false);
    s->setPluginsEnabled(false);
    s->setMinimumFontSize(6.0f);
    s->setMinimumLogicalFontSize(6.0f);
    s->setDefaultFontSize(14.0f);
    s->setDefaultFixedFontSize(10.0f);	
}

struct FrameGroup
{
    FrameGroup(const gchar* n):mainFrames(0), name(g_strdup(n)) { setDefaultWebCoreSettings(&settings); }

    void insert(BridgeImpl* mainFrame) { mainFrames = g_list_append(mainFrames, mainFrame);}
    void remove(BridgeImpl* mainFrame) { mainFrames = g_list_remove(mainFrames, mainFrame);}
    bool isEmpty() {return g_list_length(mainFrames);}

    GList* mainFrames; // of BridgeImpl*
    gchar* name;
    URLCredentialStorageImpl credentials;
    WebCoreSettings settings;
};

extern"C"{
void dict_key_destr(gpointer data)
{
    g_free((gchar*)data);
}
void dict_value_destr(gpointer data)
{
    FrameGroup* fg = static_cast<FrameGroup*>(data);
    delete fg;
}
}

class FrameGroupHolder {
    GHashTable* dict; // of FrameGroup*
public:

    FrameGroupHolder() 
    {
	dict = g_hash_table_new_full(g_str_hash, g_str_equal,dict_key_destr, dict_value_destr);
    }

    FrameGroup* groupForName(const gchar* name)
    {
	FrameGroup *f = static_cast<FrameGroup*>(g_hash_table_lookup(dict,name));
	if (!f) {
	    f = new FrameGroup(name);
	    g_hash_table_insert(dict, g_strdup(name), f);
	}
	return f;
    }

    void registerTo(FrameGroup* grp, BridgeImpl* target) 
    {
	grp->insert(target);
    }

    void unregisterFrom(FrameGroup* grp, BridgeImpl *target)
    {
	grp->remove(target);
	if (grp->isEmpty()) 
	    g_hash_table_remove(dict, grp->name); // this deletes grp
    }
};

FrameGroupHolder& groups()
{
    static FrameGroupHolder single;
    return single;
}



RootImpl::RootImpl(OSB::Root* aiface, const gchar* frameName, const gchar* groupName)
    :_iface(aiface),_textSizeMultiplier(1.0)
{
    if (!groupName) groupName = "";
    _currentGroup = groups().groupForName(groupName);

    // create mainFrame
    _mainFrame = new FrameImpl(this, 0);
    groups().registerTo(_currentGroup, this->_mainFrame);
    if (frameName) _mainFrame->setFrameName(frameName);
    _mainFrame->loadEmptyDocumentSynchronously();
}

RootImpl::~RootImpl()
{
    groups().unregisterFrom(_currentGroup, this->_mainFrame);

    delete _mainFrame;
}

void RootImpl::setGroup(const gchar* group)
{
    groups().unregisterFrom(_currentGroup, _mainFrame);
    _currentGroup = groups().groupForName(group);
    groups().registerTo(_currentGroup, _mainFrame);
    _mainFrame->changeSettingsDescendingToChildren(&_currentGroup->settings);
}

const gchar* RootImpl::group() 
{
    return _currentGroup->name;
}

void RootImpl::setFrameLoadDelegate(OSB::FrameLoadDelegate* d)
{
    _mainFrame->setFrameLoadDelegate(d);
}

void RootImpl::setPreferences(const OSB::Preferences& aprefs)
{
    aprefs.d->copyToWebCoreSettings(&_currentGroup->settings);
}

OSB::Preferences* RootImpl::preferences()
{
    prefs.d->settings = &_currentGroup->settings;
    return &prefs;
}

void RootImpl::setResourceLoadDelegate(OSB::ResourceLoadDelegate* d)
{
    _mainFrame->setResourceLoadDelegate(d);
}

void RootImpl::setUIDelegate(OSB::UIDelegate* d)
{
    _mainFrame->setUIDelegate(d);
}


void RootImpl::setTextSizeMultiplier(float multiplier)
{
    _textSizeMultiplier = multiplier;
    _mainFrame->setTextSizeMultiplier(multiplier);
}

float RootImpl::textSizeMultiplier()
{
    return _textSizeMultiplier;
}


FrameImpl* RootImpl::mainFrame()
{
    return _mainFrame;
}

bool RootImpl::searchFor(const gchar *string, bool forward, bool caseSensitive)
{
    return _mainFrame->searchFor(string, forward, caseSensitive, false /*wrapFlag*/);
}

OSB::BackForwardList* RootImpl::backForwardList()
{
//    return &(_mainFrame->history());
    return 0L;
}

OSB::URLCredentialStorage* RootImpl::credentialStorage() {
    return _mainFrame->credentials();
}


void RootImpl::goBack()
{
    _mainFrame->goBack();
}

bool RootImpl::canGoBack()
{
    return _mainFrame->canGoBack();
}

void RootImpl::goForward()
{
    _mainFrame->goForward();
}

bool RootImpl::canGoForward()
{
    return _mainFrame->canGoForward();
}

OSB::ProtocolDelegate* RootImpl::protocolDelegateForURL(const gchar* url)
{
    static InternalHttpProtoDelegate httpProv;
    if (g_str_has_prefix(url, "http://"))
	return &httpProv;

    return 0;
}

void RootImpl::registerContentRepresentation(OSB::ContentRepresentationFactory* crf, const gchar* mimeType)
{
    _mainFrame->addContentRepresentation(crf, mimeType);
}

GList* RootImpl::peers()
{
    return _currentGroup->mainFrames;
}
OSB::URLCredentialStorage* RootImpl::credentials()
{
    return &_currentGroup->credentials;
}

WebCoreSettings* RootImpl::sharedSettings()
{
    return &_currentGroup->settings;
}
