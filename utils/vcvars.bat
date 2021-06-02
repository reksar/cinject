@CALL utils\config.bat || EXIT

REM This script finds out the VCVARS_LNK's target and calls it to initialize 
REM MS Visual C++ environment variables.

REM Native Windows methods of getting a link's target are shitty: wmic is 
REM deprecated, powershell is lame duck. But we can TYPE "%VCVARS_LNK%" and 
REM find out the needed path in the output.

REM Here is another crap: the target path in TYPE's output looks like 
REM "C : \ ... \ B u i l d \ v c v a r s 6 4 . b a t", where spaces are null 
REM bytes (what the fuck). The cmd prints it pretty, which leads to additional 
REM trubbles during research.

REM We need to find the path to this file:
SET TARGET_FILE=vcvars.*\.bat
REM Sometimes the output may contain several paths and we need to prevent grep
REM greedy matching using regex for illegal path symbols.
SET PATH_ILLEGAL=:
SET PATH_START=.:\\\
SET FIND_PATH=grep -o "%PATH_START%[^%PATH_ILLEGAL%]*%TARGET_FILE%"
SET TRIM_NULLS=tr -d "\000"

FOR /F "delims=" %%G IN ('
  TYPE "%VCVARS_LNK%" ^| %TRIM_NULLS% ^| %FIND_PATH%') DO (
  CALL "%%G"
)
