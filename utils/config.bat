@echo off

rem --- Required --------------------------------------------------------------

set BIN_PATH=C:\path\to\git\usr\bin
set NASM_PATH=C:\path\to\nasm

rem --- Optional --------------------------------------------------------------

set OUTDIR=build

rem --- Init ------------------------------------------------------------------

if not exist "%BIN_PATH%" (
  echo Path to Git's usr\bin is invalid: "%BIN_PATH%".
  exit /b 1
)

if not exist "%NASM_PATH%" (
  echo NASM is not found at "%NASM_PATH%".
  exit /b 2
)

set PATH=%PATH%;%BIN_PATH%;%NASM_PATH%

if not exist "%OUTDIR%" (
  mkdir "%OUTDIR%" 2>NUL
)
