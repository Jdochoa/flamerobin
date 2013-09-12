@echo off

rem $Id$

setlocal

set REVISIONINFOFILE=src/revisioninfo.h

rem execute git log to determine git hash
rem redirect standard error to "nul" in case git isn't installed or this is not a git repository
for /F " usebackq tokens=1,2 delims=*" %%i in (`git log -1 --pretty^=tformat:%%h 2^>nul`) do set GITHASH=%%i
rem @echo git hash "%GITHASH%"

rem extract "#define FR_GIT_HASH \"abcd123\"" line, and take header git hash from it
if exist %REVISIONINFOFILE% for /F " usebackq tokens=2,3 " %%i in (%REVISIONINFOFILE%) do set name=%%i&set value=%%j&call :read-header-info
rem @echo Header git hash "%HEADER_GITHASH%"

if not exist %REVISIONINFOFILE% goto :write_header_file
if "%HEADER_GITHASH%" NEQ "%GITHASH%" goto :write_header_file
goto :EOF


:read-header-info
if "%name%" EQU "FR_GIT_HASH" set HEADER_GITHASH=%value%&
goto :EOF



:write_header_file
@echo // auto-generated by update-revision-info script> %REVISIONINFOFILE%
goto :git_hash

:git_hash
if "%GITHASH%" EQU "" goto :no_git_hash
@echo Writing git hash %GITHASH% to %REVISIONINFOFILE%
@echo #define FR_GIT_HASH "%GITHASH%">> %REVISIONINFOFILE%
goto :EOF

:no_git_hash
@echo Writing no git hash to %REVISIONINFOFILE%
@echo #undef FR_GIT_HASH>> %REVISIONINFOFILE%
goto :EOF



endlocal
goto :EOF
