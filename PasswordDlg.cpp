/*************************************************************************
                     ZipALot
**************************************************************************

Copyright (C) October, 2000 Jean-Pierre Bergamin, james@ractive.ch

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

// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZipALot.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_bHideTyping = TRUE;
	m_sFileName = _T("");
	m_sMessage = _T("");
	m_sPassword = _T("");
	//}}AFX_DATA_INIT
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Check(pDX, IDC_HIDE, m_bHideTyping);
	DDX_Text(pDX, IDC_FILENAME, m_sFileName);
	DDX_Text(pDX, IDC_MESSAGE, m_sMessage);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	ON_BN_CLICKED(IDC_HIDE, OnHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_sZipPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::OnHide() 
{
	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_PASSWORD);
	UpdateData();
	// If we set the password-char to 0 it's not hidden anymore.
	pEdit->SetPasswordChar(m_bHideTyping ? '*' : 0);
	pEdit->SetFocus();
	pEdit->InvalidateRect(NULL);
	pEdit->UpdateWindow();

}
