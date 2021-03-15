@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

set ACTION=%1
if "%1" == "" set ACTION=make

echo -^> %ACTION% quantum-script-extension-application

goto StepX
:cmdX
%*
if errorlevel 1 goto cmdXError
goto :eof
:cmdXError
echo "Error: %ACTION%"
exit 1
:StepX

call :cmdX file-to-cs --touch=source/quantum-script-extension-application.cpp --file-in=source/quantum-script-extension-application.js --file-out=source/quantum-script-extension-application.src --is-string --name=extensionApplicationSource
call :cmdX xyo-cc --mode=%ACTION% @build/source/quantum-script-extension-application.static.compile
call :cmdX xyo-cc --mode=%ACTION% @build/source/quantum-script-extension-application.dynamic.compile
