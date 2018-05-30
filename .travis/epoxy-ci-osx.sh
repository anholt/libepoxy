#!/bin/sh

export SDKROOT=$( xcodebuild -version -sdk macosx Path )
export CPPFLAGS=-I/usr/local/include
export LDFLAGS=-L/usr/local/lib
export OBJC=$CC
export PATH=$HOME/tools:$PATH

srcdir=$( pwd )
builddir=$( mktemp -d build_XXXXXX )

meson ${BUILDOPTS} $builddir $srcdir || exit $?

cd $builddir
ninja || exit $?
meson test || exit $?
cd ..

rm -rf $builddir
