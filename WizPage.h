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

#if !defined(AFX_WIZPAGE_H__CA338AC2_781F_4F8B_9C29_DE6A3BA330FE__INCLUDED_)
#define AFX_WIZPAGE_H__CA338AC2_781F_4F8B_9C29_DE6A3BA330FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizPage.h : header file
//

#include "WizSheetData.h"

/////////////////////////////////////////////////////////////////////////////
// CWizPage dialog

class CWizPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizPage)
	
	typedef enum {WIZ_SOURCE, WIZ_TARGET, WIZ_UNZIP, WIZ_NFO, WIZ_ACTION, WIZ_END, WIZ_ERROR};

// Construction
public:
	tagWizData * GetData();
	void CheckForNewDir();
	void AnalyzeSourceDir();

	CWizPage();
	CWizPage(UINT nIDTemplate);
	~CWizPage();

// Dialog Data
	//{{AFX_DATA(CWizPage)
	//enum { IDD = IDD_ABOUTBOX };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZPAGE_H__CA338AC2_781F_4F8B_9C29_DE6A3BA330FE__INCLUDED_)
