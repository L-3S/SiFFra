#!/bin/sh -x

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for QT toolkit
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 source ../../QtVersion.sh
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for framework
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd ../../
. ./FbsfEnv.sh
cd -
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for Project
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export APP_HOME=$PWD

export PATH="$APP_HOME/lib":$PATH
export LD_LIBRARY_PATH="$APP_HOME/lib":$LD_LIBRARY_PATH


qtcreator &

