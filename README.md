# Kivo

A custom programming language written in C.

The project scaffolding is ready for you to implement the language from scratch.

## Layout

```text
include/kivo/      Headers for the lexer, parser, and interpreter
src/              C source files, starting with main.c
tests/            Future tests
examples/         Future programs written in Kivo
docs/             Language design notes and documentation
CMakeLists.txt    C17 executable target and compiler warnings
run.cmd           Build and run with one command on Windows
.vscode/          VS Code IntelliSense, build tasks, and debugger settings
```

The module names are placeholders; there are no predefined APIs or dependencies.

## VS Code on Windows

Open this folder in VS Code. The workspace uses the installed Visual Studio 2026
Build Tools and recommends Microsoft's C/C++ and CMake Tools extensions.
Both extensions are already installed on this machine.

- CMake configures when you open the folder. If asked to select a kit, choose
  the Visual Studio 2026 amd64 kit.
- Press **Ctrl+Shift+B** to configure and build the Debug executable.
- Set a breakpoint, then press **F5** and select **Debug Kivo** to build and debug.
- Press **Ctrl+F5** to build and run without debugging.

The program runs in VS Code's integrated terminal with the project root as its
working directory. Add command-line arguments to `args` in `.vscode/launch.json`.
IntelliSense gets compiler flags and header paths from CMake.

## Build and run from the terminal

From the project root in PowerShell or Command Prompt:

```powershell
.\run.cmd
```

This configures CMake, builds the Debug executable, and runs it from the project
root. Unchanged files are reused; a failed build stops before running the program.
Arguments after the command are forwarded to Kivo, and its exit code is preserved.

To run the existing executable without building, use `.\build\Debug\kivo.exe`.

## Configure

Requires CMake 3.20 or newer and a C17 compiler, such as MSVC, GCC, or Clang.
Run commands from the project root.

```sh
cmake -S . -B build
```

On this Windows machine, CMake can use the installed Visual Studio Build Tools.

## Start coding

Develop your entry point in `src/main.c`. Implement modules in `src/` and place
their headers in `include/kivo/`, using includes such as `"kivo/lexer.h"`.
Add any additional `.c` files to the source list in `CMakeLists.txt`.

Once you have written `main()`, build with:

```sh
cmake --build build --config Debug
```

With the default Visual Studio generator, run from PowerShell with:

```powershell
.\build\Debug\kivo.exe
```

For a single-configuration generator such as Ninja or Unix Makefiles, configure
with `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`. The executable will be
`build/kivo` (or `build/kivo.exe` on Windows).

No tests are configured yet.
