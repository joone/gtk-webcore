/*
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
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

#ifndef QLINEEDIT_H_
#define QLINEEDIT_H_

#include "KWQWidget.h"
#include "KWQEvent.h"
#include "KWQString.h"
#include "KWQSignal.h"

class QLineEdit : public QWidget {
public:
    enum Type { Normal, Password, Search };

#if KWIQ
    QLineEdit(Type type, QWidget * parent = 0, const char * name = 0, int f = 0);
#else
    QLineEdit();
#endif    
    ~QLineEdit();
    void setAlignment(AlignmentFlags);

    void setCursorPosition(int);
    int cursorPosition() const;

    void setEdited(bool);
    bool edited() const;

    void setFont(const QFont &font);
    
    void setMaxLength(int);
    int maxLength() const;

    void setReadOnly(bool);
    bool isReadOnly() const;

    void setText(const QString &);
    QString text();

    void setWritingDirection(QPainter::TextDirection);
    
    void selectAll();
    
    QSize sizeForCharacterWidth(int numCharacters) const;
    int baselinePosition(int height) const;
    
    void returnPressed() { m_returnPressed.call(); }
    void textChanged() { m_textChanged.call(text()); }
    void performSearch() { m_performSearch.call(); }

    void clicked();
    
    virtual FocusPolicy focusPolicy() const;
    virtual bool checksDescendantsForFocus() const;

    Type type() const { return m_type; }
    
    void setLiveSearch(bool liveSearch);
    void setAutoSaveName(const QString& name);
    void setMaxResults(int maxResults);
    void setPlaceholderString(const QString& placeholder);
    void addSearchResult();

private:
    KWQSignal m_returnPressed;
    KWQSignal m_textChanged;
    KWQSignal m_clicked;
    KWQSignal m_performSearch;
    Type m_type;

//  KWIQ: not used
//    KWQTextFieldController *m_controller;
    
    bool m_edited;

};

#endif
