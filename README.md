# Shellcode injection into Windows x64 process

The x64 adaptation of x86 version from 
https://xakep.ru/2015/08/12/code-injection

# Install

Preferably use [Visual Studio Code](https://code.visualstudio.com) as IDE. 

But you can build the project manually using `utils` and reading the 
`.vscode\tasks.json` file.

## Requirements

* [Git for Windows](https://git-scm.com/download/win)
* [NASM](https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64)
* [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019) 
(first two checkboxes are required: **MSVC** and **SDK**)

# Configure

Set valid `BIN_PATH` and `NASM_PATH` in the `utils\config.bat` file.

If your VS tools version is different from given you must additionally set the 
`VCVARS_LNK` in `utils\config.bat` file.

If your VS installation path is not default, you may change the `compilerPath` 
in `c_cpp_properties.json` file to improve **IntelliSense** in **VS Code**.

# Build

In **VS Code** run build task (`Ctrl+Shift+B`).
