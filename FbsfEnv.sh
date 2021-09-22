#!/bin/sh -x
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for framework
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export FBSF_HOME=$PWD
export LD_LIBRARY_PATH="$FBSF_HOME/lib:$FBSF_HOME/FbsfFramework/fbsfplugins:$FBSF_HOME/Addons:$FBSF_HOME/Depends"
echo $FBSF_HOME
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export PATH=$PATH:"$FBSF_HOME/lib:$FBSF_HOME/FbsfFramework/fbsfplugins:$FBSF_HOME/Addons:$FBSF_HOME/Depends"

