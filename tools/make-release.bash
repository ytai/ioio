#!/bin/sh
shopt -s extglob

tempdir=$(mktemp -d)

function makehex {
  tools/merge-hex \
    firmware/bootloader/dist/$1/production/bootloader.production.hex \
    firmware/app_layer_v1/dist/$2/production/app_layer_v1.production.hex \
    > $tempdir/firmware/$1.hex
}

function copysoftware {
  cp -r software/$1 $tempdir/software/$1
  rm -rf $tempdir/software/$1/bin/!(*.apk)
  rm -rf $tempdir/software/$1/gen/*
}


mkdir $tempdir/firmware
mkdir $tempdir/software
mkdir $tempdir/software/applications

makehex SPRK0012 IOIO0011
makehex SPRK0013 IOIO0012
makehex SPRK0014 IOIO0012
makehex SPRK0015 IOIO0012
makehex SPRK0016 IOIO0013

copysoftware IOIOLib
copysoftware IOIOLibAdk
copysoftware applications/IOIOTortureTest
copysoftware applications/HelloIOIO

(cd $tempdir ; zip -qr $1 *)
cp $tempdir/$1.zip release/

