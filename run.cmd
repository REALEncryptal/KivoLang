@echo off
setlocal
pushd "%~dp0"
if errorlevel 1 exit /b %errorlevel%

cmake -S . -B build -G "Visual Studio 18 2026"
if errorlevel 1 goto finish

cmake --build build --config Debug --target kivo
if errorlevel 1 goto finish

".\build\Debug\kivo.exe" %*

:finish
set "kivoExitCode=%errorlevel%"
popd
exit /b %kivoExitCode%
