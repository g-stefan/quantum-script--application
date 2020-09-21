#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

ACTION=$1
if [ "$ACTION" = "" ]; then
	ACTION=make
fi

echo "-> $ACTION quantum-script-extension-application"

cmdX(){
	if ! "$@" ; then
		echo "Error: $ACTION"
		exit 1
	fi
}

cmdX file-to-cs --touch=source/quantum-script-extension-application.cpp --file-in=source/quantum-script-extension-application.js --file-out=source/quantum-script-extension-application.src --is-string --name=extensionApplicationSource
cmdX xyo-cc --mode=$ACTION @util/quantum-script-extension-application.static.compile
cmdX xyo-cc --mode=$ACTION @util/quantum-script-extension-application.dynamic.compile
