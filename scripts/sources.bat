@echo off

:version
set /P VERSION=Version :
if "%VERSION%" == "" exit

set NAMEDDATE=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%
set OUTPUT=theball-%VERSION%-%NAMEDDATE%-srouces.7z

title Making %OUTPUT%

echo.
echo Making %OUTPUT%
echo.

7z a "%OUTPUT%" ../* -mx=9 ^
-x!build -x!release -x!debug -x!ignore -xr!.hg*

:END
echo End of process
pause > nul
