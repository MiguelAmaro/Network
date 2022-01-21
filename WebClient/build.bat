@echo off
if not exist build mkdir build
rem REFERENCES:https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=vs-2019

rem COMPILER(MSVC)
rem ============================================================
set PROJECT_DIR=%cd%
set SOURCES= %PROJECT_DIR%\src\main.c

set MSVC_COMMON= -nologo
set MSVC_WARNING= -wd4700
set MSVC_FLAGS= %MSVC_COMMON% -MD -FC -Z7 %MSVC_WARNING%
set MSVC_INCLUDES= -I%PROJECT_DIR%\src\

rem LINKER(MSVC)
rem ============================================================
set LINK_FLAGS= -incremental:no -opt:ref
set LIBS=Ws2_32.lib

rem START BUILD
rem ============================================================
set path=%PROJECT_DIR%\build;%path%

pushd build

rem COMPILE & LINK
cl %MSVC_FLAGS% %MSVC_INCLUDE% %SOURCES% /link %LIBS%

POPD



