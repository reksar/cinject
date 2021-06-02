@CALL utils\config.bat || EXIT
nasm -f win64 -o %SHELLCODE_BIN% src/shellcode.asm
