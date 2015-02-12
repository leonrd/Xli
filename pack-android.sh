#!/bin/sh
set -ex
cd "`dirname "$0"`"

SHA=`git rev-parse HEAD`
DATE=`date +"%y%m%d"`
VERSION="$DATE-${SHA:0:6}"
BASENAME="Xli-$VERSION-Android"

rm -rf lib
if [ "$OSTYPE" = "msys" ]; then
	cmd //c build-android.bat
	"C:\Program Files\7-Zip\7z.exe" a -y "$BASENAME.zip" include lib
else
	sh build-android.sh
	zip -r "$BASENAME.zip" include lib
fi
