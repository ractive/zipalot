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


#if !defined(AFX_WHATIFDLG_H__A39A4587_82BE_4E63_BE45_961717395DD6__INCLUDED_)
#define AFX_WHATIFDLG_H__A39A4587_82BE_4E63_BE45_961717395DD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WhatIfDlg.h : header file
//

#include "unzipinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CWhatIfDlg dialog

class CWhatIfDlg : public CDialog
{
// Construction
public:
	CWhatIfDlg(tagUnzipInfo * pInfo, CWnd* pParent = NULL);   // standard constructor
	static void InsertFile(CString sName, DWORD dwSize, DWORD dwPackedSize, int nRatio, char cEncrypted, CString sDateTime);

// Dialog Data
	//{{AFX_DATA(CWhatIfDlg)
	enum { IDD = IDD_WHATIFDLG };
	CListCtrl	m_FileList;
	CString	m_sInfo;
	CString	m_sStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhatIfDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWhatIfDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnRead();
	DECLARE_MESSAGE_MAP()
private:
	CString FormatNumber(DWORD dwNumber);
	int m_nEncrypted;
	static CWhatIfDlg * m_pPseudoThis;
	BOOL m_bSortAscending;
	int m_nSortedCol;
	BOOL SortItems(int nCol, BOOL bAscending, int low, int high);
	int CmpItems(CString cstL, CString cstR, int nCol);

	BOOL m_bCanClose;
	static UINT FillFileList(LPVOID lParam);
	tagUnzipInfo * m_pUnzipInfo;
	int m_nFiles;
	DWORD m_dwPackedSize;
	DWORD m_dwUnpackedSize;
	CImageList m_ImageList;
	int GetIconIndex(const CString &sPath);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WHATIFDLG_H__A39A4587_82BE_4E63_BE45_961717395DD6__INCLUDED_)
