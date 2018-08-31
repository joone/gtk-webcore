#ifdef __cplusplus
#define NULL __null
#else
#define NULL ((void *)0)
#endif

#include <config.h>

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>

#ifdef __cplusplus
#if !KWIQ
#include <list>
#include <typeinfo>
#endif
#endif

#if KWIQ
#include <ctype.h>
#include <unistd.h>
#include <new>       /* for collection.cpp */

#include <errno.h>   /* for date_object.cpp */
#include <pthread.h> /* for internal.cpp */



#if defined(__GNUC__)
#  if defined(__arm__) || defined(__ARMEL__)
#   define Q_PACKED __attribute__ ((packed))
#  endif
#endif

#ifndef Q_PACKED
#  define Q_PACKED
#endif

#endif
