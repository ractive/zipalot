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

// ZipALotDlg.h : header file
//

#if !defined(AFX_ZIPALOTDLG_H__0FB5A9B6_B42F_11D3_8354_0000B4A265C3__INCLUDED_)
#define AFX_ZIPALOTDLG_H__0FB5A9B6_B42F_11D3_8354_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CZipALotDlg dialog

#include "structs.h"
#include "unzver.h"
#include "UnzipDlg.h"
#include "unzipinfo.h"
#include "RegBinding.h"
#include "ExpertPage.h"
#include "SimplePage.h"

typedef int (WINAPI * _USER_FUNCTIONS)(LPUSERFUNCTIONS);

/* Procedure Calls */
void WINAPI ReceiveDllMessage(unsigned long, unsigned long, unsigned,
   unsigned, unsigned, unsigned, unsigned, unsigned,
   char, LPSTR, LPSTR, unsigned long, char);

#define UNZ_DLL_NAME "UNZIP32.DLL\0"

#define DLL_WARNING "Cannot find %s."\
            " The Dll must be in the application directory, the path, "\
            "the Windows directory or the Windows System directory."
#define DLL_VERSION_WARNING "%s has the wrong version number."\
            " Insure that you have the correct dll's installed, and that "\
            "an older dll is not in your path or Windows System directory."


class CZipALotDlg : public CDialog
{
// Construction
public:
	int (__stdcall *Wiz_SingleEntryUnzip)(int, char **, int, char **, LPDCL, LPUSERFUNCTIONS);

	CString m_sDefaultInstallDir;
	CString m_sExtractFilter;
	CString m_sFilter;
	CString m_sDontExtractFilter;
	CString m_sShellDir;

	CString m_sInstallDir;
	CString m_sStartSource;
	CString m_sStartTarget;
	CString m_sUnzipped;

	BOOL m_bFilterSettings;
	BOOL m_bFilter;
	BOOL m_bExtractCheck;
	BOOL m_bDontExtractCheck;
	BOOL m_bTargetLockedStart;

	BOOL DirExists(CString sDir);
	afx_msg void GenTargetName();
	CZipALotDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CZipALotDlg)
	enum { IDD = IDD_ZIPALOT_DIALOG };
	CStatic	m_PropCtrl;
	CComboBox	m_SourceCombo;
	CButton	m_TargetPickerButton;
	CEdit	m_TargetBox;
	CString	m_sSourceDir;
	CString	m_sTargetDir;
	BOOL	m_bOverwrite;
	BOOL	m_bOwnDir;
	BOOL	m_bCaseInsensitive;
	BOOL	m_bOnlyNewer;
	BOOL	m_bRecreateDir;
	BOOL	m_bLockTarget;
	BOOL	m_bCloseWhenDone;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZipALotDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet m_PropSheet;
	CExpertPage m_ExpertPage;
	CSimplePage m_SimplePage;

	void PostProcess();
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CZipALotDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSourcepicker();
	afx_msg void OnTargetpicket();
	afx_msg void OnOK();
	afx_msg void OnClose();
	virtual void OnCancel();
	afx_msg void OnCheckFiles();
	afx_msg void OnFilterButton();
	afx_msg void OnDestroy();
	afx_msg void OnTargetLock();
	afx_msg void OnSourceShow();
	afx_msg void OnTargetShow();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUseTemp();
	afx_msg void OnWarezWizard();
	afx_msg void OnConfig();
	afx_msg void OnSourceDirSelChange();
	afx_msg void OnSourceDirEditChange();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void KillUnzipDlg();
	void UpdateSourceHistory(CString &sPath);
	void FillSourceHistoryCombo();
	tagUnzipInfo m_UnzipInfo;
	void ShowDetails(BOOL bShow);
	BOOL m_bFilterShown;
	void ToolUpdate();
	CToolTipCtrl m_ToolTip;
	CUnzipDlg * m_pUnzipDlg;
	CRegBinding rb;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZIPALOTDLG_H__0FB5A9B6_B42F_11D3_8354_0000B4A265C3__INCLUDED_)
