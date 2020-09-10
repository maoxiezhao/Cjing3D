@echo off

set platform=%1
set config=%2

echo ***************************************************
echo start to generate lua binding codes
start ./../bin/%platform%/%config%/LuaGenerator.exe -bind ./../Cjing3D ./../Cjing3D
echo ***************************************************