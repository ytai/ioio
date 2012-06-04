#!/bin/sh

# check that we're root
if [ $(whoami) != "root" ]
then
  echo "You must run this script as root. Try running 'su -'"
  exit 1
fi

if [ -z "${JAVA_HOME}" ]
then
  echo "Your JAVA_HOME environment variable must be set"
  exit 1
fi

echo "Installing Cloudhopper RXTX Build to JAVA_HOME=${JAVA_HOME}"
export HWVER=$(uname -i)

if [ $HWVER = "x86_64" ]
then
  # rename to what java uses
  export HWVER="amd64"
fi

echo "Trying to install for hardware type ${HWVER}"

cp RXTXcomm.jar $JAVA_HOME/jre/lib/ext/
if [ "$?" -ne 0 ]; then echo "Copy failed"; exit 1; fi 

cp librxtxSerial.so $JAVA_HOME/jre/lib/$HWVER/
if [ "$?" -ne 0 ]; then echo "Copy failed"; exit 1; fi

cp librxtxParallel.so $JAVA_HOME/jre/lib/$HWVER/
if [ "$?" -ne 0 ]; then echo "Copy failed"; exit 1; fi

echo "Cloudhopper RXTX Build Installed"
