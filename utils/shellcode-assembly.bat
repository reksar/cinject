@call utils\config.bat || exit
nasm -f win64 -o %OUTDIR%/shellcode.obj src/shellcode.asm