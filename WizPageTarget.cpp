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

// WizPage2.cpp : implementation file
//

#include "stdafx.h"
#include <imagehlp.h>
#include "zipalot.h"
#include "WizPageTarget.h"
#include "DirectoryPicker.h"
#include "Unzip.h"
#include "Unace.h"
#include "Unrar.h"
#include "WizSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizPageTarget property page

extern CString GetSystemMessage(DWORD dwError);

IMPLEMENT_DYNCREATE(CWizPageTarget, CWizPage)

CWizPageTarget::CWizPageTarget() : CWizPage(CWizPageTarget::IDD)
{
	//{{AFX_DATA_INIT(CWizPageTarget)
	m_bUseTemp = FALSE;
	//}}AFX_DATA_INIT

	m_sUnzipped = "unzipped";

//	m_pUnzipDlg = NULL;
}

CWizPageTarget::~CWizPageTarget()
{
}

void CWizPageTarget::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizPageTarget)
	DDX_Check(pDX, IDC_WIZUSETEMP, m_bUseTemp);
	DDX_Control(pDX, IDC_WIZTIP, m_TipButton);
	//}}AFX_DATA_MAP
	tagWizData * pData = GetData();

	DDX_Text(pDX, IDC_WIZTARGET, *pData->m_psCurTarget);
}


BEGIN_MESSAGE_MAP(CWizPageTarget, CPropertyPage)
	//{{AFX_MSG_MAP(CWizPageTarget)
	ON_BN_CLICKED(IDC_WIZUSETEMP, OnWizUseTemp)
	ON_BN_CLICKED(IDC_WIZPICKSOURCE, OnWizPickSource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizPageTarget message handlers

BOOL CWizPageTarget::OnInitDialog() 
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

	CString sTipText = "\r\nThe source directory contains a couple of ZIP files\r\n\r\nThey normally contain another archive.\r\n\r\nAt the end you can delete these temporary\r\ncreated files!";
	m_TipButton.SetTipText(sTipText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWizPageTarget::OnWizUseTemp() 
{
	tagWizData * pData = GetData();

	UpdateData();
	if (m_bUseTemp) {

		GetTempPath(_MAX_PATH, pData->m_psCurTarget->GetBuffer(_MAX_PATH));
		pData->m_psCurTarget->ReleaseBuffer();
		
		CString sTemp = *pData->m_psCurSource;
		sTemp = sTemp.Left(sTemp.GetLength() - 1);
		*pData->m_psCurTarget += sTemp.Right(sTemp.GetLength() - sTemp.ReverseFind('\\') - 1);

		((CButton *)GetDlgItem(IDC_WIZPICKSOURCE))->ModifyStyle(0, WS_DISABLED);
		InvalidateRect(NULL, TRUE);
		UpdateWindow();
	}
	else {
		((CButton *)GetDlgItem(IDC_WIZPICKSOURCE))->ModifyStyle(WS_DISABLED, 0);
		InvalidateRect(NULL, TRUE);
		UpdateWindow();
	}

	((CEdit *)GetDlgItem(IDC_WIZTARGET))->SetReadOnly(m_bUseTemp);


	UpdateData(FALSE);
}

void CWizPageTarget::OnWizPickSource() 
{
	tagWizData * pData = GetData();

	UpdateData();
	// Open a browse dialog
	CDirectoryPicker picker(m_hWnd, pData->m_psCurTarget);
	UpdateData(FALSE);
}


LRESULT CWizPageTarget::OnWizardNext() 
{

	return CPropertyPage::OnWizardNext();
}

BOOL CWizPageTarget::OnSetActive() 
{
	tagWizData * pData = GetData();

	if (pData->m_Type != ZIP) {
		pData->m_psCurTarget = pData->m_psCurSource;
		((CPropertySheet *)GetParent())->SetActivePage(WIZ_NFO);
		return FALSE;
	}

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);

	if (pData->m_psCurSource->Right(1) != "\\") {
		*pData->m_psCurSource += "\\";
	}

	*pData->m_psCurTarget = *pData->m_psCurSource + m_sUnzipped + "\\";
	
	UpdateData(FALSE);

	// Check if we can find archives...
	AnalyzeSourceDir();
	
	if (pData->m_sExtractFilter.IsEmpty()) {
		return FALSE;
	}

	return CPropertyPage::OnSetActive();
}

void CWizPageTarget::NoFilesCancel()
{
	MessageBox("No files where found to extract", "Error", MB_OK | MB_ICONINFORMATION);
	((CPropertySheet *)GetParent())->EndDialog(1);
}
