# Kivo

Kivo is a programming language built in C. 

The goal for this project is to learn more about languages and learn more about C itself.

Other than project scaffolding (cmake stuff, docs, etc) and minor pretty print functions, no AI was used to generate code. However, I did use it extensively to research and learn about how languages function.

## Layout

```text
include/kivo/     Shared module headers
src/              C source files and local headers, with any subfolders
tests/            C tests named test_*.c, with any subfolders
examples/         Future programs written in Kivo
docs/             Language design notes and documentation
CMakeLists.txt    C17 executable target and compiler warnings
CMakePresets.json Per-OS CMake settings (Windows and macOS)
run.cmd           Build and run with one command on Windows
run.sh            Build and run with one command on macOS
.vscode/          VS Code IntelliSense, build tasks, and debugger settings
```

## Build and run from the terminal

From the project root in PowerShell or Command Prompt on Windows:

```powershell
.\run.cmd
```

Or from the project root in a macOS terminal:

```sh
./run.sh
```