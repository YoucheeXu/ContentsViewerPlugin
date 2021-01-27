#pragma once
#ifndef CONTENTSLISTVIEW_H
#define CONTENTSLISTVIEW_H

#include "help.h"
#include "..\third-party\Win32xx_860\include\wxx_listview.h"
//#include "win32++/include/frame.h"

#include "resource.h"
using namespace std;
using namespace Win32xx;

class CContentsListView : public CListView
{
public :
	CContentsListView();
	virtual ~CContentsListView();
	
private:
	DISALLOW_COPY_AND_ASSIGN(CContentsListView);

protected:
	//LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) const;
	LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnLMouseDown(UINT nFlags, CPoint point);
	void OnLMouseUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	
	//void OnListViewItemLClicked();
	void OnListViewItemRClicked();

	void ChangeItem();

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM ptCLV);

public:
	//int GetLastLine() const {return mLastLine;};
	int AddListViewItem(int nItem, const TCHAR* tszContentName, int line, int level);
	void FocusLastItem();
	//void SetLastItem(int i) {mLastItem = i;};
	
	int GetLineByItem(int iItem);
	const TCHAR* GetContentByItem(int iItem) const;

	void SortItemsByLine();

	void OnDelContents();
	void OnCutContents();
	void OnInsetContentsBefore();
	void OnSelectContents();

	int GetItemByLine(int line);
	void FocusItem(int iItem);
	
private:
	int mMoveLineStart;
	int mMoveLineEnd;
	TCHAR* mSzDragItem;
	bool mIsDrag;
	bool m_bDebug;
	
protected:

};

#endif //CONTENTSLISTVIEW_H