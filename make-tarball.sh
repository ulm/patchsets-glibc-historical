#!/bin/bash

gver=$1
pver=$2

if [[ -z ${gver} ]] ; then
	echo "Usage: $0 <glibc ver> [patch ver]"
	exit 1
fi

if [[ ! -d ./${gver} ]] ; then
	echo "Error: ${gver} is not a valid glibc ver"
	exit 1
fi

if [[ -e ${gver}/README.history ]] ; then
	auto_pver=$(head -n 1 ${gver}/README.history | awk '{print $1}')
	if [[ -z ${pver} ]] ; then
		pver=${auto_pver}
		echo "Autoguessing patch ver as '${pver}' from ${gver}/README.history"
	elif [[ ${pver} != ${auto_pver} ]] ; then
		echo "Warning: user patch version (${pver}) does not match ${gver}/README.history"
	fi
elif [[ -z ${pver} ]] ; then
	echo "Error: need a patch version"
	exit 1
fi

rm -rf tmp
rm -f glibc-${gver}-*.tar.bz2

mkdir -p tmp/patches
cp -r ${gver}/*.patch ../README* tmp/patches/ || exit 1
cp -r extra tmp/ || exit 1
cp ${gver}/README* tmp/patches/ 2>/dev/null

if [[ -d ${gver}/man ]] ; then
	mkdir tmp/man
	cp -r ${gver}/man/* tmp/man/ || exit 1
fi

find tmp -type d -name CVS -print0 | xargs -0 rm -r

tar -jcf glibc-${gver}-patches-${pver}.tar.bz2 \
	-C tmp patches extra || exit 1
if [[ -d ${gver}/man ]] ; then
	tar -jcf glibc-${gver}-manpages.tar.bz2 \
		-C tmp man || exit 1
fi
rm -r tmp

du -b *.tar.bz2
