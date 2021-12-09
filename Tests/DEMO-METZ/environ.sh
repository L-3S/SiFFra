#!/bin/sh -x

echo toto


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for QT toolkit
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
source ../../QtVersion.sh
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for framework
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd ../../
. ./FbsfEnv.sh $1
cd -
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for Project
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export APP_HOME=$PWD

export PATH="$APP_HOME/lib/$BUILD_MODE":$PATH
export LD_LIBRARY_PATH="$APP_HOME/lib/$BUILD_MODE":$LD_LIBRARY_PATH

qtcreator &

