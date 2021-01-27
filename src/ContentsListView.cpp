#include "LogFile.h"
#include "ContentsListView.h"
#include "ContentsViewerPlugin.h"

//using namespace Win32xx;

//extern CContentsViewerPlugin thePlugin;
//extern CLogFile* g_pLogFile;
//extern bool g_bDebug;

CContentsListView::CContentsListView()
{
	m_bDebug = false;
	mSzDragItem = new TCHAR[MAX_PATH]; 
	mIsDrag = false;
	//LVS_EX_FULLROWSELECT 表示选择整行
	//TVN_BEGINDRAG
	//LVS_SINGLESEL
}

CContentsListView::~CContentsListView()
{
	delete mSzDragItem;
	//Destroy(); // to be sure GetHwnd() returns NULL
}

LRESULT CContentsListView::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPoint point;
	switch(uMsg)
	{
	case NM_CLICK:
		//case WM_LBUTTONCLICK:
		//OnListViewItemClicked();
		break;

	case WM_CHAR:
		break;

	case WM_KEYUP:
		switch (wParam)
		{
			//case VK_ESCAPE: //m_pDlg->m_edFilter.DirectMessage(uMsg, wParam, lParam);
			case VK_DELETE:
				OnDelContents();
				thePlugin.ReparseCurFile();
				break;

			case VK_UP:
			case VK_DOWN:
				ChangeItem();
				break;
		}
		break;

	case WM_LBUTTONDOWN:
		//point.x = LOWORD(lParam);
		//point.y = LOWORD(lParam);
		//MAKEPOINT;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		OnLMouseDown(wParam, point);
		break;

	case WM_LBUTTONUP:
		//CPoint point;
		//if(mIsDrag)
		//{
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		OnLMouseUp(wParam, point);
		//mIsDrag =false;
		//}
		break;

	case WM_MOUSEMOVE:
		//CPoint point;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		OnMouseMove(wParam, point);
		break;
	}
	return WndProcDefault(uMsg, wParam, lParam);
}

//
void CContentsListView::OnLMouseDown(UINT nFlags, CPoint point)
{
	/*POSITION pos = GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
	while (pos)
	{
	n = m_pCtrl->GetNextSelectedItem(pos); //返回下一个选中的行数(返回值从0开始)
	//做相应操作
	}
	}*/
	//UINT flag;
	//int iItem = HitTest(point, &flag);
	//if (flag & LVHT_ONITEMLABEL)
	int iItem = GetSelectionMark();
	if (iItem >= 0)
	{
		int state = GetItemState(iItem, LVIS_FOCUSED | LVIS_SELECTED);
		if (state & (LVIS_FOCUSED | LVIS_SELECTED))
		{	
			LVITEM lvi = {0};
			TCHAR szItemText[40];

			szItemText[0] = 0;

			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			lvi.iItem = iItem;
			lvi.pszText = szItemText;
			lvi.cchTextMax = sizeof(szItemText)/sizeof(szItemText[0]) - 1;

			GetItem(lvi);

			mMoveLineStart = (int) lvi.lParam;

			//if (mMoveLineStart >= 0)
			//{
			//	thePlugin.GotoLine(mMoveLineStart + 15);
			//	thePlugin.GotoLine(mMoveLineStart);
			//	//MessageBox(szItemText, _T("GotoLine"), MB_OK);
			//}

			//mLastItem = iItem;

			_tcscpy(mSzDragItem, szItemText);

			mMoveLineEnd = GetLineByItem(iItem + 1);;
		}
	}
}
void CContentsListView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(nFlags == MK_LBUTTON)
	{
		HDC hDC=::GetDC(NULL);

		SetRedraw(FALSE);

		//TextOut(hDC, point.x, point.y, mSzDragItem, _ARRAYSIZE(mSzDragItem));
		//int iTextLen  = ::WideCharToMultiByte(CP_ACP, 0, mSzDragItem, -1, NULL, NULL, NULL, NULL);
		int iTextLen = lstrlen(mSzDragItem);
		RECT r =GetWindowRect();
		TextOut(hDC, point.x+r.left+20, point.y+r.top-20, mSzDragItem, iTextLen/2-1);

		SetRedraw(TRUE);

		//::ReleaseDC(this->GetHwnd(), hDC);
		::ReleaseDC(NULL, hDC);
		mIsDrag =true;
	}
	else
		mIsDrag =false;
}

void CContentsListView::OnLMouseUp(UINT nFlags, CPoint point)
{
	UINT flag;
	int nItem = HitTest(point, &flag);

	int line = GetLineByItem(nItem);

	if(mIsDrag)
	{
		tString msg = mSzDragItem;
		msg.append(_T(" will be pulled off before "));
		msg = msg + GetItemText(nItem, 0).c_str();

		if (MessageBox(msg.c_str(), _T("Drag Item"), MB_OKCANCEL) == IDOK)
		{
			thePlugin.CutLines(mMoveLineStart, mMoveLineEnd);
			thePlugin.PasteBeforeLine(line);
			thePlugin.ReparseCurFile();
		}
		
		//}
		this->RedrawWindow();
		mIsDrag = false;

		//thePlugin.GotoLine(mMoveLineStart - mMoveLineEnd + line);
	}

	//mLastItem = nItem;
}

void CContentsListView::ChangeItem()
{
	LOGFUNBGN;
	int nItem = GetSelectionMark();
	int line = GetLineByItem(nItem);
	LOGINFO(_T("Hot Item's line: %d\n"), line);
	thePlugin.GotoLine(line);
	LOGFUNEND;
}

void CContentsListView::OnListViewItemRClicked()
{

}

//CListCtrl::SortItems使用的回调函数
//[IN]pInfo1,pInfo2：传入的是LV_ITEM::lParam，在调用InsertItem(const LVITEM* pItem)时指定
//[IN]nColumn:指定列数（从零开始），即按哪一列排序
int CALLBACK CContentsListView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM ptCLV)
{
	//LOGINFO(_T("Ready to Compare.\n"));

	//LOGINFO(_T("Start to Compare"));
	//LPLVITEM  pData1 = (LPLVITEM)lParam1;
	//LPLVITEM  pData2 = (LPLVITEM)lParam2;

	//int line1 = (int)(((LPLVITEM)lParam1)->lParam);
	//int line2 = (int)(((LPLVITEM)lParam1)->lParam);

	int line1 = (int)lParam1;
	int line2 = (int)lParam2;

	//CContentsListView* pCLV = (CContentsListView*)ptCLV;

	//int line1 = pCLV->GetLineByItem((int)lParam1);
	//int line2 = pCLV->GetLineByItem((int)lParam2);

	//LOGINFO(_T("Line1 : %d\n"), line1);
	//LOGINFO(_T("Line2 : %d\n"), line2);

	if (line1 < line2)
		return -1;
	else if (line1 > line2)
		return 1;
	else return 0;
}

int CContentsListView::AddListViewItem(int nItem, const TCHAR* tszContentName, int line, int level)
{
	LOGFUNBGN;

	//if (_tcslen(szContentName) > MAX_SIZE_CONTENT - 4 ) return -1;

	LOGINFO(_T("%s to be add to Listview %d."), tszContentName, nItem);

	tString tContentName = tszContentName;

	LVITEM lvi = { 0 };

	while (level > 1)
	{
		tContentName.insert(0, _T(" "));
		level--;
	}

	//const int len = sizeof(szContentName)/sizeof(szContentName[0]) - 1 + level - 1;
	//const int len = _tcslen(szContentName) + level;
	int len = tContentName.size();
	//TCHAR* tszContentName = new TCHAR[len + 2];
	/*TCHAR* tszContentName = new TCHAR[MAX_PATH];
	_tcscpy(tszContentName, tContentName.c_str());*/
	LOGINFO(_T("After clased：%s, length：%d\n"), tContentName.c_str(), len);

	lvi.iItem = nItem;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	//lvi.pszText = szContentName;
	//lvi.pszText = tszContentName;
	lvi.pszText = const_cast<TCHAR*>(tContentName.c_str());
	//_tcscpy(lvi.pszText, tContentName.c_str());
	//lvi.cchTextMax = MAX_PATH;
	lvi.cchTextMax = len;
	lvi.lParam = line;
	int n = InsertItem(lvi);

	//int n = m_pContentsLV->InsertItem(nItem, tContentName.c_str());
	//m_pContentsLV->SetItemData()

	TCHAR tszLine[10];
	//wsprintf(ts, _T("%d"), line);
	_stprintf(tszLine, _T("%d"), line);
	SetItemText(n, 1, tszLine);	

	LOGINFO(_T("%s has been add in listview.\n"), tContentName.c_str());

	//delete tszContentName;

	LOGFUNEND;	
	return n;
}

void CContentsListView::FocusLastItem()
{
	int line = thePlugin.GetCurLineNo();
	int item = GetItemByLine(line);
	FocusItem(item);
}

int CContentsListView::GetLineByItem(int iItem)
{

	//TCHAR szItemText[40];

	tString tLine;

	/*LVITEM lvi = { 0 };

	szItemText[0] = 0;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.pszText = szItemText;
	lvi.cchTextMax = 40;

	GetItem(lvi);

	return (int)lvi.lParam;*/

	//ListView_GetItemText(this, iItem, 1, szItemText, 40);
	
	tLine = GetItemText(iItem, 1, 10);

	//return _ttoi(GetItemText(iItem, 1).c_str());

	return _ttoi(tLine.c_str());
}

const TCHAR* CContentsListView::GetContentByItem(int iItem) const
{
	LVITEM lvi = {0};
	TCHAR tszItemText[40];

	tszItemText[0] = 0;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.pszText = tszItemText;
	lvi.cchTextMax = 40;

	GetItem(lvi);

	//LOGINFO(_T("Length of %s: %u\n"), szItemText, _tcslen(szItemText));

	return tszItemText;

	//return GetItemText(iItem, 0).c_str();
}

void CContentsListView::SortItemsByLine()
{
	SortItems((PFNLVCOMPARE)CompareFunc, (DWORD)this);
}
//wait to test
void CContentsListView::OnDelContents()
{
	LOGFUNBGN;

	// int iItem = GetSelectionMark();
	// if (iItem >= 0)
	// {
		// int lineStart = GetLineByItem(iItem);

		// int lineEnd = 0;

		// if (iItem + 1 != GetItemCount())
			// lineEnd = GetLineByItem(iItem + 1);
		// else
			// lineEnd = thePlugin.GetMaxLine();

		// LOGINFO(_T("Item to be deleted: %d"), iItem + 1);
		// LOGINFO(_T("Count of Item: %d"), GetItemCount());
		// LOGINFO(_T("Lines to be deleted from: %d to: %d\n"), lineStart, lineEnd);

		// //for (int i = lineEnd; i >= lineStart; i--)
		// //	thePlugin.DeleteLine(i);
		// thePlugin.CutLines(lineStart, lineEnd);
	// }

	//int nItem = 0;
	//int lineStart = 0, lineEnd = 0;
	//POSITION pos = GetFirstSelectedItemPosition();

	//if(pos != NULL)
	//{
	//	lineStart = GetLineByItem(nItem); 
	//	while(pos)
	//	{
	//		nItem = GetNextSelectedItem(pos);

	//		LOGINFO(_T("Item to be deleted: %d"), nItem + 1)
	//	}
	//}

	int nItem = GetSelectionMark();
	int lineStart = GetLineByItem(nItem);
	int lineEnd = 0;
	nItem = nItem + GetSelectedCount();

	if (nItem != GetItemCount())
		lineEnd = GetLineByItem(nItem);
	else
		lineEnd = thePlugin.GetLineCount();
	
	LOGINFO(_T("Line %d to %d to be deleted！"), lineStart, lineEnd - 1);
	thePlugin.CutLines(lineStart, lineEnd - 1);	
	LOGFUNEND;
}
//wait to test
void CContentsListView::OnCutContents()
{
	LOGFUNBGN;
	int iItem = GetSelectionMark();

	int nLineStart = GetLineByItem(iItem);
	int nLineEnd = nLineStart;
	if (iItem < GetItemCount())
	{ 
		nLineEnd = GetLineByItem(iItem + 1);
	}
	else
	{
		nLineEnd = thePlugin.GetLineCount();
	}

	LOGINFO(_T("Item to be cut: %d"), iItem + 1);
	LOGINFO(_T("Count of Item: %d"), GetItemCount());
	LOGINFO(_T("Lines to be cut from: %d to: %d"), nLineStart, nLineEnd - 1);

	thePlugin.CutLines(nLineStart, nLineEnd - 1);

	LOGFUNEND;
}

void CContentsListView::OnInsetContentsBefore()
{
	LOGFUNBGN;

	int iItem = GetSelectionMark();
	int line = GetLineByItem(iItem);

	thePlugin.PasteBeforeLine(line);
	thePlugin.ReparseCurFile();
	LOGFUNEND;
}
//wait to test
void CContentsListView::OnSelectContents()
{
	LOGFUNBGN;
	
	int iItem = GetSelectionMark();

	int nLineStart = GetLineByItem(iItem);
	int nLineEnd = nLineStart;
	if (iItem < GetItemCount())
	{ 
		nLineEnd = GetLineByItem(iItem + 1);
	}
	else
	{
		nLineEnd = thePlugin.GetLineCount();
	}
	
	thePlugin.SelectLines(nLineStart, nLineEnd);
	
	LOGFUNEND;
}

int CContentsListView::GetItemByLine(int line)
{
	int line1, line2;
	for(int iItem = 0; iItem<= GetItemCount()-1; iItem++)
	{ 
		line1 = GetLineByItem(iItem);

		line2 = GetLineByItem(iItem + 1);

		if(line >= line1 && line < line2)
		{
			return iItem;
		}
	}
	return -1;
}

void CContentsListView::FocusItem(int iItem)
{
	SetItemState(iItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	SetSelectionMark(iItem);
	EnsureVisible((iItem + 15)>GetItemCount() ? iItem : (iItem + 15), false);
}