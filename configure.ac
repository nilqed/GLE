########################################################################
#                                                                      #
# GLE - Graphics Layout Engine <http://www.gle-graphics.org/>          #
#                                                                      #
# Modified BSD License                                                 #
#                                                                      #
# Copyright (C) 2009 GLE.                                              #
#                                                                      #
# Redistribution and use in source and binary forms, with or without   #
# modification, are permitted provided that the following conditions   #
# are met:                                                             #
#                                                                      #
#    1. Redistributions of source code must retain the above copyright #
# notice, this list of conditions and the following disclaimer.        #
#                                                                      #
#    2. Redistributions in binary form must reproduce the above        #
# copyright notice, this list of conditions and the following          #
# disclaimer in the documentation and/or other materials provided with #
# the distribution.                                                    #
#                                                                      #
#    3. The name of the author may not be used to endorse or promote   #
# products derived from this software without specific prior written   #
# permission.                                                          #
#                                                                      #
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR   #
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED       #
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   #
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY       #
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   #
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE    #
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS        #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER #
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      #
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN  #
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        #
#                                                                      #
########################################################################

m4_define([gle_cr_version], [4.2.4b])

AC_INIT(gle-graphics, [gle_cr_version], glx-general@lists.sourceforge.net)
AC_CONFIG_HEADER(src/config.h)
AC_CONFIG_SRCDIR(src/gle/gle.cpp)

GLE_VERSION=gle_cr_version

AC_PROG_CXX
AC_LANG_CPLUSPLUS

AH_TEMPLATE(__UNIX__,  [On Unix])
AH_TEMPLATE(__CYGWIN__,[On Cygwin])
AH_TEMPLATE(__MACOS__, [On Mac])
AH_TEMPLATE(__FREEBSD__, [On FreeBSD])
AH_TEMPLATE(__GNU_HURD__, [On GNU Hurd])
AH_TEMPLATE(__LINUX__, [On Linux])

HAVE_SOCKETS=1
HAVE_LIBTIFF=0
HAVE_LIBPNG=0
HAVE_LIBJPEG=0
HAVE_X11=0
HAVE_CAIRO=0
HAVE_POPPLER=0
HAVE_NCURSES_H=0
HAVE_CURSES_H=0
HAVE_NOSCRIPTS=0
HAVE_QT=0
HAVE_AUTOPACKAGE=0
HAVE_LIBGLE=0
HAVE_LIBGLE_CPPFLAGS=0
HAVE_LIBGLE_STATIC=0
HAVE_MAC=0
HAVE_FREEBSD=0
HAVE_GNU_HURD=0
HAVE_LINUX=0
HAVE_WIN32=0
HAVE_NOCYGWIN=0
HAVE_CYGWIN=0
HAVE_MINGW=0
HAVE_LIBZ=0
HAVE_LIBGLU_H=0
QT_CONFIG="CONFIG += release"
QT_RPATH=
QT_LIBGLU_LIB=

HAVE_STATIC_LIBPNG=0
HAVE_STATIC_LIBTIFF=0
HAVE_STATIC_LIBJPEG=0
HAVE_STATIC_LIBZ=0
HAVE_RPATH=0

QMAKE_PROG=none
QMAKE_VERS=none

MANIP_LIBS=
MANIP_LDFLAGS=
AP_CONFIG_ARGS=

LIBGLE_CPPFLAGS=
LIBGLE_LDFLAGS=
EXTRA_LDFLAGS=
EXTRA_LIBS=
SIMPLE_LIBS_DL=

AC_ARG_WITH(qt,           [  --with-qt[=DIR]           where to find the Qt-4.1.x library (or yes/no)],,with_qt=yes)
AC_ARG_WITH(jpeg,         [  --with-jpeg[=DIR]         where to find the jpeg library (or yes/no)],,with_jpeg=yes)
AC_ARG_WITH(png,          [  --with-png[=DIR]          where to find the png library (or yes/no)],,with_png=yes)
AC_ARG_WITH(tiff,         [  --with-tiff[=DIR]         where to find the tiff library (or yes/no)],,with_tiff=yes)
AC_ARG_WITH(z,            [  --with-z[=DIR]            where to find the z library (or yes/no)],,with_z=yes)
AC_ARG_WITH(cairo,        [  --with-cairo            use the Cairo graphics library (yes/no)],,with_cairo=yes)
AC_ARG_WITH(poppler,      [  --with-poppler          use the Poppler PDF library (yes/no)],,with_poppler=yes)
AC_ARG_WITH(manip,        [  --with-manip            build manip utility (yes/no)],,with_manip=yes)
AC_ARG_WITH(debug,        [  --with-debug            compile in debug information (yes/no)],,with_debug=no)
AC_ARG_WITH(warn,         [  --with-warn             compile with all warnings enabled (yes/no)],,with_warn=no)
AC_ARG_WITH(win32,        [  --with-win32            compile Windows binary (yes/no)],,with_win32=no)
AC_ARG_WITH(arch,         [  --with-arch             compile for a specific Mac OS architecture (or default/no)],,with_arch=default)
AC_ARG_WITH(scripts,      [  --with-scripts          install scripts instead of executables (yes/no)],,with_scritps=no)
AC_ARG_WITH(libgle,       [  --with-libgle           build libgle (yes/no/static/both)],,with_libgle=no)
AC_ARG_WITH(rpath,        [  --with-rpath            build with rpath (or yes/no)],,with_rpath=yes)
AC_ARG_WITH(extrafonts,   [  --with-extrafonts       include additional fonts (or yes/no)],,extrafonts=no)
AC_ARG_WITH(gnuwin32,     [  --with-gnuwin32         build with GnuWin32 libs (yes/no)],,with_gnuwin32=no)
AC_ARG_WITH(staticlink,   [  --with-staticlink       statically link bitmap libs (yes/no)],,with_staticlink=def)
AC_ARG_WITH(autopackage,  [  --with-autopackage      create autopackage package (yes/no)],,with_autopackage=no)
AC_ARG_WITH(apintern,     [  --with-apintern         autopackage (internal use) (yes/no)],,with_apintern=no)

HAVE_EXTRA_FONTS=0
if test "x$extrafonts" != "xno"; then
    HAVE_EXTRA_FONTS=1
fi

if test -f src/fonts/extrafonts.inf; then
    HAVE_EXTRA_FONTS=1
fi

dnl ********************************************************************************
dnl build snapshot or release?
dnl ********************************************************************************

GLE_SNAPSHOT=""

dnl ********************************************************************************
dnl check CPU type and OS
dnl ********************************************************************************

AC_CONFIG_AUX_DIR(platform/autoconf)
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_CANONICAL_HOST
AC_CHECK_HEADERS([sys/param.h])
AC_C_BIGENDIAN
AC_TYPE_UINT16_T
PKG_PROG_PKG_CONFIG

if test "x$with_rpath" != "xno"; then
    HAVE_RPATH=1
fi

dnl ********************************************************************************
dnl check for Win32
dnl ********************************************************************************

if test "x$with_gnuwin32" != "xno"; then
    with_win32=yes
    CPPFLAGS="-I$with_gnuwin32/include $CPPFLAGS"
    CXXFLAGS="-I$with_gnuwin32/include $CXXFLAGS"
    LDFLAGS="-L$with_gnuwin32/lib $LDFLAGS"
    CFLAGS="-I$with_gnuwin32/include $CFLAGS"
    if test "x$with_staticlink" = "xdef"; then
        with_staticlink=yes
    fi
fi

if test "x$with_win32" != "xno"; then
    HAVE_WIN32=1
fi

dnl ********************************************************************************
dnl check for host type
dnl ********************************************************************************

USE_ARCH=no

case $host in
    *-*-cygwin*)
        AC_DEFINE(__CYGWIN__)
        HAVE_CYGWIN=1
    ;;
    *-*-mingw*)
        AC_DEFINE(__CYGWIN__)
        HAVE_CYGWIN=1
        HAVE_MINGW=1
    ;;
	x86_64-apple-*)
        if test "x$with_arch" = "xdefault"; then
	       USE_ARCH=i386
        fi
	;;
esac

if test "x$with_arch" != "xdefault"; then
	USE_ARCH=$with_arch
fi

if test "x$USE_ARCH" != "xno"; then
	CPPFLAGS="-arch $USE_ARCH $CPPFLAGS"
	CXXFLAGS="-arch $USE_ARCH $CXXFLAGS"
	LDFLAGS="-arch $USE_ARCH $LDFLAGS"
	CFLAGS="-arch $USE_ARCH $CFLAGS"
fi

if test "x$HAVE_CYGWIN" = "x1"; then
    if test "x$HAVE_WIN32" = "x1"; then
         HAVE_CYGWIN=0
         HAVE_NOCYGWIN=1
    fi
fi

if test "x$HAVE_NOCYGWIN" = "x1"; then
    if test "x$with_gnuwin32" = "xno"; then
        with_png=no
        with_jpeg=no
        with_tiff=no
        with_z=no
    fi
fi

dnl ********************************************************************************
dnl check for Win32
dnl ********************************************************************************

if test "x$HAVE_WIN32" != "x1"; then
    AC_DEFINE(__UNIX__)
    # MacOS should also define __UNIX__
else
    AC_DEFINE(__WIN32__)
    AC_DEFINE(__NOCYGWIN__)
    CPPFLAGS="-mno-cygwin -mthreads -mms-bitfields -march=i386 $CPPFLAGS"
    CXXFLAGS="-mno-cygwin -mthreads -mms-bitfields -march=i386 $CXXFLAGS"
    LDFLAGS="-mno-cygwin -mthreads -mms-bitfields -march=i386 $LDFLAGS"
    CFLAGS="-mno-cygwin -mthreads -mms-bitfields -march=i386 $CFLAGS"
    with_manip=no
    with_x=no
fi

dnl ********************************************************************************
dnl remember original value of different flags
dnl ********************************************************************************

_CPPFLAGS_INI=$CPPFLAGS
_CXXFLAGS_INI=$CXXFLAGS
_LDFLAGS_INI=$LDFLAGS
_LIBS_INI=$LIBS

SIMPLE_LDFLAGS=$LDFLAGS
SIMPLE_LIBS=$LIBS

dnl ********************************************************************************
dnl OS Selection
dnl ********************************************************************************

case $host in
    *-apple*)
        HAVE_MAC=1
    ;;
    *-freebsd*)
        HAVE_FREEBSD=1
    ;;
    *-linux*)
        HAVE_LINUX=1
    ;;
    *-gnu*)
        HAVE_GNU_HURD=1
    ;;
esac

dnl ********************************************************************************
dnl is Mac OX/X?
dnl ********************************************************************************

if test "x$HAVE_MAC" = "x1"; then
    AC_DEFINE(__MACOS__)
    HAVE_RPATH=0
fi

dnl ********************************************************************************
dnl is Free BSD?
dnl ********************************************************************************

if test "x$HAVE_FREEBSD" = "x1"; then
    AC_DEFINE(__FREEBSD__)
fi

dnl ********************************************************************************
dnl is Linux?
dnl ********************************************************************************

if test "x$HAVE_LINUX" = "x1"; then
    AC_DEFINE(__LINUX__)
fi

dnl ********************************************************************************
dnl is GNU / Hurd
dnl ********************************************************************************

if test "x$HAVE_GNU_HURD" = "x1"; then
    AC_DEFINE(__GNU_HURD__)
fi

dnl ********************************************************************************
dnl install scripts
dnl ********************************************************************************

if test "$with_scripts" != yes; then
     case $host in
        *-*-cygwin*|*-*-linux*|*-*-mingw*|*-*-kfreebsd*)
            HAVE_NOSCRIPTS=1
        ;;
     esac
     if test "x$HAVE_MAC" = "x1"; then
        HAVE_NOSCRIPTS=1
     fi
     if test "x$HAVE_FREEBSD" = "x1"; then
        HAVE_NOSCRIPTS=1
     fi
     if test "x$HAVE_LINUX" = "x1"; then
        HAVE_NOSCRIPTS=1
     fi
     if test "x$HAVE_GNU_HURD" = "x1"; then
        HAVE_NOSCRIPTS=1
     fi
fi

dnl ********************************************************************************
dnl check presence of X11
dnl ********************************************************************************

AC_PATH_X
if test "$with_x" != no; then
    if test "x$x_libraries" = "x"; then
        x_libraries=.
    fi
    AC_CHECK_LIB([X11], [XOpenDisplay],
        [HAVE_X11=1
         if test "x$x_libraries" != "x"; then
             LDFLAGS="$LDFLAGS -L$x_libraries"
         fi
         if test "x$x_includes" != "x"; then
             CPPFLAGS="$CPPFLAGS -I$x_includes"
         fi
         LIBS="-lX11 $LIBS"
        ],
        [],
        [[-lXt -L$x_libraries]])
fi

dnl ********************************************************************************
dnl check for static libs option
dnl ********************************************************************************

if test "x$with_staticlink" = "xdef"; then
    with_staticlink=no
fi

if test "x$with_staticlink" != "xno"; then
    HAVE_STATIC_LIBPNG=1
    HAVE_STATIC_LIBJPEG=1
    HAVE_STATIC_LIBZ=1
    HAVE_STATIC_LIBTIFF=1
fi

dnl ********************************************************************************
dnl check presence of Z library
dnl ********************************************************************************

if test "$with_z" != no; then
    AC_CHECK_LIB(z,deflate,
      [HAVE_LIBZ=1],
      [])

    if test "x$HAVE_LIBZ" = "x1"; then
        AC_DEFINE(HAVE_LIBZ)
        if test "x$HAVE_STATIC_LIBZ" = "x1"; then
            LIBS="-Wl,-Bstatic -lz -Wl,-Bdynamic $LIBS"
        else
            LIBS="-lz $LIBS"
        fi
    fi
fi

dnl ********************************************************************************
dnl check presence of sockets library
dnl on Linux, no extra library is required for this
dnl ********************************************************************************

SOCKET_LIB=0

AC_SEARCH_LIBS(socket,socket)
AC_SEARCH_LIBS(gethostbyname,nsl socket)
if test "$ac_cv_func_socket" = "yes" ; then
   if test "$ac_cv_func_gethostbyname" = "yes" ; then
      SOCKET_LIB=1
   fi
fi

case $host in
    *-*-solaris*)
   if test "$SOCKET_LIB" = "0" ; then
      HAVE_SOCKETS=0
   fi
    ;;
    *)
esac

if test "$HAVE_SOCKETS" = 1; then
    AC_DEFINE(HAVE_SOCKETS)
fi

dnl ********************************************************************************
dnl check presence of JPEG library
dnl ********************************************************************************

_CPPFLAGS=$CPPFLAGS
_LDFLAGS=$LDFLAGS

if test "$with_jpeg" != no; then
   if test -d "$with_jpeg"; then
    libjpeg_CPPFLAGS="-I$with_jpeg/include"
    libjpeg_LDFLAGS="-L$with_jpeg/lib"
    CPPFLAGS="$CPPFLAGS $libjpeg_CPPFLAGS"
    LDFLAGS="$LDFLAGS $libjpeg_LDFLAGS"
  fi

  AC_CHECK_LIB(jpeg,jpeg_destroy_decompress,
    [AC_CHECK_HEADERS(jpeglib.h,
    [HAVE_LIBJPEG=1
     AC_DEFINE(HAVE_LIBJPEG)
    ],
    [AC_MSG_WARN([found jpeg library but not jpegio.h: make sure libjpeg development package is installed])
    ])],
    [CPPFLAGS=$_CPPFLAGS
     LDFLAGS=$_LDFLAGS],
    [-lm])
fi

if test "x$HAVE_LIBJPEG" = "x1"; then
    if test "x$HAVE_STATIC_LIBJPEG" = "x1"; then
        LIBS="-Wl,-Bstatic -ljpeg -Wl,-Bdynamic $LIBS"
    else
        LIBS="-ljpeg $LIBS"
    fi
fi

dnl ********************************************************************************
dnl check presence of TIFF library
dnl ********************************************************************************

_CPPFLAGS=$CPPFLAGS
_LDFLAGS=$LDFLAGS

if test "$with_tiff" != no; then
   if test -d "$with_tiff"; then
    libtiff_CPPFLAGS="-I$with_tiff/include"
    libtiff_LDFLAGS="-L$with_tiff/lib"
    CPPFLAGS="$CPPFLAGS $libtiff_CPPFLAGS"
    LDFLAGS="$LDFLAGS $libtiff_LDFLAGS"
  fi

  AC_CHECK_LIB(tiff,TIFFReadScanline,
    [AC_CHECK_HEADERS(tiffio.h,
    [HAVE_LIBTIFF=1
     AC_DEFINE(HAVE_LIBTIFF)
    ],
    [AC_MSG_WARN([found TIFF library but not tiffio.h: make sure libtiff development package is installed])
     CPPFLAGS=$_CPPFLAGS
     LDFLAGS=$_LDFLAGS
    ])],
    [CPPFLAGS=$_CPPFLAGS
     LDFLAGS=$_LDFLAGS],
    [-lm])
fi

if test "x$HAVE_LIBTIFF" = "x1"; then
    if test "x$HAVE_STATIC_LIBTIFF" = "x1"; then
        LIBS="-Wl,-Bstatic -ltiff -Wl,-Bdynamic $LIBS"
    else
        LIBS="-ltiff $LIBS"
    fi
fi

dnl ********************************************************************************
dnl check presence of PNG library
dnl ********************************************************************************

if test "$with_png" != no; then
  if test -d "$with_png"; then
   HAVE_LIBPNG=1
   AC_DEFINE(HAVE_LIBPNG)
    CPPFLAGS="$CPPFLAGS -I$with_png/include"
    LDFLAGS="$LDFLAGS -L$with_png/lib"
    if test "x$with_staticlink" = "xyes"; then
       LIBS="-Wl,-Bstatic -lpng -Wl,-Bdynamic $LIBS"
    else
       LIBS="-lpng $LIBS"
    fi
  else
    PKG_CHECK_MODULES([libpng], [libpng >= 0],
      [HAVE_LIBPNG=1
       AC_DEFINE(HAVE_LIBPNG)
       if test "x$with_staticlink" = "xyes"; then
          LIBS="-Wl,-Bstatic $libpng_LIBS -Wl,-Bdynamic $LIBS"
       else
          LIBS="$libpng_LIBS $LIBS"
       fi
       CPPFLAGS="$libpng_CFLAGS $CPPFLAGS"
       LDFLAGS="$libpng_LDFLAGS $LDFLAGS"
      ],
      [AC_MSG_RESULT([no])]
    )
    if test "x$HAVE_LIBPNG" = "x0"; then
      AC_PATH_PROG([LIBPNG_CONFIG], [libpng-config])
      if test -n "$LIBPNG_CONFIG"; then
         HAVE_LIBPNG=1
         AC_DEFINE(HAVE_LIBPNG)
         libpng_CPPFLAGS=`libpng-config --cflags`
         libpng_LDFLAGS=`libpng-config --ldflags`
         CPPFLAGS="$libpng_CPPFLAGS $CPPFLAGS"
         LDFLAGS="$libpng_LDFLAGS $LDFLAGS"
         if test "x$with_staticlink" = "xyes"; then
            LIBS="-Wl,-Bstatic -lpng -Wl,-Bdynamic $LIBS"
         else
            LIBS="-lpng $LIBS"
         fi
      fi
    fi
  fi
fi

dnl ********************************************************************************
dnl check Cairo
dnl ********************************************************************************

if test "x$with_cairo" != "xno"; then
	PKG_CHECK_MODULES([cairo], [cairo >= 1.2.0],
	   [
		HAVE_CAIRO=1
		AC_MSG_RESULT([   cairo CFLAGS: $cairo_CFLAGS])
		AC_MSG_RESULT([   cairo LIBS: $cairo_LIBS])
		if test "x$with_staticlink" = "xyes"; then
		   LIBS="-Wl,-Bstatic $cairo_LIBS -Wl,-Bdynamic $LIBS"
		else
		   LIBS="$cairo_LIBS $LIBS"
		fi
		CPPFLAGS="$cairo_CFLAGS $CPPFLAGS"
		LDFLAGS="$cairo_LDFLAGS $LDFLAGS"
	   ],
	   [
		HAVE_CAIRO=0
	   ]
	)
fi

if test "x$HAVE_CAIRO" = "x1"; then
   if test "x$HAVE_WIN32" = "x1"; then
      if test "x$with_staticlink" = "xyes"; then
         EXTRA_LDFLAGS="$EXTRA_LDFLAGS -mwindows"
         EXTRA_LIBS="$EXTRA_LIBS -lmsimg32"
      fi
   fi
fi

dnl ********************************************************************************
dnl check Poppler
dnl ********************************************************************************

if test "x$HAVE_CAIRO" = "x1"; then
	if test "x$with_poppler" != "xno"; then
		PKG_CHECK_MODULES(POPPLER, [poppler-glib >= 0.10.0],
		   [
			HAVE_POPPLER=1
			AC_MSG_RESULT([   poppler CFLAGS: $POPPLER_CFLAGS])
			AC_MSG_RESULT([   poppler LIBS: $POPPLER_LIBS])
			if test "x$with_staticlink" = "xyes"; then
			   LIBS="-Wl,-Bstatic $POPPLER_LIBS -Wl,-Bdynamic $LIBS"
			else
			   LIBS="$POPPLER_LIBS $LIBS"
			fi
			CPPFLAGS="$POPPLER_CFLAGS $CPPFLAGS"
			LDFLAGS="$POPPLER_LDFLAGS $LDFLAGS"
		   ],
		   [
			HAVE_POPPLER=0
		   ]
		)
	fi
fi

dnl ********************************************************************************
dnl check Qt 4.1.x
dnl ********************************************************************************

QT4VER="?"
QMAKE_PROG="none"
if test "x$with_qt" != "xno"; then
    if test "x$with_qt" = "xyes"; then
        if test "x$PKG_CONFIG" != "x"; then
            QT4LOC=`$PKG_CONFIG QtCore --variable=exec_prefix`
            if test -x $QT4LOC/bin/qmake; then
                if $PKG_CONFIG QtCore --atleast-version 4.1.1; then
                    with_qt=$QT4LOC
                    QT4VER=`$PKG_CONFIG QtCore --modversion`
                fi
            fi
        fi
    fi
    if test "x$with_qt" = "xyes"; then
        if test -x /opt/qt4/bin/qmake; then
           with_qt=/opt/qt4
        fi
        if test -x /usr/lib/qt4/bin/qmake; then
           with_qt=/usr/lib/qt4
        fi
        AC_PATH_PROG([QMAKE_PROG], [qmake-qt4], [none], [$PATH])
    fi
    if test "x$with_qt" != "xyes"; then
       QTDIR=$with_qt
	   if test -x $with_qt/bin/qmake; then
		   QMAKE_PROG=$with_qt/bin/qmake
	   fi
	   if test -x $with_qt/bin/qmake-qt4; then
		   QMAKE_PROG=$with_qt/bin/qmake-qt4
	   fi
	   if test -x $with_qt/bin/qmake.exe; then
		   QMAKE_PROG=$with_qt/bin/qmake.exe
	   fi
	   if test "x$QMAKE_PROG" = "xnone"; then
	      AC_PATH_PROGS([QMAKE_PROG], [qmake-qt4 qmake], [none], [$QTDIR/bin:$PATH])
       fi
    fi
    if test "x$QMAKE_PROG" != "xnone"; then
        HAVE_QT=1
        QMAKE_VERS=`$QMAKE_PROG -v | tail -n 1`
    fi
    
    AC_CHECK_HEADERS(GL/glu.h,
    [AC_DEFINE(HAVE_LIBGLU_H)],
    [])
    
    AC_CHECK_LIB(GLU,gluLookAt,
    [QT_LIBGLU_LIB=-lGLU],
    [])
fi

if test "x$HAVE_RPATH" != "x0"; then
   QT_RPATH="-Wl,-rpath,\\\\\\\$\\\$ORIGIN/../lib"
fi

dnl ********************************************************************************
dnl build libgle stuff
dnl ********************************************************************************

if test "x$HAVE_QT" != "x0"; then
  if test "x$with_libgle" != "xstatic"; then
     if test "x$with_libgle" != "xboth"; then
        with_libgle=yes
     fi
  fi
fi

if test "x$with_libgle" = "xboth"; then
  HAVE_LIBGLE_STATIC=1
  AC_DEFINE(HAVE_LIBGLE_STATIC)
  with_libgle=yes
fi

if test "x$with_libgle" = "xyes"; then
  HAVE_LIBGLE=1
  AC_DEFINE(HAVE_LIBGLE)
  case $host in
     *-*-linux*|*-*-kfreebsd*)
        LIBGLE_CPPFLAGS="-fPIC "
        LIBGLE_LDFLAGS="-fPIC "
        HAVE_LIBGLE_CPPFLAGS=1
     ;;
  esac
fi

if test "x$with_libgle" = "xstatic"; then
  HAVE_LIBGLE_STATIC=1
  AC_DEFINE(HAVE_LIBGLE_STATIC)
fi

dnl ********************************************************************************
dnl check for -ldl
dnl ********************************************************************************

_SAVE_LIBS=$LIBS
_SAVE_LDFLAGS=$LDFLAGS
LIBS=$SIMPLE_LIBS
LDFLAGS=$SIMPLE_LDFLAGS

AC_SEARCH_LIBS(dlopen,dl)
SIMPLE_LIBS_DL=$LIBS

LIBS=$_SAVE_LIBS
LDFLAGS=$_SAVE_LDFLAGS

dnl ********************************************************************************
dnl build manip utility
dnl ********************************************************************************

MAKE_MANIP=0

if test "x$with_manip" != "xno"; then
   _SAVE_LIBS=$LIBS
   _SAVE_LDFLAGS=$LDFLAGS
   LIBS=$SIMPLE_LIBS
   LDFLAGS=$SIMPLE_LDFLAGS
   AC_CHECK_LIB(tinfo,tgetent)
   AC_SEARCH_LIBS(wbkgdset,ncurses curses,
   [
      MAKE_MANIP=1
   ],
   [],[])
   AC_CHECK_HEADER([ncurses.h],
   [
      HAVE_NCURSES_H=1
   ],
   [
      AC_CHECK_HEADER([curses.h],
      [
         HAVE_CURSES_H=1
      ],
      [
         MAKE_MANIP=0
      ])
   ])
   if test "$HAVE_NCURSES_H" = "1"; then
      AC_DEFINE(HAVE_NCURSES_H)
   fi
   if test "$HAVE_CURSES_H" = "1"; then
      AC_DEFINE(HAVE_CURSES_H)
   fi
   MANIP_LIBS=$LIBS
   MANIP_LDFLAGS=$LDFLAGS
   if test "x$with_staticlink" = "xyes"; then
      MANIP_LIBS="$SIMPLE_LIBS -Wl,-Bstatic -lncurses -ltinfo -lm -Wl,-Bdynamic -ldl"
      #MANIP_LIBS="$SIMPLE_LIBS -Wl,-Bstatic -lncurses -lm -Wl,-Bdynamic -ldl"
   else
      MANIP_LIBS="$MANIP_LIBS -lm"
   fi
   LIBS=$_SAVE_LIBS
   LDFLAGS=$_SAVE_LDFLAGS
fi

dnl ********************************************************************************
dnl set CPPFLAGS correctly - NO TESTS INVOLVING CALLING gcc BEYOND THIS POINT
dnl ********************************************************************************

if test "x$with_warn" != "xno"; then
   CPPFLAGS="-Wall $CPPFLAGS"
fi

CPPFLAGS="-Wno-write-strings $CPPFLAGS"

CPPFLAGS="-DHAVE_CONFIG_H \$(VERSION_INFO) $CPPFLAGS"
LDFLAGS="$LDFLAGS $EXTRA_LDFLAGS"
LIBS="$LIBS $EXTRA_LIBS"

dnl ********************************************************************************
dnl autopackage support
dnl ********************************************************************************

MAKE_AUTOPACKAGE=0
if test "x$with_autopackage" != "xno"; then
   MAKE_AUTOPACKAGE=1

   dnl Qt support compiled in?
   if test "x$HAVE_QT" != "x0"; then
      AP_CONFIG_ARGS="$AP_CONFIG_ARGS --with-qt=$QTDIR"
   fi
   if test "$with_png" != yes; then
      AP_CONFIG_ARGS="$AP_CONFIG_ARGS --with-png=$with_png"
   fi

   CPPFLAGS=$_CPPFLAGS_INI
   CXXFLAGS=$_CXXFLAGS_INI
   LDFLAGS=$_LDFLAGS_INI
   LIBS=$_LIBS_INI
fi

if test "x$with_apintern" != "xno"; then
   LDFLAGS="$LDFLAGS -static-libgcc"
   LIBS="$LIBS -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic"
   SIMPLE_LDFLAGS="$SIMPLE_LDFLAGS -static-libgcc"
   SIMPLE_LIBS="$SIMPLE_LIBS -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic"
   MANIP_LDFLAGS="$MANIP_LDFLAGS -static-libgcc"
   MANIP_LIBS="$MANIP_LIBS -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic"
   HAVE_X11=0
   HAVE_AUTOPACKAGE=1
fi

dnl ********************************************************************************
dnl debugging version?
dnl ********************************************************************************

dnl --with-debug=yes, --with-debug=no, --with-debug=gle

if test "x$with_debug" != "xno"; then
   CPPFLAGS="-g $CPPFLAGS"
   LDFLAGS="-g $LDFLAGS"
fi

if test "x$with_debug" = "xyes"; then
   QT_CONFIG="CONFIG += debug"
fi

dnl ********************************************************************************
dnl final substitutes
dnl ********************************************************************************

if test "x$HAVE_X11" != "x0"; then
   AC_DEFINE(HAVE_X11)
fi
if test "x$HAVE_CAIRO" != "x0"; then
   AC_DEFINE(HAVE_CAIRO)
fi
if test "x$HAVE_POPPLER" != "x0"; then
   AC_DEFINE(HAVE_POPPLER)
fi

AC_SUBST(HAVE_SOCKETS)
AC_SUBST(HAVE_LIBTIFF)
AC_SUBST(HAVE_LIBPNG)
AC_SUBST(HAVE_LIBJPEG)
AC_SUBST(HAVE_CAIRO)
AC_SUBST(HAVE_NOSCRIPTS)
AC_SUBST(HAVE_QT)
AC_SUBST(HAVE_LIBGLE)
AC_SUBST(HAVE_LIBGLE_CPPFLAGS)
AC_SUBST(HAVE_LIBGLE_STATIC)
AC_SUBST(HAVE_AUTOPACKAGE)
AC_SUBST(HAVE_MAC)
AC_SUBST(HAVE_FREEBSD)
AC_SUBST(HAVE_WIN32)
AC_SUBST(HAVE_NOCYGWIN)
AC_SUBST(HAVE_CYGWIN)
AC_SUBST(HAVE_MINGW)
AC_SUBST(HAVE_RPATH)
AC_SUBST(QMAKE_PROG)
AC_SUBST(MAKE_MANIP)
AC_SUBST(AP_CONFIG_ARGS)
AC_SUBST(GLE_SNAPSHOT)

if test "$prefix" = "NONE"; then
   GLE_INSTALL_DATA=/usr/share/$PACKAGE_TARNAME/$GLE_VERSION
   prefix="/usr"
else
   GLE_INSTALL_DATA=$prefix/share/$PACKAGE_TARNAME/$GLE_VERSION
fi

if test "$exec_prefix" = "NONE"; then
   if test "$prefix" = "NONE"; then
      GLE_INSTALL_BIN=/usr/bin
   else
      GLE_INSTALL_BIN=$prefix/bin
   fi
else
   GLE_INSTALL_BIN=$exec_prefix
fi

if test "$bindir" != "\${exec_prefix}/bin"; then
   GLE_INSTALL_BIN=$bindir
fi

AC_SUBST(SIMPLE_LIBS)
AC_SUBST(SIMPLE_LIBS_DL)
AC_SUBST(SIMPLE_LDFLAGS)
AC_SUBST(MANIP_LIBS)
AC_SUBST(MANIP_LDFLAGS)
AC_SUBST(LIBGLE_CPPFLAGS)
AC_SUBST(LIBGLE_LDFLAGS)
AC_SUBST(GLE_INSTALL_DATA)
AC_SUBST(GLE_INSTALL_BIN)
AC_SUBST(GLE_VERSION)
AC_SUBST(MAKE_AUTOPACKAGE)
AC_SUBST(QT_CONFIG)
AC_SUBST(QT_RPATH)
AC_SUBST(QT_LIBGLU_LIB)
AC_SUBST(HAVE_EXTRA_FONTS)

AC_CONFIG_FILES([Makefile src/gle/Makefile src/gle/bitmap/Makefile src/gle/letzfitz/Makefile src/gle/surface/Makefile src/gle/tokens/Makefile src/fbuild/Makefile src/makefmt/Makefile src/manip/Makefile src/TeX/Makefile src/gui/MakefileAC src/gui/qgle.pro src/doc/gle.1 platform/autoconf/gle-graphics.pc platform/autopackage/gle4.apspec])
AC_OUTPUT

AC_MSG_RESULT([])

AC_MSG_RESULT([CPPFLAGS: $CPPFLAGS])
AC_MSG_RESULT([LDFLAGS: $LDFLAGS])
AC_MSG_RESULT([LIBS: $LIBS])
AC_MSG_RESULT([SIMPLE_LIBS: $SIMPLE_LIBS])
AC_MSG_RESULT([SIMPLE_LIBS_DL: $SIMPLE_LIBS_DL])
AC_MSG_RESULT([SIMPLE_LDFLAGS: $SIMPLE_LDFLAGS])

AC_MSG_RESULT([
** Configuration summary for GLE $GLE_VERSION$GLE_SNAPSHOT ($host):
])

AC_MSG_RESULT([   ** Bitmap support:])

if test "$HAVE_LIBPNG" = 1; then
  AC_MSG_RESULT([      PNG support: yes])
else
  AC_MSG_RESULT([      PNG support: no])
fi

if test "$HAVE_LIBTIFF" = 1; then
  AC_MSG_RESULT([      TIFF support: yes])
else
  AC_MSG_RESULT([      TIFF support: no])
fi

if test "$HAVE_LIBJPEG" = 1; then
  AC_MSG_RESULT([      JPEG support: yes])
else
  AC_MSG_RESULT([      JPEG support: no])
fi

if test "x$with_staticlink" != "xno"; then
  AC_MSG_RESULT([      Statically link: yes])
else
  AC_MSG_RESULT([      Statically link: no])
fi

AC_MSG_RESULT([])

if test "$HAVE_X11" = 1; then
  AC_MSG_RESULT([   ** X11 preview support: yes])
else
  AC_MSG_RESULT([   ** X11 preview support: no])
fi

if test "$HAVE_CAIRO" = 1; then
  AC_MSG_RESULT([   ** Cairo graphics support: yes])
else
  AC_MSG_RESULT([   ** Cairo graphics support: no])
fi

if test "$HAVE_POPPLER" = 1; then
  AC_MSG_RESULT([   ** Poppler PDF support: yes])
else
  AC_MSG_RESULT([   ** Poppler PDF support: no])
fi

if test "$HAVE_SOCKETS" = 1; then
  AC_MSG_RESULT([   ** Socket support: yes])
else
  AC_MSG_RESULT([   ** Socket support: no])
fi

if test "$MAKE_MANIP" = 1; then
  AC_MSG_RESULT([   ** Include 'manip' utility: yes])
else
  AC_MSG_RESULT([   ** Include 'manip' utility: no])
fi

if test "x$with_debug" != "xno"; then
  AC_MSG_RESULT([   ** Debug mode: on])
else
  AC_MSG_RESULT([   ** Debug mode: off])
fi

AC_MSG_RESULT([])

if test "$HAVE_QT" = 1; then
  if test "x$QT4VER" != "x?"; then
     AC_MSG_RESULT([   ** GLE GUI support: yes (Qt4 version: $QT4VER)])
  else
     AC_MSG_RESULT([   ** GLE GUI support: yes])
     if test "x$QMAKE_VERS" != "xnone"; then
        AC_MSG_RESULT([      $QMAKE_VERS])
     fi
  fi
else
  AC_MSG_RESULT([   ** GLE GUI support: no])
  AC_MSG_RESULT([      (make sure you have Qt >= 4.1.1 devel and use ./configure --with-qt=/path/to/qt.)])
fi

AC_MSG_RESULT([])

AC_MSG_RESULT([** Install data into: $GLE_INSTALL_DATA])
AC_MSG_RESULT([** Install binary into: $GLE_INSTALL_BIN])
AC_MSG_RESULT([   (use ./configure --prefix=dir to change the installation directory.)])

AC_MSG_RESULT([])

if test "x$with_autopackage" != "xno"; then
   AC_MSG_RESULT([GLE $GLE_VERSION$GLE_SNAPSHOT has been configured. Run "make" to create an autopackage for GLE.])
else
   AC_MSG_RESULT([GLE $GLE_VERSION$GLE_SNAPSHOT has been configured. The next step is to run "make". After "make" completes, run "make install" to install GLE.])
fi
