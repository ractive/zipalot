/*************************************************************************
                     ZipALot
**************************************************************************

Copyright (C) December, 2000 Jean-Pierre Bergamin, james@ractive.ch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/

#if !defined(AFX_WIZPAGE1_H__E4B90C4E_36C4_47CB_98C1_FB5960FE8374__INCLUDED_)
#define AFX_WIZPAGE1_H__E4B90C4E_36C4_47CB_98C1_FB5960FE8374__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizPageSource dialog
#include "WizSheetData.h"
#include "WizPage.h"
#include "TipButton.h"

class CWizPageSource : public CWizPage
{
	DECLARE_DYNCREATE(CWizPageSource)

// Construction
public:
	CWizPageSource();
	~CWizPageSource();

// Dialog Data
	//{{AFX_DATA(CWizPageSource)
	enum { IDD = IDD_WIZ1 };
	CTipButton m_TipButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizPageSource)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizPageSource)
	afx_msg void OnWizPickSource();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZPAGE1_H__E4B90C4E_36C4_47CB_98C1_FB5960FE8374__INCLUDED_)
