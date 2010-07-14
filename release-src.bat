@echo off
set OUTPUT=theball-1.0.%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%.7z
title Making %OUTPUT%
if exist "%OUTPUT%" del "%OUTPUT%"
7z a "%OUTPUT%" * -mtc=on -mx=9 -x!versions -x!sourcesold -x!build -xr!doc/html -xr!*.lnk -xr!*.bat -xr!*.tdl -xr!.hg*
echo End of process
pause > nul
