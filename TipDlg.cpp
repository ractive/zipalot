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

// TipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "TipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog


CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTipDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTipDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Font.CreateFont(-12,0,0,0,400,0,0,0,0,3,2,1,34, "Microsoft Sans Serif");
}


void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTipDlg, CDialog)
	//{{AFX_MSG_MAP(CTipDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipDlg message handlers

BOOL CTipDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	RECT rt;
	GetClientRect(&rt);
	rt.right -= 20;
	m_Rgn1.CreateEllipticRgnIndirect(&rt);
	rt.right += 20;
	m_Rgn2.CreateEllipticRgn(rt.right - 80, rt.bottom - 30, rt.right - 30, rt.bottom);
	m_Rgn3.CreateEllipticRgn(rt.right - 25, rt.bottom - 15, rt.right, rt.bottom);

	m_WindowRgn.CreateEllipticRgnIndirect(&rt);
	m_WindowRgn.CombineRgn(&m_Rgn1, &m_Rgn2, RGN_OR);
	m_WindowRgn.CombineRgn(&m_WindowRgn, &m_Rgn3, RGN_OR);

	SetWindowRgn(m_WindowRgn, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTipDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	dc.SelectObject(m_Font);
	
	RECT rt; 
	GetClientRect(&rt);
	dc.FillSolidRect(&rt, RGB(255, 255, 150));

	CBrush brush;

	brush.CreateSolidBrush(RGB(230, 230, 100));
	dc.FrameRgn(&m_Rgn1, &brush, 4, 4);
	dc.FrameRgn(&m_Rgn2, &brush, 4, 4);
	dc.FrameRgn(&m_Rgn3, &brush, 4, 4);

	brush.Detach();
	brush.CreateSolidBrush(RGB(200, 200, 0));
	dc.FrameRgn(&m_Rgn1, &brush, 2, 2);
	dc.FrameRgn(&m_Rgn2, &brush, 2, 2);
	dc.FrameRgn(&m_Rgn3, &brush, 2, 2);

	brush.Detach();
	brush.CreateSolidBrush(RGB(150, 150, 0));
	dc.FrameRgn(&m_Rgn1, &brush, 1, 1);
	dc.FrameRgn(&m_Rgn2, &brush, 1, 1);
	dc.FrameRgn(&m_Rgn3, &brush, 1, 1);

	m_Rgn1.GetRgnBox(&rt);
	rt.top += 35;
	rt.left += 60;
	rt.right -= 60;
	rt.bottom -= 35;
	dc.DrawText(m_sTipText, &rt, DT_CENTER | DT_END_ELLIPSIS );
}
