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

// WizPage1.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "WizPageSource.h"
#include "DirectoryPicker.h"
#include "WizSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizPageSource property page

IMPLEMENT_DYNCREATE(CWizPageSource, CWizPage)

CWizPageSource::CWizPageSource() : CWizPage(CWizPageSource::IDD)
{
	//{{AFX_DATA_INIT(CWizPageSource)
	//}}AFX_DATA_INIT
}

CWizPageSource::~CWizPageSource()
{
}

void CWizPageSource::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizPageSource)
	DDX_Control(pDX, IDC_WIZTIP, m_TipButton);
	//}}AFX_DATA_MAP
	tagWizData * pData = GetData();
	DDX_Text(pDX, IDC_WIZSOURCE, *pData->m_psCurSource);
}


BEGIN_MESSAGE_MAP(CWizPageSource, CPropertyPage)
	//{{AFX_MSG_MAP(CWizPageSource)
	ON_BN_CLICKED(IDC_WIZPICKSOURCE, OnWizPickSource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizPageSource message handlers

void CWizPageSource::OnWizPickSource() 
{
	UpdateData();
	// Open a browse dialog
	tagWizData * pData = GetData();
	CDirectoryPicker picker(m_hWnd, pData->m_psCurSource);
	UpdateData(FALSE);
}

BOOL CWizPageSource::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CHANGEDIR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_WIZPICKSOURCE))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TIP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_TipButton.SetIcon(hIcon);
	}

	CString sTipText = "\r\nI recommend that you put all your\r\ndownloaded files in a separate\r\ndirectory!\r\n\r\nOnly this way the mighty wizard\r\ncan analyze your data...";
	m_TipButton.SetTipText(sTipText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizPageSource::OnSetActive() 
{
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	tagWizData * pData = GetData();
	return CPropertyPage::OnSetActive();
}

LRESULT CWizPageSource::OnWizardNext() 
{
	UpdateData();
	tagWizData * pData = GetData();

	AnalyzeSourceDir();

	if (pData->m_Type != ZIP) {
		pData->m_psCurTarget = pData->m_psCurSource;
		pData->NoExtractDir();
		((CPropertySheet *)GetParent())->SetActivePage(3);
		return 3;
	}
	return CPropertyPage::OnWizardNext();
}
