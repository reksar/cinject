@CALL utils\config.bat || EXIT

REM Gets this file:
SET SHELLCODE_OBJ=%OUTDIR%/shellcode.obj
REM Then cuts out a pure shellcode without NASM's shit and writes it into this
REM file in `xxd -i` format:
SET SHELLCODE_HEADER=src/shellcode.h

REM Intermediate file.
SET HEXLINE=%OUTDIR%/hexline.txt
REM Like `-c` (columns) argument of `xxd`.
SET DUMPWIDTH=13

REM Write HEXLINE.
SET HEX_DUMP=xxd -ps %SHELLCODE_OBJ%
SET SINGLE_LINE=tr -d \n
SET PURE_SHELLCODE=sed -r 's/.*abcdef(.*)fedcba.*/\1/'
%HEX_DUMP% | %SINGLE_LINE% | %PURE_SHELLCODE% > %HEXLINE%

REM Create SHELLCODE_HEADER.
ECHO #include ^<minwindef.h^> > %SHELLCODE_HEADER%

REM Append shellcode dump in `xdd -i` format.
SET BYTECHARS=2
SET BYTEARRAY=sed -r 's/([[:xdigit:]]{%BYTECHARS%})/0x\1, /g'
SET TRIM_END=sed 's/, $//'
SET BYTELINES=sed -r 's/((0x[[:xdigit:]]{%BYTECHARS%}, ){%DUMPWIDTH%})/\1\n/g'
ECHO const UCHAR SHELLCODE[] = { >> %SHELLCODE_HEADER%
cat %HEXLINE% | %BYTEARRAY% | %TRIM_END% | %BYTELINES% >> %SHELLCODE_HEADER%
ECHO }; >> %SHELLCODE_HEADER%

REM Append SZ_SHELLCODE.
SET CHARCOUNT=wc -m %HEXLINE%
FOR /F %%G IN ('%CHARCOUNT%') DO (
  SET /A SZ_SHELLCODE=%%G / %BYTECHARS%
)
ECHO const SIZE_T SZ_SHELLCODE = %SZ_SHELLCODE%; >> %SHELLCODE_HEADER%

REM Append variables offsets relatively to shellcode dump.
GOTO :Offsets

:WriteOffset
SET NAME=%~1
SET PATTERN=%2
SET FIND_SUBSTRING=grep -Ebo %PATTERN% %HEXLINE%
SET CUT_OFFSET=cut -d: -f1
FOR /F %%G IN ('%FIND_SUBSTRING% ^| %CUT_OFFSET%') DO (
  SET /A OFFSET=%%G / %BYTECHARS%
)
ECHO const SIZE_T %NAME% = %OFFSET%; >> %SHELLCODE_HEADER%
GOTO :EOF

:Offsets
CALL :WriteOffset "FMT_ADDRESS_OFFSET" "[a]{16}"
CALL :WriteOffset "MSG_ADDRESS_OFFSET" "[b]{16}" 
CALL :WriteOffset "CALL_ADDRESS_OFFSET" "[c]{16}" 

ECHO The shellcode:
cat %HEXLINE%
ECHO.
ECHO.
ECHO shellcode.h has been generated.
