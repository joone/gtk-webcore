dnl OSB_ENABLE_DEBUG
dnl -----------------
dnl check to enable debug compiler flags
AC_DEFUN([OSB_ENABLE_DEBUG],
[AC_ARG_ENABLE([debug],
	AC_HELP_STRING([--enable-debug],[enable debugging (default is no)]),
	[ac_cv_use_debug=$enableval], [ac_cv_use_debug=no])
AC_CACHE_CHECK([whether to enable debugging],
	[ac_cv_use_debug], [ac_cv_use_debug=no])

if test x$ac_cv_use_debug = xyes; then
    CFLAGS="`echo $CFLAGS | sed -n -e 's/-g[[:digit:]]//gp'` -g3"
fi
])

dnl OSB_ENABLE_PROFILING
dnl --------------------
dnl check to enable profiling
AC_DEFUN([OSB_ENABLE_PROFILING],
[AC_ARG_ENABLE([profiling],
	AC_HELP_STRING([--enable-profiling],[enable profiling (default is no)]),
	[ac_cv_use_profiling=$enableval], [ac_cv_use_profiling=no])
AC_CACHE_CHECK([whether to enable profiling],
	[ac_cv_use_profiling], [ac_cv_use_profiling=no])

if test x$ac_cv_use_profiling = xyes; then
    CFLAGS="$CFLAGS -pg"
    LDFLAGS="$LDFLAGS -pg"
fi
])

dnl OSB_ENABLE_OPTIMIZATION
dnl -----------------------
dnl check to enable optimization
AC_DEFUN([OSB_ENABLE_OPTIMIZATION],
[AC_ARG_ENABLE([optimization],
	AC_HELP_STRING([--enable-optimizetion],[enable optimization (no,size,yes) (default is yes)]),
	[ac_cv_use_optimization=$enableval], [ac_cv_use_optimization=yes])
AC_CACHE_CHECK([whether to enable optimization],
	[ac_cv_use_optimization], [ac_cv_use_optimization=yes])
case $ac_cv_use_optimization in
	yes)
		CFLAGS="`echo $CFLAGS | sed -n -e 's/-O[[:digit:]]\{0,1\}//gp'` -O2"
		;;
	size)
		CFLAGS="`echo $CFLAGS | sed -n -e 's/-O[[:digit:]]\{0,1\}//gp'` -Os"
		;;	
esac

])
dnl OSB_HAVE_ISNAN
dnl --------------
dnl
AC_DEFUN([OSB_HAVE_ISNAN],
[AC_CHECK_FUNCS(isnan,[
	AC_DEFINE(HAVE_FUNC_ISNAN,[],[Define to 1 if you have isnan function.])
	],)])

dnl OSB_HAVE_ISINF
dnl --------------
dnl
AC_DEFUN([OSB_HAVE_ISINF],
[AC_CHECK_FUNCS(isinf,[
        AC_DEFINE(HAVE_FUNC_ISINF,[],[Define to 1 if you have isinf function.])
	],)])

dnl OSB_HAVE_INFINITE
dnl --------------
dnl
AC_DEFUN([OSB_HAVE_INFINITE],
[AC_CHECK_FUNCS(infinite,[
	AC_DEFINE(HAVE_FUNC_INFINITE,[],[Define to 1 if you have isinfinite function.])
	],)])

dnl OSB_ENABLE_JAVASCRIPT
dnl ---------------------
dnl check to enable javascript.
AC_DEFUN([OSB_ENABLE_JAVASCRIPT],
[AC_ARG_ENABLE([javascript],
	AC_HELP_STRING([--enable-javascript],[use javascript (default is yes)]),
	[ac_cv_use_javascript=$enableval], [ac_cv_use_javascript=yes])
AC_CACHE_CHECK([whether to use javascript],
	[ac_cv_use_javascript], [ac_cv_use_javascript=yes])

])

dnl OSB_ENABLE_INTLIBCURL
dnl ------------------
dnl check libcurl. currently path thingy doesnt atleast work
AC_DEFUN([OSB_ENABLE_INTLIBCURL],
[AC_ARG_ENABLE([intlibcurl],
	AC_HELP_STRING([--enable-intlibcurl],[use internal libcurl (default is YES)]),
	[ac_cv_use_intlibcurl=$enableval], [ac_cv_use_intlibcurl=yes])
AC_CACHE_CHECK([whether to use internal libcurl],
	[ac_cv_use_intlibcurl], [ac_cv_use_intlibcurl=yes])

if test x$ac_cv_use_intlibcurl = xno; then
    AC_CHECK_PROG(curl_config, curl_config, true,false,)
    if text x$curlconfig = xtrue; then
       LIBCURL_INCLUDE=`curl-config --cflags`
       LIBCURL_LIBS=`curl-config --libs`
    else
	AC_MSG_ERROR([libcurl not installed, or curl-config not in path])
    fi
else
    if test x$ac_cv_use_intlibcurl = xyes; then	
	LIBCURLDIR=$default_libcurl_dir
	AC_CONFIG_SUBDIRS(curl-7.10.6)
    else
	LIBCURLDIR=$ac_cv_use_intlibcurl
    fi
    LIBCURL_INCLUDE="-I$LIBCURLDIR/include/"
    LIBCURL_LIBS="$LIBCURLDIR/lib/libcurl.la" 
    AC_SUBST(LIBCURLDIR)
fi 

AC_SUBST(LIBCURL_INCLUDE)
AC_SUBST(LIBCURL_LIBS)
])
