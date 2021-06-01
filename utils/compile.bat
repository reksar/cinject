@ECHO OFF

SET OUTDIR=build
SET VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC
SET VCVARS=%VCVARS%\Auxiliary\Build\vcvars64.bat

CALL "%VCVARS%"

IF NOT EXIST "%OUTDIR%" (
  MKDIR "%OUTDIR%" 2>NUL
)

cl /MD /W4 /EHsc /std:c++17 /O2 /Fo%OUTDIR%\\ /Fe%OUTDIR%\\ %1
