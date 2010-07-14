@echo off
set OUTPUT=theball-1.0.%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%-release.7z
title Making %OUTPUT%
if exist "%OUTPUT%" del "%OUTPUT%"
7z a "%OUTPUT%" .\dist\Release\MinGW-Windows\* -mtc=on -mx=9 -xr!*.lnk -xr!*.bat -xr!*.tdl -xr!*.psd -xr!*.blend -xr!*.blend1 -xr!*.flp
echo End of process
pause > nul
