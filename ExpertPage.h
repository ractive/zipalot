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

#if !defined(AFX_EXPERTPAGE_H__A2C1843C_4280_4A70_AD5A_69C99F852831__INCLUDED_)
#define AFX_EXPERTPAGE_H__A2C1843C_4280_4A70_AD5A_69C99F852831__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExpertPage.h : header file
//

#include "unzipinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CExpertPage dialog

class CExpertPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CExpertPage)

// Construction
public:
	CString * m_psFilter;
	CString * m_psExtractFilter;
	CString * m_psDontExtractFilter;

	BOOL * m_pbFilter;
	BOOL * m_pbExtractCheck;
	BOOL * m_pbDontExtractCheck;


	CExpertPage();
	~CExpertPage();
	tagUnzipInfo * m_pUnzipInfo;
// Dialog Data
	//{{AFX_DATA(CExpertPage)
	enum { IDD = IDD_EXPERTFILTER };

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CExpertPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	void InitData();
	// Generated message map functions
	//{{AFX_MSG(CExpertPage)
	afx_msg void OnFilterCheck();
	afx_msg void OnExtractFilterCheck();
	afx_msg void OnDontExtractFilterCheck();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CToolTipCtrl m_ToolTip;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPERTPAGE_H__A2C1843C_4280_4A70_AD5A_69C99F852831__INCLUDED_)
