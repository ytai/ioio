#!/bin/bash

PROJECTS="libusb libconn libbtstack libadb bootloader device_bootloader app_layer_v1 blink latency_tester"

if [[ -z "$MAKE" ]] && which colormake > /dev/null 2>&1 ; then
   MAKE=colormake
else
   MAKE=make     
fi

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     PRJMAKEFILEGENERATOR=prjMakefilesGenerator.sh;;
    Darwin*)    PRJMAKEFILEGENERATOR=prjMakefilesGenerator.sh;;
    CYGWIN*)    PRJMAKEFILEGENERATOR=prjMakefilesGenerator.sh;;
    //MINGW*)   PRJMAKEFILEGENERATOR=prjMakefilesGenerator.sh;;
    *)          PRJMAKEFILEGENERATOR=prjMakefilesGenerator.bat
esac

echo PRJMAKEFILEGENERATOR=$PRJMAKEFILEGENERATOR

for PROJ in $PROJECTS; do
  echo ===========================================
  echo make "$PROJ"
  echo ===========================================
  if ! $PRJMAKEFILEGENERATOR firmware/"$PROJ"; then echo "WARNING: failed to regenerate Makefiles."; fi
  if ! $MAKE -C firmware/"$PROJ" "$@"; then echo FAILURE ; exit 1 ; fi
done
echo SUCCESS

