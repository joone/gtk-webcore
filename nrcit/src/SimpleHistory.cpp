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
#include <assert.h>
#include "SimpleHistory.h"

SimpleHistory::SimpleHistory()
    :locked(false)
{
    start = g_queue_new();
    end = g_queue_new();
}

SimpleHistory::~SimpleHistory()
{
    gchar* str=0;
    while((str=(gchar*)g_queue_pop_head(start)))
	g_free(str);
    
    while((str=(gchar*)g_queue_pop_head(end)))
	g_free(str);    

    g_queue_free(start);    
    g_queue_free(end);
}

void SimpleHistory::lock() 
{ 
    locked = true;
}

void SimpleHistory::unlock() 
{
    locked = false;
}

void SimpleHistory::pushNew(const gchar* url)
{
    assert(url);
    if (locked) return;    
    
    g_queue_push_head(start, g_strdup(url));
    
    // empty end queue.
    gchar* str=0;
    while((str=(gchar*)g_queue_pop_head(end)))
        g_free(str);    
}

void SimpleHistory::setPos(int newPos)
{ 
    // bogosort.

    if (newPos > pos()) {
	if (newPos < length()){
	    int i = newPos - pos();
	    while(i--) { 
		next();
	    }
	} else {
	    // lenght out of range.
	}
    } else {
	if (pos() > 0){
	    while(newPos--){
		prev();
	    }
	} else {
	    // length out of range.
	}
	
    }
}

int SimpleHistory::pos()
{
    return start->length - 1;
}

int SimpleHistory::length()
{
    return pos() + end->length;
}

const gchar* SimpleHistory::current()
{    
    return (gchar*) g_queue_peek_head(start);
}

void SimpleHistory::next()
{
    gchar* str = (gchar*)g_queue_pop_head(end);
    if (str) g_queue_push_head(start, str);
}

void SimpleHistory::prev()
{
    gchar* str = (gchar*)g_queue_pop_head(start);
    if (str) g_queue_push_head(end, str);
}
