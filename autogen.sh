#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd "$srcdir"

autoreconf -v --install || exit $?
cd "$ORIGDIR" || exit $?

if test -z "$NOCONFIGURE"; then
    exec "$srcdir/configure" "$@"
fi
