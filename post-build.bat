@ECHO off

ECHO "Running post build..."

IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "Post build completed successfully."