@CALL utils\vcvars.bat || EXIT
cl /MD /W4 /EHsc /std:c++17 /O2 /Fo%OUTDIR%\\ /Fe%OUTDIR%\\ %1
