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

#if !defined(AFX_WIZPAGE4_H__1240C4F0_4522_429F_88B3_C2FBAC1A0B3D__INCLUDED_)
#define AFX_WIZPAGE4_H__1240C4F0_4522_429F_88B3_C2FBAC1A0B3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizPage4.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizPageAction dialog
#include "WizSheetData.h"
#include "WizPage.h"
#include "TipButton.h"

class CWizPageAction : public CWizPage
{
	DECLARE_DYNCREATE(CWizPageAction)

// Construction
public:
	CWizPageAction();
	~CWizPageAction();

// Dialog Data
	//{{AFX_DATA(CWizPageAction)
	enum { IDD = IDD_WIZ4 };
	CListCtrl	m_ActionCtrl;
	CTipButton m_TipButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizPageAction)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizPageAction)
	virtual BOOL OnInitDialog();
	afx_msg void OnWizPickSource();
	afx_msg void OnClickFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoIt();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString CreateTipText();
	CTipDlg * m_pTipDlg;
	BOOL m_b2ndExtraction;
	BOOL m_b3rdExtraction;

	CString * m_psTempSource;
	CString * m_psTempTarget;
	void CheckForFiles();
	BOOL AddAction(LPCTSTR lpszFileName, LPCTSTR lpszAction);
	void AddListHeaders();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZPAGE4_H__1240C4F0_4522_429F_88B3_C2FBAC1A0B3D__INCLUDED_)
