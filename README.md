## To-Do
* support replace in body or contents
* prompt miss content
* support English contents
* manage shortcut of command
* mark/export miss or empty contents
* github/add in Plugins Admin of Notepad++
* color content
* capture print of python script
* support parse only by CDocument
* rewrite in C++ 11


## Known bugs
* 第一千四十章 ->  第140章
* 十 -> 十
* 脚本替换完无法回原来位置
* it will cause parse again when close file
* it will cause parse again when selection in find result


## v0.6
* prepare command in NPP Menu								OK
  * Python script plugin in plugin							OK
* prompt version in about									OK
* add specific index										OK
* clarify ANSI and Unicode									OK
* get and prompt error of python script						OK

Bug fixed:
* 十 -> 十													OK
* 第一千四十章 -> 第140章									wait to test
* it will count less last line when drag item in listview	wait to test
* it will count less last line when delete item				To-Do


## v0.5
* focus on the cursor corresponding content of viewer		OK
* jump back origin position of editor after action			OK
* shortcut in toolbar of Notepad++							OK
* hint of line number at content							OK
* make toolbar look better									OK

Bug fixed:
* space align doesn't work well								OK
* Merge paragraphs doesn't work								OK
* gotoline doesn't work well								OK
* IDM_SC_INTO_NUMBER doesn't work well						OK


### stdRep.jb
* 删除非法字	√
* 替换拼音字	wait to add
* 全角变半角	√


### Function Map
* 前进按钮								wait to add
* 后退按钮								wait to add
* 脚本按钮								OK
	* 列jb脚本							OK
	* 执行脚本							OK
* Py脚本按钮							OK
	* 列py脚本							OK
	* 执行脚本							OK
* ListView按钮							OK
* TreeView按钮							wait to add
* 刷新按钮
	* 索引目录							OK
	* 第X集、第X部、第X册、第X卷、		OK in Cfg
		第X篇、第X章、第X节、第X话、
		第X回							
	* 分级别							OK
	* 目录更新时，激活原选择目录		a little bug
* 菜单按钮
	* 顶头索引							OK
		^.+
	* 指定索引							OK
	* 导出目录
	* 导入目录
	* 标准化目录
		* 转换目录为半角数字			OK
			* “o”替换“0”				OK
		* 规整目录空格					OK
			* 剔除目录前后空格			OK
			* “章”变后为一个空格		OK
		* 排序目录						wait to add
			* 插入缺失目录
			* 对齐目录数字
			（上）  （1）
			（中）  （2）
			（下）  （3）
		* 删除重复(duplicate)目录		
			* 中间隔一行不行			OK
			* 最后一段不正常			OK
			* 不能删除多重复			wait to add
			* 完成后触发规整段落		wait to add
		* 标记空目录
			* 空段：目录后加“+”        	OK
			* 有“+”后，工作不正常		OK
			* 空段消除：目录后去“+”	  	OK
	* 繁体转简体						wait to add
	* 合并段落							wait to complete
		[^\r\n。！？”：」]\$			段落分开
	* 规整段落							wait to complete
		* 去除空段
		* 标题顶格，非标题缩进两字符

### ListView
* 目录右键菜单	wait to test
	* 剪切	快捷键
	* 复制	快捷键
	* 粘贴	快捷键
* 光标上下可选择对应目录    √
* 拖拽目录		wait to complete
	* 确认            √
	* 在目的地划线
* 允许多选		√
* 删除指定目录内容及标题（Del）		√

## Reference 
[ScintillaDoc](http://www.scintilla.org/ScintillaDoc.html)
[NPP Plugin Development](http://sourceforge.net/apps/mediawiki/notepad-plus/index.php?title=Plugin_Development)