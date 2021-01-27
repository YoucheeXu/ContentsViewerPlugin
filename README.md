## To-Do

* make toolbar look better
* focus on content of viewer
* shortcut in toolbar of Notepad++
* Python script
* hint of line number at content
* upload to github
* add into Plugins Admin of Notepad++
* color content
* mark/export miss or empty contents
* jump back origin position after action
* Merge paragraphs

## Known bugs

* 第一千四十章 -*  第140章
* space align doesn't work well

## v0.4
### BUG
* 不能处理非ANSI编码文件
* 无法删除最后一个目录    √
* 索引目录时，“花”与“回”分不清
* 目录空格
* 脚本替换完无法回原来位置
* 刷新指示段落

### stdRep.jb
* 删除非法字	√
* 替换拼音字	wait to add
* 全角变半角	√

### Function Map
* 前进按钮
* 后退按钮
* 脚本按钮
	* 列jb脚本						√
	* 执行脚本						√
* Py脚本按钮
	* 列py脚本						√
	* 执行脚本						
* ListView按钮
* TreeView按钮
* 刷新按钮
	* 索引目录
		^第.{0,14}章.+
		^第\\S{0, 14}章.{0, 30}\$
		^\\s+第\\S{0, 14}章.{0, 50}\$    #bug 找不到含“五”的章节
		^[ 　]*第.{0, 14}回.*        #bug “花”与“回”分不清
	* 第X集、第X部、第X册、第X卷(√)、第X篇、第X章(√)、第X节、第X话、第X回(√)
		关键字前允许出现的字符数为：0      √
		X的数目小于7              √
		X包括：半角数字，中文数字，全角数字，中文大写数字    ok
	* 分级别             √
	* 目录更新时，激活原选择目录	√ a little bug
* 菜单按钮
	* 顶头索引	ok
		^.+
	* 导出目录
	* 导入目录
	* 标准化目录
		* 转换目录为半角数字	ok
			* “o”替换“0”			ok
		* 规整目录空格			ok
			* 剔除目录前后空格	ok
			* “章”变后为一个空格	ok
		* 对齐目录数字
		* 排序目录
			插入缺失目录
			（上）  （1）
			（中）  （2）
			（下）  （3）
		* 删除重复(duplicate)目录		
			中间隔一行不行
			最后一段不正常
			不能删多重复
			不能识别空目录
		* 标记空目录
			* 空段：目录后加“+”        	√
			* 有“+”后，工作不正常
			* 最后一段不正常			√
			* 空段消除：目录后去“+”	  
	* 繁体转简体
	* 合并段落
	* 规整段落	wait to test
		* 去除空段
		* 段前缩进两字符

### ListView
* 目录右键菜单	wait to test
	* 剪切	快捷键
	* 复制	快捷键
	* 粘贴	快捷键
* 光标上下可选择对应目录    √
* 拖拽目录
	* 确认            √
	* 在目的地划线
* 允许多选		√
* 删除指定目录内容及标题		√
* 快捷键		√

http://www.scintilla.org/ScintillaDoc.html
http://sourceforge.net/apps/mediawiki/notepad-plus/index.php?title=Plugin_Development