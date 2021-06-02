@CALL utils\vcvars.bat || EXIT

IF NOT EXIST "%OUTDIR%" (
  MKDIR "%OUTDIR%" 2>NUL
)

cl /MD /W4 /EHsc /std:c++17 /O2 /Fo%OUTDIR%\\ /Fe%OUTDIR%\\ %1
