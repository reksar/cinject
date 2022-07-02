# Shellcode injection into Windows x64 process

The **x64** adaptation of **x86** version from 
[https://xakep.ru/2015/08/12/code-injection](https://xakep.ru/2015/08/12/code-injection)

Tested on Windows 10 with VS Build Tools 2022.

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
