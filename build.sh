#!/bin/bash
PLATFORM="$1"
ACTION="$2"
TARGET="$3"

set echo off

if [ $ACTION = "all" ] || [ $ACTION = "build" ]
then
   ACTION="all"
   ACTION_STR="Building"
   ACTION_STR_PAST="built"
elif [ $ACTION = "clean" ]
then
   ACTION="clean"
   ACTION_STR="Cleaning"
   ACTION_STR_PAST="cleaned"
else
   echo "Unknown action $ACTION. Aborting" && exit
fi

echo "$ACTION_STR everything on $PLATFORM ($TARGET)..."

# Engine
make -f Makefile.library.mak $ACTION TARGET=$TARGET ASSEMBLY=engine
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

# Sandbox
make -f Makefile.executable.mak $ACTION TARGET=$TARGET ASSEMBLY=sandbox ADDL_INC_FLAGS="-Idais/src" ADDL_LINK_FLAGS="-ldais"
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "All assemblies $ACTION_STR_PAST successfully on $PLATFORM ($TARGET)."