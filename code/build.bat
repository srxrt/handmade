@echo off

mkdir ..\handmade_build
pushd ..\handmade_build

cl -Zi d:\handmade\code\win32_handmade.cpp user32.lib gdi32.lib

popd