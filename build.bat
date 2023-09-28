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

REM Engine
make -f "Makefile.library.mak" %ACTION% TARGET=%TARGET% ASSEMBLY=engine ADDL_INC_FLAGS="-I%VULKAN_SDK%\include"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Game
make -f "Makefile.library.mak" %ACTION% TARGET=%TARGET% ASSEMBLY=game ADDL_INC_FLAGS="-I%VULKAN_SDK%\include -Iengine\src" ADDL_LINK_FLAGS="-lengine"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Dais (Launcher)
make -f "Makefile.executable.mak" %ACTION% TARGET=%TARGET% ASSEMBLY=dais ADDL_INC_FLAGS="-I%VULKAN_SDK%\include -Iengine\src" ADDL_LINK_FLAGS="-lengine"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies %ACTION_STR_PAST% successfully on %PLATFORM% (%TARGET%)."
