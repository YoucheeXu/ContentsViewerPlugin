::mshta vbscript:msgbox("提示内容1",1,"提示窗口1")(window.close)

%1 %2
ver|find "5.">nul&&goto :st
mshta vbscript:createobject("shell.application").shellexecute("%~s0","goto :st","","runas",1)(window.close)&goto :eof
pause

:st

taskkill /F /IM notepad++.exe


cd /d %~dp0
::copy /y ContentsViewerPlugin.dll %ProgramFiles%\Notepad++\plugins\
REM copy /y ContentsViewerPlugin.dll C:\"Program Files"\"Notepad++"\plugins
copy /y ContentsViewerPlugin.dll C:\"Program Files (x86)"\"Notepad++"\plugins\ContentsViewerPlugin
rem copy /y ContentsViewerPlugin.ini C:\"Program Files"\"Notepad++"\plugins\Config

rem start C:\"Program Files"\"Notepad++"\Notepad++.exe

ping -n 6 127.1 

::pause