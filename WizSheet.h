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

#if !defined(AFX_WIZSHEET_H__9A63168C_4E44_4EA9_A11F_F90772E1B59E__INCLUDED_)
#define AFX_WIZSHEET_H__9A63168C_4E44_4EA9_A11F_F90772E1B59E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizSheet.h : header file
//

#include "ZipALot.h"	// Added by ClassView
#include "WizSheetData.h"

/////////////////////////////////////////////////////////////////////////////
// CWizSheet

class CWizSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CWizSheet)

// Construction
public:
	CWizSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CWizSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	tagWizData m_Data;

/*	CString m_sInstalledDir;
	CString m_sTempExtractDir;
	CString m_sFileToExtract;*/
	/*CString m_sSource;
	CString m_sTarget;*/
	virtual ~CWizSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZSHEET_H__9A63168C_4E44_4EA9_A11F_F90772E1B59E__INCLUDED_)
