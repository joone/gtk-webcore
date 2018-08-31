/* DON'T INCLUDE THIS FILE TO LIBRARY
 * ONLY USED ON TEST
 * */
#include <stdlib.h>
#include <string.h>

void *
operator new (size_t s)
{
    void *p = malloc (s);
/*    if (p)
        memset (p,0,s);*/
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

