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

#if !defined(AFX_TIPBUTTON_H__F2EFEC19_6877_47D0_8936_061CDEA1CA8D__INCLUDED_)
#define AFX_TIPBUTTON_H__F2EFEC19_6877_47D0_8936_061CDEA1CA8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TipButton.h : header file
//

#include "TipDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTipButton window

class CTipButton : public CButton
{
// Construction
public:
	CTipButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTipText(CString &sText);
	virtual ~CTipButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTipButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CString m_sTipText;
	void CloseTip();
	BOOL ShowTip();
	CTipDlg * m_pTipDlg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPBUTTON_H__F2EFEC19_6877_47D0_8936_061CDEA1CA8D__INCLUDED_)
