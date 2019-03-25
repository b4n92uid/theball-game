@echo off

:version
set /P VERSION=Version : 
if "%VERSION%" == "" exit

set NAMEDDATE=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%
set OUTPUT=theball-%VERSION%-%NAMEDDATE%-release

title Making %OUTPUT%

echo.
echo Making %OUTPUT%
echo.

makensis /DOUTPUT=%OUTPUT%.exe installer.nsi

:END
echo End of process
pause > nul
