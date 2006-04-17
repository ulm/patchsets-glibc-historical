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

mkdir -p tmp/patches
cp -r ${gver}/*.patch tmp/patches/ || exit 1
cp -r extra tmp/patches/ || exit 1

if [[ -d ${gver}/man ]] ; then
	mkdir tmp/man
	cp -r ${gver}/man/* tmp/man/ || exit 1
fi

find tmp -type d -name CVS -print0 | xargs -0 rm -r

tar -jcf glibc-${gver}-patches-${pver}.tar.bz2 \
	-C tmp patches || exit 1
if [[ -d ${gver}/man ]] ; then
	tar -jcf glibc-${gver}-manpages.tar.bz2 \
		-C tmp man || exit 1
fi
rm -r tmp

du -b *.tar.bz2
