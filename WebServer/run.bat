@echo off

setlocal enabledelayedexpansion

set "PARAM1=%~1"

if "!PARAM1!" equ "kill" (goto KILL) else (goto RUN)


:RUN
pushd build
del dashboard.html
main.exe %IP_ADDRESS% > dashboard.html
popd
goto eof

:KILL
taskkill /F /im main.exe
goto eof

:eof
