@call utils\config.bat || exit


rem  `shellcode.obj` -> `shellcode.h`


set SHELLCODE_OBJ=%OUTDIR%/shellcode.obj

rem  Intermediate file, that will contain a shellcode represented as a
rem  continuous hexadecimal line (hexline). Will be translated into bytearray
rem  and written to `SHELLCODE_H` later.
set HEXLINE=%OUTDIR%/hexline.txt

rem  Pure shellcode without NASM's shit from the `SHELLCODE_OBJ` will be
rem  written into this file in `xxd -i` format. Also will contain shellcode
rem  size and some address offsets.
set SHELLCODE_H=src/shellcode.h

rem  Bytes per line for chellcode bytearray in `SHELLCODE_H`. Like `xxd -c`.
rem  Now is set for 80 characters alignment.
set DUMPWIDTH=13

rem  The number of characters representing a byte. Used to split a hexline.
set BYTECHARS=2


rem --- `HEXLINE` -------------------------------------------------------------

set HEXDUMP=xxd -ps %SHELLCODE_OBJ%

rem  Join the `HEXDUMP` lines to produce a hexline:
rem  "[useless shit]abcdef[shellcode]fecba[useless shit]".
rem  See the description at the top of the `src\shellcode.asm`.
set JOIN_LINES=tr -d \n

rem  Cut out useless shit and markers from the hexline to get a pure shellcode.
set CUT_SHELLCODE=sed -r 's/.*abcdef(.*)fedcba.*/\1/'

%HEXDUMP% | %JOIN_LINES% | %CUT_SHELLCODE% > %HEXLINE%


rem --- Write shellcode -------------------------------------------------------

rem  Translate the `HEXLINE` into bytearray of `xdd -i` format.
set BYTEARRAY=sed -r 's/([[:xdigit:]]{%BYTECHARS%})/0x\1, /g'
set TRIM_END=sed 's/, $//'

rem  Split the `BYTEARRAY` into lines of `DUMPWIDTH`.
set BYTELINES=sed -r 's/((0x[[:xdigit:]]{%BYTECHARS%}, ){%DUMPWIDTH%})/\1\n/g'

echo #include ^<minwindef.h^> > %SHELLCODE_H%
echo const UCHAR SHELLCODE[] = { >> %SHELLCODE_H%
cat %HEXLINE% | %BYTEARRAY% | %TRIM_END% | %BYTELINES% >> %SHELLCODE_H%
echo }; >> %SHELLCODE_H%


rem --- Write shellcode size --------------------------------------------------

set CHARCOUNT=wc -m %HEXLINE%
for /f %%g in ('%CHARCOUNT%') do (
  set /a SZ_SHELLCODE=%%g / %BYTECHARS%
)

echo const SIZE_T SZ_SHELLCODE = %SZ_SHELLCODE%; >> %SHELLCODE_H%


rem --- Append address offsets relatively to the shellcode --------------------

goto :Offsets

:WriteOffset
set NAME=%~1
set PATTERN=%2
set FIND_SUBSTRING=grep -Ebo %PATTERN% %HEXLINE%
set CUT_OFFSET=cut -d: -f1
for /f %%g in ('%FIND_SUBSTRING% ^| %CUT_OFFSET%') do (
  set /a OFFSET=%%g / %BYTECHARS%
)
echo const SIZE_T %NAME% = %OFFSET%; >> %SHELLCODE_H%
goto :EOF

:Offsets
call :WriteOffset "FMT_ADDRESS_OFFSET" "[a]{16}"
call :WriteOffset "MSG_ADDRESS_OFFSET" "[b]{16}" 
call :WriteOffset "CALL_ADDRESS_OFFSET" "[c]{16}" 


rem  Console log
echo The shellcode:
cat %HEXLINE%
echo.
echo.
echo The shellcode.h has been generated.