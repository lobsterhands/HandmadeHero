@echo off

mkdir ..\..\HandmadeHero_build
pushd ..\..\HandmadeHero_build
cl -FC -Zi ..\HandmadeHero\code\win32_handmade.cpp user32.lib gdi32.lib
popd
