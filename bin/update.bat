@rem @echo off
@rem mshta vbscript:msgbox("��ʾ����1",1,"��ʾ����1")(window.close)

%1 %2
ver|find "5.">nul&&goto :st
mshta vbscript:createobject("shell.application").shellexecute("%~s0","goto :st","","runas",1)(window.close)&goto :eof
pause

:st

taskkill /F /IM notepad++.exe

@cd /d %~dp0
@REM copy /y ContentsViewerPlugin.dll %ProgramFiles%\Notepad++\plugins\
@REM copy /y ContentsViewerPlugin.dll C:\"Program Files"\"Notepad++"\plugins
copy /y ContentsViewerPlugin.dll "%ProgramFiles(x86)%"\"Notepad++"\plugins\ContentsViewerPlugin

@rem copy /y ContentsViewerPlugin.ini C:\"Program Files"\"Notepad++"\plugins\Config
@rem xcopy /y .\ContentsViewerPlugin %appdata%\"Notepad++"\plugins\Config

@rem ping -n 6 127.1 

@rem start C:\"Program Files"\"Notepad++"\Notepad++.exe

@rem pause