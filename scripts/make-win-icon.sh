#!/bin/bash
echo $(magick --version)
ICOFILE="../build/mpc-qt.ico"
mkdir ../build
if [ ! -f $ICOFILE ]; then
    magick ../res/images/icon/mpc-qt.svg -define icon:auto-resize=16,24,32,48,64,72,96,128,256 -antialias -transparent white -compress zip $ICOFILE
fi
if [ -f $ICOFILE ]; then
    echo "$ICOFILE written"
else
    echo "$ICOFILE NOT written"
    exit 1
fi
