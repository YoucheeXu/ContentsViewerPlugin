#!/usr/bin/python3
# encoding: UTF-8
import re
import thePlugin
import theDocument
import theCViewerDlg

def Go():

	doc = thePlugin.fetch_document()

	doc = re.sub(r"^[ 　\t]+", r"　　", doc, flags = re.M)
	doc = re.sub(r" sample.com", r"\r\n", doc)
	doc = re.sub(r"[ 　\t]+$", r"", doc, flags = re.M)

	theDocument.set_document(doc)
	theDocument.merge_paragraphs()
	theDocument.default_replace()

	doc = theDocument.get_document()

	thePlugin.update_document(doc)

	theDocument.parse()
	theDocument.trim_contents()
	theDocument.number_contents()

	# doc = theDocument.get_document()
	# thePlugin.update_document(doc)

	theDocument.headParse()
	theCViewerDlg.update_contentsview()

if __name__ == '__main__':
	# print("脚本开始")
	thePlugin.logInfo("sample Script Start")
	Go()
	thePlugin.logInfo("sample Script End")