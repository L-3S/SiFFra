#!/bin/sh -x
if [[ "$1" == "debug" ]]
then
	export BUILD_MODE="debug"
elif [[ "$1" == "batch" ]]
then
	export BUILD_MODE="batch"
elif [[ "$1" == "release" ]]
then
	export BUILD_MODE="release"
else
	export BUILD_MODE="release"
fi
echo "$BUILD_MODE"
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Path for framework
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export FBSF_HOME=$PWD/

echo Fbsf Base Directory : $FBSF_HOME
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export PATH=$PATH:"$FBSF_HOME/lib/$BUILD_MODE:$FBSF_HOME/lib/$BUILD_MODE/fbsfplugins:$FBSF_HOME/Addons:$FBSF_HOME/Depends"
export LD_LIBRARY_PATH="$FBSF_HOME/lib/$BUILD_MODE:$FBSF_HOME/lib/$BUILD_MODE/fbsfplugins:$FBSF_HOME/Addons:$FBSF_HOME/Depends":$LD_LIBRARY_PATH
