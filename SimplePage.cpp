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

// SimplePage.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "SimplePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimplePage property page

IMPLEMENT_DYNCREATE(CSimplePage, CPropertyPage)

CSimplePage::CSimplePage() : CPropertyPage(CSimplePage::IDD)
{
	//{{AFX_DATA_INIT(CSimplePage)
	m_bDocs = FALSE;
	m_bAce = TRUE;
	m_bRar = TRUE;
	m_bZip = TRUE;
	m_bImages = FALSE;
	m_bMusic = FALSE;
	m_bPrograms = FALSE;
	m_bVideo = FALSE;
	m_bWeb = FALSE;
	//}}AFX_DATA_INIT

	CString sTipText = "\r\nAttention!\r\n\r\nIf you switch from EXPERT to SIMPLE mode,\r\nthe changes you made get lost!";
	m_WizTip.SetTipText(sTipText);
}

CSimplePage::~CSimplePage()
{
}

void CSimplePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	
	if (((CPropertySheet *)GetParent())->GetActiveIndex() != 0) {
		// Don't store any data if this is not the active page
		return;
	}

	//{{AFX_DATA_MAP(CSimplePage)
	DDX_Control(pDX, IDC_WIZTIP, m_WizTip);
	DDX_Check(pDX, IDC_DOCS, m_bDocs);
	DDX_Check(pDX, IDC_EXTRACTACE, m_bAce);
	DDX_Check(pDX, IDC_EXTRACTRAR, m_bRar);
	DDX_Check(pDX, IDC_EXTRACTZIP, m_bZip);
	DDX_Check(pDX, IDC_IMAGES, m_bImages);
	DDX_Check(pDX, IDC_MUSIC, m_bMusic);
	DDX_Check(pDX, IDC_PROGRAMS, m_bPrograms);
	DDX_Check(pDX, IDC_VIDEO, m_bVideo);
	DDX_Check(pDX, IDC_WEBDOCS, m_bWeb);
	//}}AFX_DATA_MAP

	m_psFilter->Format("%s/%s/%s", m_bZip ? "*.zip" : "", m_bRar ? "*.rar" : "", m_bAce ? "*.ace" : "");

	m_psExtractFilter->Empty();

	if (m_bImages) {
		*m_psExtractFilter += "/*.gif/*.bmp/*.jpg/*.jpeg/*.jfif/*.jpe/*.eps/*.iff/*.img/*.pic/*.tga/*.wmf/*.tiff/*.tif/*.png/*.sda/*.cda/*.pds/*.psp";
	}

	if (m_bMusic) {
		*m_psExtractFilter += "/*.mp1/*.mp2/*.mp3/*.m3u/*.pls/*.wav/*.au/*.wma/*.voc/*.mod";
	}

	if (m_bPrograms) {
		*m_psExtractFilter += "/*.exe/*.bat/*.com";
	}

	if (m_bDocs) {
		*m_psExtractFilter += "/*.doc/*.dot/*.pps/*.xl?/*.txt/*.pdf/*.ps/*.sdw/*.sdc/*.sdp/*.sgl";
	}

	if (m_bVideo) {
		*m_psExtractFilter += "/*.mpg/*.mpeg/*.avi/*.wmv/*.rm/*.asf";	
	}

	if (m_bWeb) {
		*m_psExtractFilter += "/*.html/*.shtml/*.xml/*.phtml/*.php/*.xhtml/*.cgi";	
	}

	m_psExtractFilter->TrimLeft('/');
}


BEGIN_MESSAGE_MAP(CSimplePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSimplePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimplePage message handlers

BOOL CSimplePage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message== WM_LBUTTONDOWN || pMsg->message== WM_LBUTTONUP || pMsg->message== WM_MOUSEMOVE) {
		m_ToolTip.RelayEvent(pMsg);
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CSimplePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);

	m_ToolTip.AddTool(GetDlgItem(IDC_EXTRACTACE), IDC_EXTRACTACE);
	m_ToolTip.AddTool(GetDlgItem(IDC_EXTRACTRAR), IDC_EXTRACTRAR);
	m_ToolTip.AddTool(GetDlgItem(IDC_EXTRACTZIP), IDC_EXTRACTZIP);
	m_ToolTip.AddTool(GetDlgItem(IDC_DOCS), IDC_DOCS);
	m_ToolTip.AddTool(GetDlgItem(IDC_WEB), IDC_WEB);
	m_ToolTip.AddTool(GetDlgItem(IDC_MUSIC), IDC_MUSIC);
	m_ToolTip.AddTool(GetDlgItem(IDC_PROGRAMS), IDC_PROGRAMS);
	m_ToolTip.AddTool(GetDlgItem(IDC_VIDEO), IDC_VIDEO);
	m_ToolTip.AddTool(GetDlgItem(IDC_IMAGES), IDC_IMAGES);

	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TIP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_WizTip.SetIcon(hIcon);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSimplePage::OnSetActive() 
{
	if (m_psExtractFilter->IsEmpty() || *m_pbExtractCheck == FALSE) {
		m_bMusic = m_bImages = m_bDocs = m_bPrograms = m_bVideo = m_bWeb = FALSE;
	}

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL CSimplePage::OnApply() 
{
	//UpdateData();
	
	return CPropertyPage::OnApply();
}
