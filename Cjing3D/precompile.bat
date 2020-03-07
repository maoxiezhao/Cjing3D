@echo ***************************************************
@echo start to generate lua binding codes

set platform=%1
set config=%2

start ./../bin/%platform%/%config%/LuaGenerator.exe -bind ./../Cjing3D ./../Cjing3D


@echo ***************************************************