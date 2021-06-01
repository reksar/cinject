# Shellcode injection into Windows x64 process

The x64 adaptation of x86 version from 
https://xakep.ru/2015/08/12/code-injection

# Install

Preferably use [Visual Studio Code](https://code.visualstudio.com) as IDE. 

But you can use scripts from `utils` dir to build project manually 
(see `.vscode\tasks.json` also).

## Requirements

* [Git for Windows](https://git-scm.com/download/win)
* [NASM](https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64)
* [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019) 
(minimal requirements are first two checkboxes: **MSVC** and **SDK**)

# Configure

Set valid `BIN_PATH` and `NASM_PATH` at `utils\config.bat`.

Optional: set valid `VCVARS` at `utils\compile.bat`.

# Build

In **VS Code** run build task by pressing `Ctrl+Shift+B`
