#ifdef __cplusplus
#define NULL __null
#else
#define NULL ((void *)0)
#endif

#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus

#include <algorithm>
#include <cstddef>
#include <new>

#ifndef NDEBUG
#include <ostream>
#endif

#if !KWIQ
// Work around bug 3553309 by re-including <ctype.h>.
#include <cctype>
#define isalnum(c)      __istype((c), (_CTYPE_A|_CTYPE_D))
#define isalpha(c)      __istype((c), _CTYPE_A)
#define iscntrl(c)      __istype((c), _CTYPE_C)
#define isdigit(c)      __isctype((c), _CTYPE_D)	/* ANSI -- locale independent */
#define isgraph(c)      __istype((c), _CTYPE_G)
#define islower(c)      __istype((c), _CTYPE_L)
#define isprint(c)      __istype((c), _CTYPE_R)
#define ispunct(c)      __istype((c), _CTYPE_P)
#define isspace(c)      __istype((c), _CTYPE_S)
#define isupper(c)      __istype((c), _CTYPE_U)
#define isxdigit(c)     __isctype((c), _CTYPE_X)	/* ANSI -- locale independent */
#define tolower(c)      __tolower(c)
#define toupper(c)      __toupper(c)
#endif
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <time.h>
#if !KWIQ
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#endif

#define KHTML_NO_XBL 1

#if !KWIQ
#ifdef __OBJC__

#import <Cocoa/Cocoa.h>
#endif
#endif

#if KWIQ

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#if defined(__GNUC__)
#  if defined(__arm__) || defined(__ARMEL__)
#   define Q_PACKED __attribute__ ((packed))
#  endif
#endif

#ifndef Q_PACKED
#  define Q_PACKED
#endif

#ifndef PRUword
#define PRUword unsigned short 
#endif

#endif
