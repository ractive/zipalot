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

// WizError.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "WizError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizError property page

IMPLEMENT_DYNCREATE(CWizError, CWizPage)

CWizError::CWizError() : CWizPage(CWizError::IDD)
{
	//{{AFX_DATA_INIT(CWizError)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWizError::~CWizError()
{
}

void CWizError::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizError)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizError, CPropertyPage)
	//{{AFX_MSG_MAP(CWizError)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizError message handlers

extern char curzipfile[_MAX_PATH];

BOOL CWizError::OnSetActive() 
{
	tagWizData * pData = GetData();

	((CPropertySheet *)GetParent())->SetWizardButtons(0);

	CString sErrorMsg;
	CString sFileName = curzipfile;
	/*if (sFile) {
		sFileName = sFile;
	}*/
	switch(pData->m_Error) {
		case EX_OK:
			return TRUE;
		case EX_ARCHIVE:
			sErrorMsg = "Error in Archive file " + sFileName;
			break;
		case EX_MEM:
			sErrorMsg = "Not enough free memory available";
			break;
		case EX_NO_ARCHIVES:
			sErrorMsg = "No archives found";
			break;
		case EX_NO_FILES:
			sErrorMsg = "No files in archive found " + sFileName;
			break;
		case EX_DISK:
			sErrorMsg = "The disk is probably full";
			break;
		case EX_EOF:
			sErrorMsg = "Unexpected end of file found. Archive is probably corrupt";
			break;
		case EX_UNSUP:
			sErrorMsg = "Unsupported archive type";
			break;
		case EX_BADPWD:
			sErrorMsg = "Bad password for file " + sFileName;
			break;
		case EX_CRC:
			sErrorMsg = "CRC error in " + sFileName + ". Data is probably corrupt";
			break;
		case EX_OPEN:
			sErrorMsg = sFileName + " could not be opened";
			break;
		case EX_CLOSE:
			sErrorMsg = sFileName + " could not be closed";
			break;
		case EX_READ:
			sErrorMsg = "No data could be read from " + sFileName;
			break;
		case EX_WRITE:
			sErrorMsg = "No data could be written to " + sFileName;
			break;
		case EX_CREATE:
			sErrorMsg = sFileName + " could not be created";
			break;
		case EX_CANCELLED:
			sErrorMsg = "Operation cancelled";
			break;
		default:
			sErrorMsg = "Unknown Error while processing files";
	}

	GetDlgItem(IDC_ERROR)->SetWindowText(sErrorMsg);

	return CPropertyPage::OnSetActive();
}
