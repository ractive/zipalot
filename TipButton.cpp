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

// TipButton.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "TipButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipButton

CTipButton::CTipButton()
{
	m_pTipDlg = NULL;
}

CTipButton::~CTipButton()
{
}


BEGIN_MESSAGE_MAP(CTipButton, CButton)
	//{{AFX_MSG_MAP(CTipButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipButton message handlers

void CTipButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetCapture();
	//GetFocus();
		
	CRect rt;
	GetClientRect(&rt);
	if (!rt.PtInRect(point)) {
		CloseTip();
		ReleaseCapture();
	}
	else {
		ShowTip();
	}
	
	CButton::OnMouseMove(nFlags, point);
}

BOOL CTipButton::ShowTip()
{
	if (m_pTipDlg == 0) {
		m_pTipDlg = new CTipDlg();

		m_pTipDlg->m_sTipText = m_sTipText;
		m_pTipDlg->Create(IDD_TIPDLG, this);

		RECT rectButton;
		RECT rectTip;
		m_pTipDlg->GetClientRect(&rectTip);
		GetClientRect(&rectButton);
		ClientToScreen(&rectButton);
		RECT move;
		move.left = rectButton.left - (rectTip.right - rectTip.left);
		move.right = rectButton.left;
		move.bottom = rectButton.bottom;
		move.top = rectButton.bottom - (rectTip.bottom - rectTip.top);
		m_pTipDlg->MoveWindow(&move, TRUE);
	}

	return m_pTipDlg->ShowWindow(SW_SHOWNA);
}

void CTipButton::CloseTip()
{
	m_pTipDlg->EndDialog(0);
	m_pTipDlg->DestroyWindow();
	delete m_pTipDlg;
	m_pTipDlg = NULL;
}

void CTipButton::SetTipText(CString &sText)
{
	m_sTipText = sText;
}


void CTipButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//CButton::OnLButtonDown(nFlags, point);
}

void CTipButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//CButton::OnLButtonDblClk(nFlags, point);
}
