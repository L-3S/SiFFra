#!/bin/sh -x
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for QT toolkit
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export QTDIR=/opt/Qt
export QT_VERSION=5.15.2
export QT_COMPILER=gcc_64

echo Make sure that you are using the correct Qt Version :
echo QTDIR=$QTDIR
echo QT_VERSION=$QT_VERSION
echo QT_COMPILER=$QT_COMPILER


export PATH=/$QTDIR/$QT_VERSION/$QT_COMPILER/bin:$QTDIR/Tools/QtCreator/bin:$PATH
export LD_LIBRARY_PATH=/$QTDIR/$QT_VERSION/$QT_COMPILER/lib:$QTDIR/Tools/QtCreator/bin:$LD_LIBRARY_PATH
