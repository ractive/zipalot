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

// WizEnd.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "WizEnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizEnd property page

IMPLEMENT_DYNCREATE(CWizEnd, CWizPage)

CWizEnd::CWizEnd() : CWizPage(CWizEnd::IDD)
{
	//{{AFX_DATA_INIT(CWizEnd)
	m_bDelTemp = FALSE;
	//}}AFX_DATA_INIT
}

CWizEnd::~CWizEnd()
{
}

void CWizEnd::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizEnd)
	DDX_Check(pDX, IDC_WIZDELTEMP, m_bDelTemp);
	DDX_Control(pDX, IDC_WIZTIP, m_TipButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizEnd, CPropertyPage)
	//{{AFX_MSG_MAP(CWizEnd)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizEnd message handlers

BOOL CWizEnd::OnSetActive() 
{
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_FINISH);
	
	return CPropertyPage::OnSetActive();
}

BOOL CWizEnd::OnWizardFinish() 
{
	UpdateData();
	if (m_bDelTemp) {
		tagWizData * pData = GetData();
		CString sExtractDir = pData->GetExtractDir();
		if (!sExtractDir.IsEmpty()) {
			if (sExtractDir.Right(1) != "\\") {
				sExtractDir += "\\";
			}

			CFileFind finder;
			int nFiles = 0;
			BOOL bFound = finder.FindFile(sExtractDir + "*");
			while (bFound) {
				bFound = finder.FindNextFile();
				if (finder.IsDots()) {
					continue;
				}
				nFiles++;
			}

			finder.Close();

			if (nFiles > 0) {
				// Allocate a string for all the names of the files that will
				// be deleted. It's a double zero terminated string.
				char *pFrom = new char[nFiles * (_MAX_PATH + 1) + 1];
				// Set it to 0
				memset(pFrom, '\0', nFiles * (_MAX_PATH + 1) + 1);
				char *cursor = pFrom;

				BOOL bFound = finder.FindFile(sExtractDir + "*");
				while (bFound) {
					bFound = finder.FindNextFile();
					if (finder.IsDots()) {
						continue;
					}

					char elem[_MAX_PATH];
					strcpy(elem, finder.GetFilePath());
					// Copy the filename to pFrom
					strcpy(cursor, elem);
					cursor += strlen(elem) + 1;
				}



				// Set up the struct to tell Windows to delete the files.
				SHFILEOPSTRUCT shFile;
				shFile.hwnd = 0;
				shFile.wFunc = FO_DELETE;
				shFile.pFrom = pFrom;
				shFile.pTo = NULL;
				shFile.fFlags = FOF_ALLOWUNDO;
				shFile.fAnyOperationsAborted = FALSE;

				bool bRet = true;

				// Now we delete the files.
				bRet = SHFileOperation(&shFile) == 0;
				if (bRet) {
					BOOL b = RemoveDirectory(sExtractDir);
					if (!b) {
						DWORD dwError = GetLastError();
					}
				}
			}
		}
	}
	
	return CPropertyPage::OnWizardFinish();
}

BOOL CWizEnd::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TIP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_TipButton.SetIcon(hIcon);
	}

	tagWizData * pData = GetData();
	CString sExtractDir = pData->GetExtractDir();
	if (!sExtractDir.IsEmpty()) {
		CString sTipText = "\r\n\r\nYou can delete the the extracted\r\narchive files.\r\n\r\nBut try the program out, before you\r\ndelete the ZIP files :-)";
		m_TipButton.SetTipText(sTipText);
	}
	else {
		GetDlgItem(IDC_WIZDELTEMP)->ModifyStyle(WS_VISIBLE, 0);
		m_TipButton.ModifyStyle(WS_VISIBLE, 0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
