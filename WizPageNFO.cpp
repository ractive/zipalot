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

// WizPage3.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "WizPageNFO.h"
#include "NfoViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizPageNFO property page

IMPLEMENT_DYNCREATE(CWizPageNFO, CWizPage)

CWizPageNFO::CWizPageNFO() : CWizPage(CWizPageNFO::IDD)
{
	//{{AFX_DATA_INIT(CWizPageNFO)
	m_sInfoFile = _T("");
	//}}AFX_DATA_INIT
	m_pNfoViewer = NULL;

}

CWizPageNFO::~CWizPageNFO()
{
	if (m_pNfoViewer != NULL) {
		//delete m_pNfoViewer;
		m_pNfoViewer = NULL;
	}
}

void CWizPageNFO::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizPageNFO)
	DDX_Control(pDX, IDC_INFOFILES, m_InfoFiles);
	DDX_LBString(pDX, IDC_INFOFILES, m_sInfoFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizPageNFO, CPropertyPage)
	//{{AFX_MSG_MAP(CWizPageNFO)
	ON_LBN_DBLCLK(IDC_INFOFILES, OnDblclkInfofiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizPageNFO message handlers

BOOL CWizPageNFO::OnSetActive() 
{
	tagWizData * pData = GetData();

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);

	if (!HasNFOFile(*pData->m_psCurTarget)) {
		return FALSE;
	}

	SetDlgItemText(IDC_WIZCURDIR, *pData->m_psCurTarget);

	m_InfoFiles.ResetContent();

	CFileFind finder;
	BOOL bFound = finder.FindFile(*pData->m_psCurTarget + "*.nfo");
	while (bFound) {
		bFound = finder.FindNextFile();
		m_InfoFiles.AddString(finder.GetFileName());
	}
	finder.Close();

	bFound = finder.FindFile(*pData->m_psCurTarget + "*.diz");
	while (bFound) {
		bFound = finder.FindNextFile();
		m_InfoFiles.AddString(finder.GetFileName());
	}
	finder.Close();

	return CPropertyPage::OnSetActive();
}

void CWizPageNFO::OnDblclkInfofiles() 
{
	tagWizData * pData = GetData();

	int nSel = m_InfoFiles.GetCurSel();
	if (nSel == -1) {
		return;
	}
	
	UpdateData();

	//ShellExecute(AfxGetMainWnd()->m_hWnd, "open", "notepad.exe", m_sInfoFile, *m_psTarget, SW_NORMAL);
	RECT rc;
	rc.top = 0;
	rc.bottom = 500;
	rc.left = 0;
	rc.right = 500;

	//LPCTSTR wndClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_WINDOW+1), 0);

	m_pNfoViewer = new CNfoViewer(*pData->m_psCurTarget + m_sInfoFile);

	if (m_pNfoViewer->Create(NULL, m_sInfoFile, WS_VSCROLL | WS_OVERLAPPEDWINDOW | WS_POPUP, rc, this)) {
		m_pNfoViewer->ShowWindow(SW_SHOWNORMAL);
		m_pNfoViewer->UpdateWindow();
	}
}

BOOL CWizPageNFO::HasNFOFile(LPCTSTR sDir)
{
	CFileFind finder;
	CString sWildCard = sDir;
	if (sWildCard.Right(1) != "\\") {
		sWildCard += "\\";
	}
	sWildCard += "*.nfo";
	BOOL bRet = finder.FindFile(sWildCard);
	finder.Close();
	
	sWildCard = sDir;
	if (sWildCard.Right(1) != "\\") {
		sWildCard += "\\";
	}
	sWildCard += "*.diz";
	return bRet || finder.FindFile(sWildCard);
}

BOOL CWizPageNFO::OnInitDialog() 
{
	tagWizData * pData = GetData();
	CPropertyPage::OnInitDialog();

	if (pData->m_psCurTarget->Right(1) != "\\") {
		*pData->m_psCurTarget += "\\";
	}
	
	CheckForNewDir();
	
	if (pData->m_psCurTarget->Right(1) != "\\") {
		*pData->m_psCurTarget += "\\";
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CWizPageNFO::OnWizardNext() 
{
	return CPropertyPage::OnWizardNext();
}

