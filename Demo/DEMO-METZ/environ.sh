#!/bin/sh -x
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
export QML_IMPORT_PATH="$FBSF_HOME/FbsfFramework":"$FBSF_HOME/FbsfEditors/GraphicEditor":$QML_IMPORT_PATH

export PATH=$PATH:"$PWD/lib"

qtcreator

