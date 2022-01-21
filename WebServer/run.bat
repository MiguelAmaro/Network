@echo off

setlocal enabledelayedexpansion

set "IP_ADDRESS=%~1"

if "!IP_ADDRESS!" equ "" (set IP_ADDRESS=184.25.233.135)

pushd build

del site.html
main.exe %IP_ADDRESS% > site.html

popd