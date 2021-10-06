#!/bin/sh -x
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for framework
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export FBSF_HOME=$PWD/

echo Fbsf Base Directory : $FBSF_HOME
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export PATH=$PATH:"$FBSF_HOME/lib:$FBSF_HOME/FbsfFramework/fbsfplugins:$FBSF_HOME/Addons:$FBSF_HOME/Depends"
export LD_LIBRARY_PATH="$FBSF_HOME/lib:$FBSF_HOME/FbsfFramework/fbsfplugins:$FBSF_HOME/Addons:$FBSF_HOME/Depends":$LD_LIBRARY_PATH
