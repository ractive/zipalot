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

#if !defined(AFX_WIZPAGE3_H__A1147DA1_5825_42A8_A552_DA911A16619B__INCLUDED_)
#define AFX_WIZPAGE3_H__A1147DA1_5825_42A8_A552_DA911A16619B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizPage3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWizPageNFO dialog
#include "WizSheetData.h"
#include "WizPage.h"

class CNfoViewer;

class CWizPageNFO : public CWizPage
{
	DECLARE_DYNCREATE(CWizPageNFO)

// Construction
public:
	CWizPageNFO();
	~CWizPageNFO();

// Dialog Data
	//{{AFX_DATA(CWizPageNFO)
	enum { IDD = IDD_WIZ3 };
	CListBox	m_InfoFiles;
	CString	m_sInfoFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizPageNFO)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizPageNFO)
	afx_msg void OnDblclkInfofiles();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CNfoViewer * m_pNfoViewer;
	BOOL HasNFOFile(LPCTSTR sDir);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZPAGE3_H__A1147DA1_5825_42A8_A552_DA911A16619B__INCLUDED_)
