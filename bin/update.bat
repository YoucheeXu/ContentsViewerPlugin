@::mshta vbscript:msgbox("提示内容1",1,"提示窗口1")(window.close)

%1 %2
ver|find "5.">nul&&goto :st
mshta vbscript:createobject("shell.application").shellexecute("%~s0","goto :st","","runas",1)(window.close)&goto :eof
pause

:st

taskkill /F /IM notepad++.exe

:del

set CVPPath=\Notepad++\plugins\config\ContentsViewerPlugin

set cfgPath=%AppData%%CVPPath%

del /f/s/q %cfgPath%\*.*
rd /s/q %cfgPath%\ScriptFiles

cd /d %~dp0

:cp
copy /y ContentsViewerPlugin.dll C:\"Program Files (x86)"\"Notepad++"\plugins\ContentsViewerPlugin
xcopy /y ScriptPlugin\*.dll C:\"Program Files (x86)"\"Notepad++"\plugins\ContentsViewerPlugin\ScriptPlugin

xcopy /y ContentsViewerPlugin\*.ini %cfgPath%
xcopy /e/y/i ContentsViewerPlugin\ScriptFiles\* %cfgPath%\ScriptFiles

REM start C:\"Program Files (x86)"\"Notepad++"\Notepad++.exe

ping -n 6 127.1