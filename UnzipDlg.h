/*************************************************************************
                     ZipALot
**************************************************************************

Copyright (C) October, 2000 Jean-Pierre Bergamin, james@ractive.ch

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

#if !defined(AFX_UNZIPDLG_H__B4E83EB4_BE46_11D3_8363_0000B4A265C3__INCLUDED_)
#define AFX_UNZIPDLG_H__B4E83EB4_BE46_11D3_8363_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnzipDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CUnzipDlg dialog

#include <afxmt.h>
#include "unzipinfo.h"

class CUnzipDlg : public CDialog
{
// Construction
public:
	CMutex m_Mutex;

	void Kill();
	BOOL CloseIfDone();
	static void ErrorInCurFile(errorCode error, LPCTSTR sFile = NULL);
	void AddCurFile(LPCTSTR sName);
	void CancelStatus();
	void EndStatus();
	BOOL m_bCanClose;
	CString m_sSourceDir;
	CString m_sTargetDir;
	static void UpdateDlg(LPCTSTR sFileName);
	static void SetRange(UINT nRange);
	static void StepIt();
	static void ShowCurZIPFile(LPCTSTR sFileName);
	CUnzipDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Pump();
// Dialog Data
	//{{AFX_DATA(CUnzipDlg)
	enum { IDD = IDD_UNZIPDLG };
	CButton	m_DetailGroup;
	CTreeCtrl	m_FileTree;
	CAnimateCtrl	m_Animation;
	CProgressCtrl	m_Progress;
	BOOL *m_pbCloseWhenDone;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnzipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CUnzipDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnShowDetails();
	//}}AFX_MSG
	afx_msg void OnRange(UINT pParam, long lParam);
	afx_msg void OnCurFile(UINT pParam, long lParam);
	afx_msg void OnCurZIPFile(UINT pParam, long lParam);
	afx_msg void OnStepIt(UINT pParam, long lParam);
	afx_msg void OnError(UINT pParam, long lParam);
	afx_msg void OnSetTarget(UINT pParam, long lParam);
	afx_msg void OnSetSource(UINT pParam, long lParam);

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bInfoShown;
	void ShowDetails(BOOL bShow);
	int m_nFileNo;
	int m_nErrors;
	void DeleteChildren(HTREEITEM hParent);
	int m_nZIPIconIndex;
	int m_nTXTIconIndex;
	int GetIconIndex(const CString &sPath);
	CImageList m_ImageList;
	HTREEITEM m_htiCurFile;
	void AddExtractedFile(LPCTSTR sName);
	UINT m_noOfFiles;
	static CUnzipDlg * m_pPseudoThis;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNZIPDLG_H__B4E83EB4_BE46_11D3_8363_0000B4A265C3__INCLUDED_)
