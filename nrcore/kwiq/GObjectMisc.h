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
