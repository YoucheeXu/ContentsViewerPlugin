// AboutBox.h
#pragma once
#ifndef _ABOUTBOX_H_
#define _ABOUTBOX_H_

#include "..\third_party\Win32xx_860\include\wxx_dialog.h"
#include "resource.h"

class AboutBox: public CDialog
{
	public:
		// constructors and destructor
		AboutBox(): CDialog(IDD_ABOUTBOX) {}
		virtual ~AboutBox(){}

		void OnHelpAbout(const wchar_t* wszVersion)
		{
			if (!IsWindow())
				Create();  // make the AboutBox modeless

			SetDlgItemText(IDC_ABOUTBOX_VERSION, wszVersion);
			SetFocus();     // set the focus on the box
			return;
		}
};

#endif  // _ABOUTBOX_H_
