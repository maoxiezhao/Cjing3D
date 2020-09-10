@echo off

set platform=%1
set config=%2
set outputDir=%3

echo ***************************************************
echo start to copy generated shaders
xcopy ".\..\Assets\Shaders" "%outputDir%\Assets\Shaders" 	    /E /I /y
echo ***************************************************