#include <stdlib.h>
#include <string.h>

#include <new>

#undef CLEAR_MEMORY
// #define CLEAR_MEMORY 

void *
operator new (size_t s)
{
    void *p = malloc (s);
#if CLEAR_MEMORY 
    if (p)
        memset (p,0,s);
#endif
    return p;
}

void
operator delete (void *p)
{
    free (p);
}

void *
operator new[] (size_t s)
{
    return ::operator new (s);
}

void
operator delete[] (void *p)
{
    ::operator delete (p);
}

extern "C" void
__cxa_pure_virtual (void)
{
}

