export CPPFLAGS="-I$HOME/staticlibs/include -arch i386 -mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
export CXXFLAGS="-I$HOME/staticlibs/include -arch i386 -mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
export CFLAGS="-I$HOME/staticlibs/include -arch i386 -mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
export LDFLAGS="-L$HOME/staticlibs/lib -arch i386 -mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
export PATH=/Users/macbook/staticlibs/bin/:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/X11/bin:/opt/local/bin
export CC=gcc-4.0
export CXX=g++-4.0
export MACOSX_DEPLOYMENT_TARGET=10.4

# Build QT:
#
# ./configure -prefix $HOME/staticlibs -release -nomake examples -nomake tools -static -system-libpng -no-qt3support -no-xmlpatterns -no-phonon -no-phonon-backend -no-webkit -no-mmx -no-3dnow -no-sse -no-sse2 -qt-zlib -no-gif -no-libtiff -no-libmng -no-libjpeg -no-openssl -no-cups -no-iconv -no-dbus -carbon -arch x86 -nomake demos -no-javascript-jit -no-script -no-scripttools -no-declarative -I /Users/macbook/staticlibs/include -L /Users/macbook/staticlibs/lib
# make sub-src
# make install

# Build libjpeg, libtiff, pixman
#
# ./configure --prefix=$HOME/staticlibs --enable-static=yes --enable-shared=no

# Cairo
#
# ./configure --prefix=$HOME/staticlibs --enable-static=yes --enable-shared=no --enable-quartz=no --enable-ft=no --enable-xlib=no

# libpng should be linked dynamically
# (it is used by both Qt and GLE)
