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

#if !defined(AFX_WIZUNZIP_H__B0CBA0DF_8E4C_4DE7_849D_AEC92F74A273__INCLUDED_)
#define AFX_WIZUNZIP_H__B0CBA0DF_8E4C_4DE7_849D_AEC92F74A273__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizUnzip.h : header file
//

#include "unzipinfo.h"
#include "WizSheetData.h"
#include "WizPage.h"

/////////////////////////////////////////////////////////////////////////////
// CWizUnzip dialog

class CWizUnzip : public CWizPage
{
	DECLARE_DYNCREATE(CWizUnzip)

// Construction
public:
	tagUnzipInfo m_UnzipInfo;

	CWizUnzip();
	~CWizUnzip();

// Dialog Data
	//{{AFX_DATA(CWizUnzip)
	enum { IDD = IDD_WIZUNZIP };
	CProgressCtrl	m_Progress;
	CAnimateCtrl	m_Animation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizUnzip)
	public:
	virtual BOOL OnSetActive();
	virtual void OnCancel();
	virtual BOOL OnQueryCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizUnzip)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnRange(UINT pParam, long lParam);
	afx_msg void OnCurFile(UINT pParam, long lParam);
	afx_msg void OnCurZIPFile(UINT pParam, long lParam);
	afx_msg void OnStepIt(UINT pParam, long lParam);
	afx_msg void OnError(UINT pParam, long lParam);
	afx_msg void OnSetSource(UINT pParam, long lParam);
	afx_msg void OnSetTarget(UINT pParam, long lParam);

	afx_msg void DoExtraction(UINT pParam, long lParam);

	DECLARE_MESSAGE_MAP()
private:
	LPCTSTR GetFileName(LPTSTR sFilePath);

	//archiveType m_Type;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZUNZIP_H__B0CBA0DF_8E4C_4DE7_849D_AEC92F74A273__INCLUDED_)
