@ECHO OFF

REM --- Required --------------------------------------------------------------

SET BIN_PATH=C:\path\to\git\usr\bin
SET NASM_PATH=C:\path\to\nasm

REM --- Optional --------------------------------------------------------------

SET OUTDIR=build

REM --- Init ------------------------------------------------------------------

IF NOT EXIST "%BIN_PATH%" (
  ECHO Path to Git's usr\bin is invalid: "%BIN_PATH%".
  EXIT /B 1
)

IF NOT EXIST "%NASM_PATH%" (
  ECHO NASM is not found at "%NASM_PATH%".
  EXIT /B 2
)

SET PATH=%PATH%;%BIN_PATH%;%NASM_PATH%

IF NOT EXIST "%OUTDIR%" (
  MKDIR "%OUTDIR%" 2>NUL
)
