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
#ifndef GObjectMisc_h
#define GObjectMisc_h

#include <glib-object.h>
#include <assert.h>

template <typename T>
class GPtr
{

    GPtr<T>& operator=(const GPtr<T>& other);

public:
    GPtr() : ptr(0), conn_sigs(0)
    {

    }

    explicit GPtr(T* obj) : ptr(obj), conn_sigs(0) 
    {
	if (ptr) {
	    assert(G_IS_OBJECT(ptr)); 
	    g_object_ref(ptr);
	}
    }
    
  
    ~GPtr() { 
	if (ptr) {
	    disconnect_all();
	    g_object_unref(ptr);
	}
    }

    operator T*() const { return ptr;}
    operator GTypeInstance*() const { return (GTypeInstance*) ptr;}
    operator void*() const {return ptr;}
    operator bool() const { return ptr!=0;}
    T* operator->() const { return ptr; }  

    GPtr<T>& operator=(T *obj)
    { 
	if (ptr) {
	    disconnect_all();
	    g_object_unref(ptr); 
	    ptr = 0;
	}

	if (obj) {
	    assert(G_IS_OBJECT (obj));
	    ptr = obj;
	    g_object_ref(ptr);		
	}
        return *this;
    }

    template <typename R, typename A>
    GPtr<T>& connect(const gchar* signal_name, R (*callback)(A,gpointer), gpointer data) 
    {
	return connect(signal_name, G_CALLBACK (callback), data);
    }

    template <typename R, typename A, typename B>
    GPtr<T>& connect(const gchar* signal_name, R (*callback)(A,B,gpointer), gpointer data) 
    {
	return connect(signal_name, G_CALLBACK (callback), data);
    }

    template <typename R, typename A, typename B, typename C>
    GPtr<T>& connect(const gchar* signal_name, R (*callback)(A,B,C,gpointer), gpointer data) 
    {
	return connect(signal_name, G_CALLBACK (callback), data);
    }    

    template <typename R, typename A, typename B, typename C, typename D>
    GPtr<T>& connect(const gchar* signal_name, R (*callback)(A,B,C,D,gpointer), gpointer data) 
    {
	return connect(signal_name, G_CALLBACK (callback), data);
    }
    template <typename R, typename A, typename B, typename C, typename D, typename E>
    GPtr<T>& connect(const gchar* signal_name, R (*callback)(A,B,C,D,E, gpointer), gpointer data) 
    {
	return connect(signal_name, G_CALLBACK (callback), data);
    }

    GPtr<T>& connect(const gchar* signal_name, GCallback callback, gpointer data)
    {
	assert(ptr);
	assert(callback);
	assert(signal_name);
	
	gulong* idp = g_new(gulong,1);

	*idp = g_signal_connect(G_OBJECT (ptr), 
				signal_name,
				callback,
				data);

	conn_sigs = g_list_append(conn_sigs, idp);
	return *this;
    }

    GPtr<T>& disconnect_all()
    {
	if (ptr) {
	    GList* iter = g_list_first(conn_sigs);
	    while (iter) {
		g_signal_handler_disconnect(ptr, *((gulong*)iter->data));
		g_free(iter->data);
		iter = g_list_next(iter);
	    }
	    g_list_free(conn_sigs);
	    conn_sigs = 0;
	}
	return *this;
    }

private:    
    T* ptr;
    GList* conn_sigs; // list of gulong
};

#endif
