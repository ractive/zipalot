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

#if !defined(AFX_NFOVIEWER_H__073C807C_E854_493C_AB66_5DAD479A4807__INCLUDED_)
#define AFX_NFOVIEWER_H__073C807C_E854_493C_AB66_5DAD479A4807__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NfoViewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNfoViewer window

class CNfoViewer : public CFrameWnd
{
// Construction
public:
	CNfoViewer(LPCTSTR sNFOFile);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNfoViewer)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNfoViewer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNfoViewer)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_lpszFileName;
	BOOL LoadFile(LPCTSTR sFileName);
	COLORREF m_bkColor;
	COLORREF m_textColor;
	void ScrollLine(int nLines);
	void Scroll(int nNewPos);
	HFONT m_hTerminalFont;
	int m_nLineHeight;
	int m_nCurPos;
	SIZE m_TextSize;
	int m_nLines;
	LPTSTR m_lpszNFOContent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NFOVIEWER_H__073C807C_E854_493C_AB66_5DAD479A4807__INCLUDED_)
