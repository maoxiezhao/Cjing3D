@echo off

echo 1. Clearn...
call "build\clean.bat"
echo:

echo 2. copying required GameAssets to bin directory...
xcopy "GameAssets" "bin\x64\Debug\GameAssets\" 	    /E /I /y
xcopy "GameAssets" "bin\x64\Release\GameAssets\"	/E /I /y
echo:

echo 3. Generating VS solution...
build\premake5.exe --file=build\premake.lua %*
echo:

echo 4. copying required dll to bin directory...
xcopy "lib\optick\x64\debug\OptickCore.dll" "bin\x64\Debug\" 	/E /I /y
xcopy "lib\optick\x64\release\OptickCore.dll" "bin\x64\Release\" 	/E /I /y
echo: