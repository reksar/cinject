# Shellcode injection into Windows x64 process

The **x64** adaptation of **x86** version from 
[https://xakep.ru/2015/08/12/code-injection](https://xakep.ru/2015/08/12/code-injection)

Tested on **Windows 10** using **VS Build Tools 2022**.


# Install

**Note:** Git submodule is used, so clone with `--recurse-submodules`.

Preferable IDE - [Visual Studio Code](https://code.visualstudio.com)


## Requirements

* [Git for Windows](https://git-scm.com/download/win)
* [NASM](https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64)
* [Visual Studio Build Tools](https://aka.ms/vs/17/release/vs_buildtools.exe)
    - MSVC 143 - VS 2022 C++ x64/x86 Build Tools
    - SDK for Windows 10 (10.0.20348.0)


# Configure

Set valid `BIN_PATH` and `NASM_PATH` in `utils\config.bat`.

If your VS installation path is not default, you may change the `compilerPath` 
in `.vscode\c_cpp_properties.json` to improve **IntelliSense** in **VS Code**.


# Build

In **VS Code** run build task (`Ctrl+Shift+B`).

Or build manually with `utils` using info from `.vscode\tasks.json`.


# Using

Run `build\test.exe` first. You can play with it or restart it at any time, 
but one requirement is that it must be running before you run 
`build\injector.exe`.


## Address offsets

The most important thing - is to find out address offsets for the injector. 
To do this, run the test program with a debugger, e.g. 
[x64dbg](https://x64dbg.com).

First find the memory address of the test program. **Note:** the memory map in 
the debugger might not start with the needed address of the test executable! 
So you need to find the exact starting address of the executable (`test.exe`) 
which should be equal to the `Process.Pointer` that the injector outputs when 
hanldes the test Windows process.

Set breakpoint at the `.text` memory section of the executable. Then you can 
get to the right place just using *step into* / *step over* and find out the 
three needed addresses. **Note:** to calc the `CALL_OFFSET` you need the 
address of the `PrintMessage` that is called before `jmp` to the loop start, 
when the `getchar` is called.

Alternatively, you can find the `.rdata` section in the memory map and check 
its dump to see where the messages and the `fmt` arg value ("%s") are stored. 
Or you may try to find a string references using debugger tools.

The last thing is calculating offsets by subtracting the test program address 
from other found addresses.
