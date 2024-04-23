#!/bin/sh

# https://techbase.kde.org/Development/Tutorials/Localization/i18n_Build_Systems/Outside_KDE_repositories
# the command extractrc is provided by the pacakge kde-dev-scripts

BASEDIR="../"	# root of translatable sources
PROJECT="fastgame"	# project name
BUGADDR="https://github.com/wwmm/fastgame/issues"	# MSGID-Bugs
WDIR=`pwd`		# working dir

echo "Preparing rc files"
cd ${BASEDIR}
# we use simple sorting to make sure the lines do not jump around too much from system to system
find src -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/rc.cpp
echo "Done preparing rc files"

echo "Preparing source files: C++, C, QML..."
# see above on sorting
find src -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' -o -name '*.qml' \
  | sort > ${WDIR}/infiles.list

echo "rc.cpp" >> ${WDIR}/infiles.list

# preparing the desktop file
echo "Preparing the desktop file"
cp -v src/contents/com.github.wwmm.fastgame.desktop.in ${WDIR}/com.github.wwmm.fastgame.desktop
cd ${WDIR}
msgfmt --desktop --template=com.github.wwmm.fastgame.desktop -d ${BASEDIR}/po -o /tmp/teste.desktop
# echo "com.github.wwmm.fastgame.desktop" >> ${WDIR}/infiles.list

# extracting messages on xgettext
cd ${WDIR}
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
	-kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
	--msgid-bugs-address="${BUGADDR}" \
	--files-from=infiles.list -D ${BASEDIR} -D ${WDIR} -o ${BASEDIR}/po/${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }

echo "Done extracting messages"

echo "Merging translations"
cd ${BASEDIR}/po
catalogs=`find . -name '*.po'`
for cat in $catalogs; do
  echo $cat
  msgmerge -o $cat.new $cat ${PROJECT}.pot
  mv $cat.new $cat
done
echo "Done merging translations"

echo "Cleaning up"
cd ${WDIR}
cp -v infiles.list /tmp
rm rcfiles.list
rm infiles.list
rm rc.cpp
rm com.github.wwmm.fastgame.desktop
echo "Done"
