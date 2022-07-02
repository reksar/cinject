@call utils\config.bat || exit
@call utils\vscmd\init.bat || exit
cl /MD /W4 /EHsc /std:c++17 /O2 /Fo%OUTDIR%\\ /Fe%OUTDIR%\\ %1