@CALL utils\config.bat || EXIT
nasm -f win64 -o build/shellcode.obj src/shellcode.asm
