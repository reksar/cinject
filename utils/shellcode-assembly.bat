@CALL utils\config.bat || EXIT
nasm -f win64 -o %OUTDIR%/shellcode.obj src/shellcode.asm
