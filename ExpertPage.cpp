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

// ExpertPage.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "ExpertPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExpertPage property page

IMPLEMENT_DYNCREATE(CExpertPage, CPropertyPage)

CExpertPage::CExpertPage() : CPropertyPage(CExpertPage::IDD)
{
	//{{AFX_DATA_INIT(CExpertPage)
	//}}AFX_DATA_INIT
}

CExpertPage::~CExpertPage()
{
}

void CExpertPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpertPage)

	//}}AFX_DATA_MAP

	if (((CPropertySheet *)GetParent())->GetActiveIndex() != 1) {
		// Don't store any data if this is not the active page
		return;
	}

	DDX_Check(pDX, IDC_EXTRACTFILTERCHECK, *m_pbExtractCheck);
	DDX_Check(pDX, IDC_DONTEXTRACTFILTERCHECK, *m_pbDontExtractCheck);
	DDX_Check(pDX, IDC_CASEINSENSITIV, m_pUnzipInfo->bCaseInsensitive);

	DDX_Text(pDX, IDC_FILTER, *m_psFilter);
	DDX_Text(pDX, IDC_EXTRACTFILTER, *m_psExtractFilter);
	DDX_Text(pDX, IDC_DONTEXTRACTFILTER, *m_psDontExtractFilter);


	DDX_Check(pDX, IDC_ONLYNEWER, m_pUnzipInfo->bOnlyNewer);
	DDX_Check(pDX, IDC_OVERWRITE, m_pUnzipInfo->bOverwrite);
	DDX_Check(pDX, IDC_OWNDIR, m_pUnzipInfo->bOwnDir);
	DDX_Check(pDX, IDC_RECREATEDIR, m_pUnzipInfo->bRecreateDir);
/*  Original
	DDX_Check(pDX, IDC_CASEINSENSITIV, m_bCaseInsensitive);
	DDX_Text(pDX, IDC_DONTEXTRACTFILTER, m_sDontExFilter);
	DDX_Text(pDX, IDC_EXTRACTFILTER, m_sExFilter);
	DDX_Text(pDX, IDC_FILTER, m_sFilter);
	DDX_Check(pDX, IDC_ONLYNEWER, m_bOnlyNewer);
	DDX_Check(pDX, IDC_OVERWRITE, m_mOverwrite);
	DDX_Check(pDX, IDC_OWNDIR, m_bCreateOwnDir);
	DDX_Check(pDX, IDC_RECREATEDIR, m_bRecreateDir);
*/
}


BEGIN_MESSAGE_MAP(CExpertPage, CPropertyPage)
	//{{AFX_MSG_MAP(CExpertPage)
	ON_BN_CLICKED(IDC_EXTRACTFILTERCHECK, OnExtractFilterCheck)
	ON_BN_CLICKED(IDC_DONTEXTRACTFILTERCHECK, OnDontExtractFilterCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpertPage message handlers



void CExpertPage::OnFilterCheck() 
{
	/*UpdateData(TRUE);
	// Set the edit box to readonly
	((CEdit *)GetDlgItem(IDC_FILTER))->SetReadOnly(!*m_pbFilter);	*/
}

void CExpertPage::OnExtractFilterCheck() 
{
	UpdateData(TRUE);
	// Set the edit box to readonly
	((CEdit *)GetDlgItem(IDC_EXTRACTFILTER))->SetReadOnly(!*m_pbExtractCheck);
}

void CExpertPage::OnDontExtractFilterCheck() 
{
	UpdateData(TRUE);
	// Set the edit box to readonly
	((CEdit *)GetDlgItem(IDC_DONTEXTRACTFILTER))->SetReadOnly(!*m_pbDontExtractCheck);	
}

BOOL CExpertPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);

	m_ToolTip.AddTool(GetDlgItem(IDC_EXTRACTFILTERCHECK), IDC_FILTERCHECK);
	m_ToolTip.AddTool(GetDlgItem(IDC_DONTEXTRACTFILTERCHECK), IDC_FILTERCHECK);
	m_ToolTip.AddTool(GetDlgItem(IDC_OVERWRITE), IDC_OVERWRITE);
	m_ToolTip.AddTool(GetDlgItem(IDC_ONLYNEWER), IDC_ONLYNEWER);
	m_ToolTip.AddTool(GetDlgItem(IDC_RECREATEDIR), IDC_RECREATEDIR);
	
	m_ToolTip.AddTool(GetDlgItem(IDC_FILTER), IDC_FILTER);
	m_ToolTip.AddTool(GetDlgItem(IDC_DONTEXTRACTFILTER), IDC_DONTEXTRACTFILTER);
	m_ToolTip.AddTool(GetDlgItem(IDC_EXTRACTFILTER), IDC_EXTRACTFILTER);

	m_ToolTip.AddTool(GetDlgItem(IDC_OWNDIR), IDC_OWNDIR);

	InitData();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CExpertPage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message== WM_LBUTTONDOWN || pMsg->message== WM_LBUTTONUP || pMsg->message== WM_MOUSEMOVE) {
		m_ToolTip.RelayEvent(pMsg);
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CExpertPage::InitData()
{
	*m_pbExtractCheck = !m_psExtractFilter->IsEmpty();
	*m_pbDontExtractCheck = !m_psDontExtractFilter->IsEmpty();

	*m_pbFilter = *m_psFilter != "*.zip/*.rar/*.ace";
	

	UpdateData(FALSE);

	OnFilterCheck();
	OnExtractFilterCheck();
	OnDontExtractFilterCheck();	
}


BOOL CExpertPage::OnSetActive() 
{
	InitData();
	
	return CPropertyPage::OnSetActive();
}

BOOL CExpertPage::OnKillActive() 
{
	/*if (MessageBox("Changes will be lost if you switch to simple mode.\r\n\r\nDo you want to continue?", "WARNING", MB_YESNO | MB_ICONEXCLAMATION) == IDNO) {
		return FALSE;
	}*/
	//int n = ((CPropertySheet *)GetParent())->Get
	return CPropertyPage::OnKillActive();
}

BOOL CExpertPage::OnApply() 
{
	//UpdateData();	
	return CPropertyPage::OnApply();
}
