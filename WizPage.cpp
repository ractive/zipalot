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

// WizPage.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "WizPage.h"
#include "WizSheet.h"
#include "Unzip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizPage property page

IMPLEMENT_DYNCREATE(CWizPage, CPropertyPage)

CWizPage::CWizPage() : CPropertyPage()
{
	//{{AFX_DATA_INIT(CWizPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


CWizPage::CWizPage(UINT nIDTemplate) : CPropertyPage(nIDTemplate)
{
	//{{AFX_DATA_INIT(CWizPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWizPage::~CWizPage()
{
}


BEGIN_MESSAGE_MAP(CWizPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWizPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizPage message handlers

tagWizData * CWizPage::GetData()
{
	return &((CWizSheet *)GetParent())->m_Data;
}

void CWizPage::CheckForNewDir()
{
	tagWizData * pData = GetData();

	CFileFind finder;
	CString sTemp;
	BOOL bFound = finder.FindFile(*pData->m_psCurTarget + "*");
	int nDirs = 0;
	while (bFound) {
		bFound = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		}

		if (!finder.IsDirectory()) {
			// Other files have been found, not only a new directory
			return;
		}
		else {
			sTemp = finder.GetFilePath();
			nDirs++;
		}

		if (nDirs > 1) {
			// More than one directories have been found
			return;
		}
	}

	// The first dir is the only one. We change the target
	// to this directory.
	*pData->m_psCurTarget = sTemp;
}

void CWizPage::AnalyzeSourceDir()
{
	// Check if we can find archives...
	//errorCode error;
	tagWizData * pData = GetData();

	pData->m_Type = NO_TYPE;

	CString sWild = *pData->m_psCurSource;
	if (sWild.Right(1) != "\\") {
		sWild += "\\";
	}
	sWild += "*";

	CFileFind finder;

	if (!finder.FindFile(sWild)) {
		// No files found at all
		pData->m_Type = NO_TYPE;
		return;
	}
	
	finder.Close();

	// Search for ACE files
	sWild = *pData->m_psCurSource;
	if (sWild.Right(1) != "\\") {
		sWild += "\\";
	}
	sWild += "*.ace";

	if (finder.FindFile(sWild)) {
		finder.FindNextFile();
		// We found an ACE file
		pData->m_sExtractFilter, "*.ace";
		pData->m_Type = ACE;
		return;

		// We don't test it, because it takes
		// too much time...
	}

	finder.Close();

	// Search for ACE files
	sWild = *pData->m_psCurSource;
	if (sWild.Right(1) != "\\") {
		sWild += "\\";
	}
	sWild += "*.rar";

	if (finder.FindFile(sWild)) {
		finder.FindNextFile();

		pData->m_sExtractFilter = "*.rar";
		pData->m_Type = RAR;
		return;

	}

	// Search for ZIP files
	sWild = *pData->m_psCurSource;
	if (sWild.Right(1) != "\\") {
		sWild += "\\";
	}
	sWild += "*.zip";

	if (!finder.FindFile(sWild)) {
		finder.Close();
		// No *.zip files found
		// Now check every file  if it's a ZIP file.
		// Sometimes the ZIP-files are camouflaged as other filetypes (i.e. as *.jpg)
		sWild = *pData->m_psCurSource;
		if (sWild.Right(1) != "\\") {
			sWild += "\\";
		}
		sWild += "*";

		if (finder.FindFile(sWild)) {
			while (finder.FindNextFile()) {
				if (finder.IsDots() || finder.IsDirectory()) {
					continue;
				}
				CUnzip unzipper(NULL);
				if (!unzipper.Init()) {
					return;
				}
				CString sFP = finder.GetFilePath();
				errorCode ret = unzipper.Test(sFP);
				if (ret == EX_OK) {
					// The current file is a ZIP file
					// Extract all files with the same extension
					pData->m_sExtractFilter = "*." + sFP.Right(sFP.GetLength() - sFP.ReverseFind('.') - 1);
					pData->m_Type = ZIP;
					return;
				}
			}
		}
	}
	else {
		pData->m_sExtractFilter = "*.zip";
		pData->m_Type = ZIP;
	}
}