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

// NfoViewer.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "NfoViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNfoViewer

CNfoViewer::CNfoViewer(LPCTSTR sNFOFile)
{
	m_lpszFileName = sNFOFile;

	m_bkColor = RGB(0, 0, 0);
	m_textColor = RGB(200, 200, 200);


	LOGFONT lf;
	lf.lfCharSet = OEM_CHARSET;
	lf.lfClipPrecision = 0;
	lf.lfEscapement = 0;
	strcpy(lf.lfFaceName, "Terminal");
	lf.lfHeight = -12;
	lf.lfItalic = 0;
	lf.lfOrientation = 0;
	lf.lfOutPrecision = 0;
	lf.lfPitchAndFamily = 0;
	lf.lfQuality = 0;
	lf.lfStrikeOut = 0;
	lf.lfUnderline = 0;
	lf.lfWeight = 400;
	lf.lfWidth = 0;

	m_hTerminalFont = CreateFontIndirect(&lf);

	//m_Font.CreateFont(-12,0,0,0,400,0,0,0,255,1,2,1,49, "Terminal");
	
	m_lpszNFOContent = NULL;
	m_nLines = 0;
	m_nCurPos = 0;
	m_TextSize = CSize(0, 0);
}

CNfoViewer::~CNfoViewer()
{
	if (m_lpszNFOContent != NULL) {
		delete [] m_lpszNFOContent;
	}
}


BEGIN_MESSAGE_MAP(CNfoViewer, CWnd)
	//{{AFX_MSG_MAP(CNfoViewer)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNfoViewer message handlers

void CNfoViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.SelectObject(m_hTerminalFont);
	//dc.SelectObject(&m_Font);

	RECT rt;
	GetClientRect(&rt);
	dc.FillSolidRect(&rt, m_bkColor);

	rt.left = 0;
	rt.right = m_TextSize.cx;
	rt.top = 0 - m_nCurPos;
	rt.bottom = rt.bottom;

	dc.SetTextColor(m_textColor);

	
	dc.DrawText(m_lpszNFOContent, -1, &rt, DT_LEFT);
}

int CNfoViewer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!LoadFile(m_lpszFileName)) {
		MessageBox("File could not be loaded!", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	CDC * pDC = GetDC();

	pDC->SelectObject(m_hTerminalFont);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	// count the lines and calculate the
	// the extent of the biggest line
	LPTSTR s = (LPTSTR)m_lpszNFOContent;
	LPTSTR sTemp = NULL;
	int nMaxWidth = 0;
	while (sTemp = strpbrk(s, "\n")) {
		int chars = sTemp - s;
		nMaxWidth = __max(nMaxWidth, chars * tm.tmAveCharWidth);
		s = sTemp + 1;
		m_nLines++;
	}

	m_nLines++;
	nMaxWidth++;

	m_nLineHeight = tm.tmHeight + tm.tmExternalLeading;
	m_TextSize.cx = nMaxWidth;
	m_TextSize.cy = m_nLineHeight * m_nLines;


	// Size the window so that the biggest line fits in
	RECT wRect, cRect;
	GetWindowRect(&wRect);
	GetClientRect(&cRect);
	int yDiff = (wRect.bottom - wRect.top) - (cRect.bottom - cRect.top);
	wRect.right = (wRect.right - wRect.left) - (cRect.right - cRect.left) + nMaxWidth;

	int nMaxHeight = GetSystemMetrics(SM_CYMAXIMIZED);
	if ((m_TextSize.cy + yDiff) < nMaxHeight) {
		wRect.bottom = m_TextSize.cy + yDiff;
	}

	MoveWindow(&wRect);

	//SetScrollRange(SB_VERT, 0, m_TextSize.cy - (nMaxHeight - yDiff), FALSE);

	pDC->SetBkMode(TRANSPARENT);
	
	CenterWindow(GetParent());

	return 0;
}

void CNfoViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int nNewPos = 0;

	switch (nSBCode) {
	case SB_BOTTOM:
		break;
	case SB_ENDSCROLL:
		nNewPos = m_nCurPos;
		break;
	case SB_LINEDOWN:
		ScrollLine(1);
		break;
	case SB_LINEUP:
		ScrollLine(-1);
		break;
	case SB_PAGEDOWN:
		ScrollLine(10);
		break;
	case SB_PAGEUP:
		ScrollLine(-10);
		break;
	case SB_THUMBPOSITION:
		nNewPos = nPos;
		Scroll(nNewPos);
		break;
	case SB_THUMBTRACK: 
		nNewPos = nPos;
		Scroll(nNewPos);
		break;
	case SB_TOP:
		break;
	default:
		break;

	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CNfoViewer::OnSize(UINT nType, int cx, int cy) 
{
	if (cy > m_TextSize.cy && m_TextSize.cy > 0) {
		RECT wRect;
		GetWindowRect(&wRect);
		int xDiff = wRect.right - wRect.left - cx;
		int yDiff = wRect.bottom - wRect.top - cy;
		cy = m_TextSize.cy;
		SetWindowPos(NULL, 0, 0, cx + xDiff, cy + yDiff, SWP_NOMOVE | SWP_NOZORDER);
	}
	else {
		SetScrollRange(SB_VERT, 0, m_TextSize.cy - cy, FALSE);
		if (m_TextSize.cy > 0 && m_nCurPos > m_TextSize.cy - cy) {
			SetScrollPos(SB_VERT, m_TextSize.cy - cy);
			m_nCurPos = m_TextSize.cy - cy;
			InvalidateRect(NULL);
			UpdateWindow();
		}
	}

	CWnd::OnSize(nType, cx, cy);
}

void CNfoViewer::Scroll(int nNewPos)
{
	RECT rt;
	GetClientRect(&rt);

	if (nNewPos < 0) {
		nNewPos = 0;
	}

	if (nNewPos > m_TextSize.cy - rt.bottom) {
		nNewPos = m_TextSize.cy - rt.bottom;
	}


	if (!(m_nCurPos == nNewPos)) {
		m_nCurPos = nNewPos;
		SetScrollPos(SB_VERT, m_nCurPos);
	
		InvalidateRect(NULL, FALSE);
		UpdateWindow();
	}
}

BOOL CNfoViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	Scroll(m_nCurPos - zDelta);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CNfoViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
       case VK_UP:
		   ScrollLine(-1);
		   break;
	   case VK_DOWN:
		   ScrollLine(1);
		   break;
	   case VK_PRIOR:
		   ScrollLine(-10);
		   break;
	   case VK_NEXT:
		   ScrollLine(10);
		   break;
	   default:
		   break;
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CNfoViewer::ScrollLine(int nLines)
{
	Scroll(m_nCurPos + m_nLineHeight * nLines);
}

BOOL CNfoViewer::OnEraseBkgnd(CDC* pDC) 
{
	RECT rt;
	GetClientRect(&rt);
	pDC->FillSolidRect(&rt, m_bkColor);
	
	return CWnd::OnEraseBkgnd(pDC);
}


BOOL CNfoViewer::LoadFile(LPCTSTR sFileName)
{
	CFile file;
	if (!file.Open(sFileName, CFile::modeRead)) {
		return FALSE;
	}

	DWORD dwFileSize = file.GetLength();

	m_lpszNFOContent = new TCHAR[dwFileSize + 1];
	memset(m_lpszNFOContent, 0, dwFileSize + 1);

	UINT nRead = file.Read(m_lpszNFOContent, dwFileSize);

	file.Close();

	return nRead == dwFileSize;
}


