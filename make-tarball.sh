#!/bin/bash

PN="glibc"
PV=${1%/}
pver=$2

if [[ -z ${PV} ]] ; then
	echo "Usage: $0 <${PN} ver> [patch ver]"
	exit 1
fi

if [[ ! -d ./${PV} ]] ; then
	echo "Error: ${PV} is not a valid ${PN} ver"
	exit 1
fi

if [[ -e ${PV}/README.history ]] ; then
	auto_pver=$(head -n 1 ${PV}/README.history | awk '{print $1}')
	if [[ -z ${pver} ]] ; then
		pver=${auto_pver}
		echo "Autoguessing patch ver as '${pver}' from ${PV}/README.history"
	elif [[ ${pver} != ${auto_pver} ]] ; then
		echo "Warning: user patch version (${pver}) does not match ${PV}/README.history"
	fi
elif [[ -z ${pver} ]] ; then
	echo "Error: need a patch version"
	exit 1
fi

rm -rf tmp
rm -f ${PN}-${PV}-*.tar.bz2

mkdir -p tmp/patches
# copy README.Gentoo.patches
cp ../README* tmp/patches/ || exit 1
if [[ ${PV} = 9999 ]]; then
	echo "Warning: ${PV} patchset includes only extra/"
else
	cp ${PV}/*.patch tmp/patches/ || exit 1
fi
cp -r extra tmp/ || exit 1
cp ${PV}/README* tmp/patches/ 2>/dev/null || exit

if [[ -d ${PV}/man ]] ; then
	mkdir tmp/man
	cp -r ${PV}/man/* tmp/man/ || exit 1
fi

find tmp -type d -name CVS -print0 | xargs -0 rm -r

tar -jcf ${PN}-${PV}-patches-${pver}.tar.bz2 \
	-C tmp patches extra || exit 1
if [[ -d ${PV}/man ]] ; then
	tar -jcf ${PN}-${PV}-manpages.tar.bz2 \
		-C tmp man || exit 1
fi
rm -r tmp

du -b *.tar.bz2
