#!/bin/bash

if [[ $# -ne 2 ]] ; then
	echo "Usage: $0 <glibc ver> <patch ver>"
	exit 1
fi
gver=$1
pver=$2

if [[ ! -d ./${gver} ]] ; then
	echo "Error: ${gver} is not a valid binutils ver"
	exit 1
fi

rm -rf tmp
rm -f glibc-${gver}-*.tar.bz2

mkdir -p tmp/patch
cp -r ${gver}/*.patch tmp/patch/ || exit 1
bzip2 tmp/patch/* || exit 1

mkdir tmp/man
cp -r ${gver}/man/* tmp/man/ || exit 1

tar -jcf glibc-${gver}-patches-${pver}.tar.bz2 \
	-C tmp patch || exit 1
tar -jcf glibc-${gver}-manpages.tar.bz2 \
	-C tmp man || exit 1
rm -r tmp

du -b *.tar.bz2
