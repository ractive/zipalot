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

#if !defined(AFX_POSTPROCESSDLG_H__02CA1312_0D46_495B_B206_A15376643167__INCLUDED_)
#define AFX_POSTPROCESSDLG_H__02CA1312_0D46_495B_B206_A15376643167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PostProcessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPostProcessDlg dialog

class CPostProcessDlg : public CDialog
{
// Construction
public:
	BOOL m_bACE;
	BOOL m_bRAR;
	BOOL m_bEXE;
	BOOL m_bInstall;
	CPostProcessDlg(CString sTarget, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPostProcessDlg)
	enum { IDD = IDD_POSTPROCESS };
	CListCtrl	m_ActionCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostProcessDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_sTargetDir;

	// Generated message map functions
	//{{AFX_MSG(CPostProcessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnSelectall();
	afx_msg void OnUnselectall();
	afx_msg void OnInvert();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SwapRows(int lo, int hi);
	BOOL AddAction(LPCTSTR lpszFileName, LPCTSTR lpszAction);
	void AddListHeaders();
	CToolTipCtrl m_ToolTip;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTPROCESSDLG_H__02CA1312_0D46_495B_B206_A15376643167__INCLUDED_)
