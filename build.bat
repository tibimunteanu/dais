@ECHO OFF

SET PLATFORM=%1
SET ACTION=%2
SET TARGET=%3

if "%ACTION%" == "build" (
    SET ACTION=all
    SET ACTION_STR=Building
    SET ACTION_STR_PAST=built
) else (
    if "%ACTION%" == "clean" (
        SET ACTION=clean
        SET ACTION_STR=Cleaning
        SET ACTION_STR_PAST=cleaned
    ) else (
        ECHO "Unknown action %ACTION%. Aborting" && EXIT
    )
)

del bin\*.pdb

ECHO "%ACTION_STR% everything on %PLATFORM% (%TARGET%)..."

REM Dais
make -f "Makefile.library.mak" %ACTION% TARGET=%TARGET% ASSEMBLY=dais ADDL_INC_FLAGS="-I%VULKAN_SDK%\include"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Sandbox
make -f "Makefile.executable.mak" %ACTION% TARGET=%TARGET% ASSEMBLY=sandbox ADDL_INC_FLAGS="-I%VULKAN_SDK%\include -Idais\src" ADDL_LINK_FLAGS="-ldais"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies %ACTION_STR_PAST% successfully on %PLATFORM% (%TARGET%)."
