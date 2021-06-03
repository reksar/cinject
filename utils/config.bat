@ECHO OFF

REM --- Required --------------------------------------------------------------

SET BIN_PATH=C:\path\to\git\usr\bin
SET NASM_PATH=C:\path\to\nasm

REM --- Optional --------------------------------------------------------------

REM VS build tools may be installed to a non-default path, so we set the link
REM at the Start menu instead of path to a vcvars bat-file.
SET VCVARS_LNK=%ProgramData%\Microsoft\Windows\Start Menu\Programs
SET VCVARS_LNK=%VCVARS_LNK%\Visual Studio 2019\Visual Studio Tools\VC
SET VCVARS_LNK=%VCVARS_LNK%\x64 Native Tools Command Prompt for VS 2019.lnk

SET OUTDIR=build

REM --- Init ------------------------------------------------------------------

SET PATH=%PATH%;%BIN_PATH%;%NASM_PATH%

IF NOT EXIST "%OUTDIR%" (
  MKDIR "%OUTDIR%" 2>NUL
)
