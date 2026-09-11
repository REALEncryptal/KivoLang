# Kivo

A custom programming language written in C.

The project scaffolding is ready for you to implement the language from scratch.

## Layout

```text
include/kivo/      Shared module headers (optional convention)
src/              C source files and local headers, with any subfolders
tests/            C tests named test_*.c, with any subfolders
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
clangd provides code completion and diagnostics; Microsoft's IntelliSense is
disabled for this workspace, while the C/C++ extension still provides debugging.
The root `compile_flags.txt` tells clangd to parse source files and headers as C17
and search `include/` and `src/` for headers. Keep these flags in sync if you
change the language standard, include directories, or add compile definitions in
CMake. CMake continues to control the actual MSVC build.

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

Develop your entry point in `src/main.c`. CMake automatically discovers `.c`
files anywhere under `src/`, including nested folders. Add or remove a file,
then build as usual; there is no source list to update manually.

Headers can live beside their implementations, for example
`src/interpreter/utils/value.h` and `value.c`. Other modules and tests can use
`#include "interpreter/utils/value.h"` because `src/` is an include search path.
Within `value.c`, use `#include "value.h"`.

The `include/` folder is a convention for headers shared with other modules or
users of a library. It is optional: you can keep every header under `src/` if
you prefer. Existing `#include "kivo/lexer.h"` paths still work. Include `.h`
files, not `.c` files. Put reusable functions outside `src/main.c` so tests
can use them without pulling in the application's entry point.

All `.c` files under `src/` belong to the application. Put test programs and
scratch programs with their own `main()` elsewhere. Automatic discovery uses
`GLOB_RECURSE` with `CONFIGURE_DEPENDS`; CMake recommends explicit lists for
maximum generator compatibility, but discovery is convenient for this project.

Build with:

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

## Add and run C tests

CTest is configured. Create a file such as `tests/lexer/test_digits.c`;
the filename must start with `test_` and end with `.c`. Each matching file
becomes a separate test executable and needs its own `int main(void)`.
CMake includes the language's `.c` files from `src/`, except `src/main.c`,
so a test can include your headers and call the real module functions.

For example, **after implementing** a `lexer_is_digit(char)` function in your
lexer and declaring it in `include/kivo/lexer.h`, a test could contain:

```c
#include <stdio.h>
#include "kivo/lexer.h"

int main(void)
{
    if (!lexer_is_digit('0') || !lexer_is_digit('9') ||
        lexer_is_digit('/') || lexer_is_digit(':') || lexer_is_digit('a')) {
        fprintf(stderr, "lexer_is_digit classified a character incorrectly\n");
        return 1;
    }

    return 0;
}
```

That function is an example API, not implemented by the scaffolding. Replace
it with your actual lexer API when writing your tests. Returning `0` passes;
returning a nonzero number fails. Explicit checks also work in Release builds,
where the standard `assert()` macro can be disabled.

From the project root, configure, build, and run:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Use these commands after editing, adding, or removing tests. Configuring first
ensures Visual Studio loads the current set of test executables. CTest runs
built executables; it does not compile them. `run.cmd` builds only the application,
so use the build command above for tests. Tests run with the project root as
their working directory. There are no C test cases yet.

Programs written in Kivo (`.kvo`) are different from these C unit tests. Keep
sample programs in `examples/`; the current `main()` opens `input.kvo`
directly, so choosing other files requires adding command-line input handling.
