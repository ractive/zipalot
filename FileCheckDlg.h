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


#if !defined(AFX_FILECHECKDLG_H__6C3231C0_E81E_4D83_9B4C_1F97906C7EDB__INCLUDED_)
#define AFX_FILECHECKDLG_H__6C3231C0_E81E_4D83_9B4C_1F97906C7EDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileCheckDlg.h : header file
//

#include "unzipinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CFileCheckDlg dialog

class CFileCheckDlg : public CDialog
{
// Construction
public:
	static int m_nEncrypted;
	void ShowZIPDetails();
	void HideZIPDetails();
	static void InsertFile(CString sName, DWORD dwSize, DWORD dwPackedSize, int nRatio, char cEncrypted, CString sDateTime);
	BOOL m_bSortAscending;

	CFileCheckDlg(tagUnzipInfo * pInfo, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileCheckDlg)
	enum { IDD = IDD_FILECHECK };
	CButton	m_DetailGroup;
	static CListCtrl	m_ZIPDetail;
	CListCtrl	m_FileList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileCheckDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnItemChangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFullList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bSorting;
	int nLastProcessed;
	void AttachSystemImageList();
	CString m_sFilter;
	tagUnzipInfo * m_pUnzipInfo;
	static DWORD m_dwPackedSize;
	static DWORD m_dwUnpackedSize;
	static int m_nFiles;
	static int GetIconIndex(const CString &sPath);
	BOOL m_bInfoShown;
	void AddZIPFileHeaders();
	int FillFileList(DWORD * pdwTotalSize);
	void AddFileListHeaders();
	CString FormatNumber(DWORD dwNumber);
	CImageList m_ImageList;
	BOOL SortItems(int nCol, BOOL bAscending, int low, int high);
	int CmpItems(CString cstL, CString cstR, int nCol);
	int m_nSortedCol;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILECHECKDLG_H__6C3231C0_E81E_4D83_9B4C_1F97906C7EDB__INCLUDED_)
