@echo off
setlocal
pushd "%~dp0"
if errorlevel 1 exit /b %errorlevel%

cmake -S . -B build -G "Visual Studio 18 2026" -DBUILD_TESTING=ON
if errorlevel 1 goto finish

cmake --build build --config Debug
if errorlevel 1 goto finish

ctest --test-dir build -C Debug --output-on-failure %*

:finish
set "kivoExitCode=%errorlevel%"
popd
exit /b %kivoExitCode%
