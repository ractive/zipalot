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

#if !defined(AFX_SIMPLEPAGE_H__676040D0_172D_46E4_8B8A_C7983CF9A350__INCLUDED_)
#define AFX_SIMPLEPAGE_H__676040D0_172D_46E4_8B8A_C7983CF9A350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimplePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimplePage dialog

#include "unzipinfo.h"
#include "TipButton.h"

class CSimplePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSimplePage)

// Construction
public:
	CString * m_psFilter;
	CString * m_psExtractFilter;
	CString * m_psDontExtractFilter;

	BOOL * m_pbExtractCheck;
	
	CSimplePage();
	~CSimplePage();
	tagUnzipInfo * m_pUnzipInfo;

// Dialog Data
	//{{AFX_DATA(CSimplePage)
	enum { IDD = IDD_SIMPLEFILTER };
	CTipButton	m_WizTip;
	BOOL	m_bDocs;
	BOOL	m_bAce;
	BOOL	m_bRar;
	BOOL	m_bZip;
	BOOL	m_bImages;
	BOOL	m_bMusic;
	BOOL	m_bPrograms;
	BOOL	m_bVideo;
	BOOL	m_bWeb;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSimplePage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSimplePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CToolTipCtrl m_ToolTip;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEPAGE_H__676040D0_172D_46E4_8B8A_C7983CF9A350__INCLUDED_)
