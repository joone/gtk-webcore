/*
 * Copyright (C) 2001, 2002 Apple Computer, Inc.  All rights reserved.
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

#include "KWQKPartsHistoryProvider.h"

#include "KWQKURL.h"
#include "KWQLogging.h"
#include "WebCoreHistory.h"

namespace KParts {

HistoryProvider *HistoryProvider::self()
{
    static HistoryProvider instance;
    return &instance;
}

void HistoryProvider::insert(const QString &s)
{
    // Insertion handled by WebKit.
}

bool HistoryProvider::contains(const QString &s) const
{
    // the other side of the bridge is careful not to throw exceptions here

    if (s.hasFastLatin1()) {
	return WebCoreHistory::historyProvider()->containsItemForURLLatin1(s.latin1(), s.length());
    } else {
	return WebCoreHistory::historyProvider()->containsItemForURLUTF8(s.utf8(), s.length());
	// return WebCoreHistory::historyProvider()->containsItemForURLUnicode(UniChar *)s.unicode() s.length());
    }
}

} // namespace KParts
