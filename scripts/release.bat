@echo off

:version
set /P VERSION=Version :
if "%VERSION%" == "" exit

set NAMEDDATE=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%
set OUTPUT=theball-%VERSION%-%NAMEDDATE%.7z

title Making %OUTPUT%

echo.
echo Making %OUTPUT%
echo.

7z a "%OUTPUT%" ..\release\* -mx=9 ^
-xr!*.bat -xr!*.lnk -xr!.hg* -xr!*.blend* -xr!*.psd -xr!*.svg -xr!*.flp

:END
echo End of process
pause > nul
