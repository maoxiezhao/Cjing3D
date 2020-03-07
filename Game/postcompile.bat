@echo ***************************************************
@echo start to copy runtime files

set platform=%1
set config=%2
set outputDir=%3

if %platform%==x64 (
	if %config%==Debug (
		xcopy ".\..\lib\optick\%platform%\%config%\OptickCore.dll" %outputDir% /y
	)
)

@echo ***************************************************