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

// WizPage4.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "WizPageAction.h"
#include "WizSheet.h"
#include "DirectoryPicker.h"
#include "Unrar.h"
#include "Unzip.h"
#include "Unace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL Pump(HWND hWnd);

/////////////////////////////////////////////////////////////////////////////
// CWizPageAction property page

IMPLEMENT_DYNCREATE(CWizPageAction, CWizPage)

CWizPageAction::CWizPageAction() : CWizPage(CWizPageAction::IDD)
{
	//{{AFX_DATA_INIT(CWizPageAction)
	//}}AFX_DATA_INIT
	m_pTipDlg = NULL;
	m_psTempSource = NULL;
	m_psTempTarget = NULL;
	m_b2ndExtraction = FALSE;
	m_b3rdExtraction = FALSE;
}

CWizPageAction::~CWizPageAction()
{
}

void CWizPageAction::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizPageAction)
	DDX_Control(pDX, IDC_FILES, m_ActionCtrl);
	DDX_Control(pDX, IDC_WIZTIP, m_TipButton);
	//}}AFX_DATA_MAP
	tagWizData * pData = GetData();
	DDX_Text(pDX, IDC_WIZTARGET, pData->m_sInstallationDir);
}


BEGIN_MESSAGE_MAP(CWizPageAction, CPropertyPage)
	//{{AFX_MSG_MAP(CWizPageAction)
	ON_BN_CLICKED(IDC_WIZPICKSOURCE, OnWizPickSource)
	ON_BN_CLICKED(IDC_WIZTAKEACTION, OnDoIt)
	ON_NOTIFY(NM_CLICK, IDC_FILES, OnClickFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizPageAction message handlers

BOOL CWizPageAction::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	tagWizData * pData = GetData();
	
	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CHANGEDIR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_WIZPICKSOURCE))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TIP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_TipButton.SetIcon(hIcon);
	}

	AddListHeaders();

	/*archiveType type = ((CWizSheet *)GetParent())->AnalyzeDirectory(*m_psTarget, m_sFilter.GetBuffer(_MAX_PATH));
	m_sFilter.ReleaseBuffer();

	AddAction(m_sFilter, "GAGA");*/

	// Set the new source
	//*m_psSource = *m_psTarget;

	//*m_psInstalledDir = *m_psGameDir;	
	pData->m_sInstallationDir = pData->m_sDefaultInstallDir;


	UpdateData(FALSE);

	//GetDlgItem(IDC_FILETYPE)->SetWindowText("Found archives: " + m_sFilter);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWizPageAction::AddListHeaders()
{
	int nItem = 0;
	
	int nSubItem = 0;

	// Add the list control headers
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 140;
	lvc.pszText = _T("Filename");
	m_ActionCtrl.InsertColumn(0, &lvc);
	
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Action");
	m_ActionCtrl.InsertColumn(1, &lvc);

}

BOOL CWizPageAction::AddAction(LPCTSTR lpszFileName, LPCTSTR lpszAction)
{
	int nSubItem = 0;
	LVITEM item;
	item.mask = LVIF_TEXT;
	item.iItem = 0;
	item.iSubItem = nSubItem++;
	item.pszText = (LPTSTR)lpszFileName;
	int nRet = m_ActionCtrl.InsertItem(&item);
	if (nRet == -1) {
		return FALSE;
	}

	item.iItem = nRet;
	item.iSubItem = nSubItem++;
	item.pszText = (LPTSTR)lpszAction;
	return m_ActionCtrl.SetItem(&item) != -1;
}

BOOL CWizPageAction::OnSetActive() 
{
	tagWizData * pData = GetData();
	
	if (pData->m_psCurTarget->Right(1) != "\\") {
		*pData->m_psCurTarget += "\\";
	}

	if (m_ActionCtrl.GetItemCount() != 0) {
		if (m_psTempSource) {
			pData->m_psCurSource = m_psTempSource;
		}
		if (m_psTempTarget) {
			pData->m_psCurTarget = m_psTempTarget;				
		}
		((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	}


	if (pData->m_sInstallationDir == pData->m_sDefaultInstallDir) {
		pData->m_sInstallationDir = pData->m_sDefaultInstallDir;
	}

	SetDlgItemText(IDC_WIZCURDIR, *pData->m_psCurTarget);

	CheckForFiles();

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_TipButton.SetTipText(CreateTipText());

	return CPropertyPage::OnSetActive();
}

void CWizPageAction::OnWizPickSource() 
{
	tagWizData * pData = GetData();

	UpdateData();
	// Open a browse dialog
	CDirectoryPicker picker(m_hWnd, &pData->m_sInstallationDir);
	UpdateData(FALSE);
	
}

void CWizPageAction::CheckForFiles()
{
	CWaitCursor wait;

	tagWizData * pData = GetData();

	CFileFind finder;

	m_ActionCtrl.DeleteAllItems();
	
	if (pData->m_psCurTarget->Right(1) != "\\") {
		*pData->m_psCurTarget += "\\";
	}

	BOOL bFound = finder.FindFile(*pData->m_psCurTarget + "*.ace");
	while (bFound) {
		bFound = finder.FindNextFile();
		AddAction(finder.GetFileName(), "Unace");
	}

	finder.Close();
	
	bFound = finder.FindFile(*pData->m_psCurTarget + "*.rar");
	while (bFound) {
		bFound = finder.FindNextFile();
		AddAction(finder.GetFileName(), "Unrar");
	}

	bFound = finder.FindFile(*pData->m_psCurTarget + "*.zip");
	while (bFound) {
		bFound = finder.FindNextFile();
		AddAction(finder.GetFileName(), "Unzip");
	}


	bFound = finder.FindFile(*pData->m_psCurTarget + "*.exe");
	while (bFound) {
		bFound = finder.FindNextFile();
		// Test if these are self extraction archives...
		CUnzip unzipper(this); unzipper.Init();
		CUnace unace(this); unace.Init();
		CUnrar unrar(this); unrar.Init();
		if (unzipper.IsArchive(finder.GetFilePath())) {
			AddAction(finder.GetFileName(), "Unzip");
		}
		else if (unace.IsArchive(finder.GetFilePath())) {
			AddAction(finder.GetFileName(), "Unace");
		}
		else if (unrar.IsArchive(finder.GetFilePath())) {
			AddAction(finder.GetFileName(), "Unrar");
		}
		else {
			AddAction(finder.GetFileName(), "Execute");
		}
	}

	bFound = finder.FindFile(*pData->m_psCurTarget + "*.bat");
	while (bFound) {
		bFound = finder.FindNextFile();
		AddAction(finder.GetFileName(), "Execute");
	}
	bFound = finder.FindFile(*pData->m_psCurTarget + "*.reg");
	while (bFound) {
		bFound = finder.FindNextFile();
		AddAction(finder.GetFileName(), "Execute");
	}

	pData->m_Error = EX_OK;
}

void CWizPageAction::OnClickFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	tagWizData * pData = GetData();


	NMLISTVIEW* pNMListView = (NM_LISTVIEW *)pNMHDR;
	if (m_ActionCtrl.HitTest(pNMListView->ptAction) == -1) {
		// The doubleclick was not on an item. Nothing to do!
		return;
	}
	// Get the entry from the data field.
	CString sAction = m_ActionCtrl.GetItemText(pNMListView->iItem, 1);
	if (sAction == "Execute") {
		((CEdit *)GetDlgItem(IDC_WIZTARGET))->SetReadOnly(TRUE);
		GetDlgItem(IDC_WIZPICKSOURCE)->ModifyStyle(0, WS_DISABLED);
		SetDlgItemText(IDC_WIZTAKEACTION, sAction);
	}
	else {
		((CEdit *)GetDlgItem(IDC_WIZTARGET))->SetReadOnly(FALSE);
		GetDlgItem(IDC_WIZPICKSOURCE)->ModifyStyle(WS_DISABLED, 0);
		pData->m_sExtractFilter = m_ActionCtrl.GetItemText(pNMListView->iItem, 0);
		CString sNewTarget = pData->m_sInstallationDir;
		if (sNewTarget.Right(1) != "\\") {
			sNewTarget += "\\";
		}
		if (pData->m_sInstallationDir == pData->m_sDefaultInstallDir) {
			sNewTarget += pData->m_sExtractFilter.Left(pData->m_sExtractFilter.ReverseFind('.'));
		}
		
		GetDlgItem(IDC_WIZTARGET)->SetWindowText(sNewTarget);
		SetDlgItemText(IDC_WIZTAKEACTION, sAction);
	}

	InvalidateRect(NULL);
	UpdateWindow();

	*pResult = 0;
}

LRESULT CWizPageAction::OnWizardNext() 
{
	if (!m_b3rdExtraction && m_b2ndExtraction) {
		tagWizData * pData = GetData();
		pData->m_psCurTarget = &pData->m_sInstallationDir;
		SetDlgItemText(IDC_WIZCURDIR, *pData->m_psCurTarget);
		CheckForFiles();
		if (m_ActionCtrl.GetItemCount() == 0) {
			// If there were no files found, go to the end
			((CPropertySheet *)GetParent())->SetActivePage(WIZ_END);
		}
		m_TipButton.SetTipText(CreateTipText());
		return -1;
	}
	return CPropertyPage::OnWizardNext();
}

LRESULT CWizPageAction::OnWizardBack() 
{
	return CPropertyPage::OnWizardBack();
}

void CWizPageAction::OnDoIt()
{
	tagWizData * pData = GetData();

	UpdateData();

	POSITION pos = m_ActionCtrl.GetFirstSelectedItemPosition();	
	if (pos) {
		int nItem = m_ActionCtrl.GetNextSelectedItem(pos);
		UpdateData();
		CString sAction = m_ActionCtrl.GetItemText(nItem, 1);
		if (sAction == "Unace" || sAction == "Unrar" || sAction == "Unzip") {
			m_psTempSource = pData->m_psCurSource;
			m_psTempTarget = pData->m_psCurTarget;

			pData->m_sExtractFilter = m_ActionCtrl.GetItemText(nItem, 0);
			pData->m_psCurSource = pData->m_psCurTarget;
			pData->m_psCurTarget = &pData->m_sInstallationDir;
			pData->m_nNextPage = ((CPropertySheet *)GetParent())->GetActiveIndex();
			((CPropertySheet *)GetParent())->SetActivePage(WIZ_UNZIP);
			if (m_b2ndExtraction) {
				m_b3rdExtraction = TRUE;
			}
			m_b2ndExtraction = TRUE;
		}
		else if (sAction == "Execute") {
			((CPropertySheet *)GetParent())->SetWizardButtons(0);

			CString sFile = *pData->m_psCurTarget;
			if (sFile.Right(1) != "\\") {
				sFile += "\\";
			}
			sFile += m_ActionCtrl.GetItemText(nItem, 0);
			if (sFile.Right(3).CompareNoCase("reg") == 0) {
				ShellExecute(m_hWnd, "open", sFile, NULL, NULL, SW_NORMAL);
			}
			else {
				STARTUPINFO sui = {0};
				sui.cb = sizeof(sui);
				sui.lpReserved = NULL;
				sui.lpReserved2 = NULL;
				sui.lpDesktop = NULL;
				sui.lpTitle = NULL;
				PROCESS_INFORMATION pi;
				if (CreateProcess(sFile, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NORMAL_PRIORITY_CLASS, NULL, *pData->m_psCurTarget, &sui, &pi)) {
					while(WaitForSingleObject(pi.hProcess, 500) != WAIT_OBJECT_0) {
						Pump(m_hWnd);
					}
					DWORD dwReturn;
					GetExitCodeProcess(pi.hProcess, &dwReturn);
				}
				sFile = m_ActionCtrl.GetItemText(nItem, 0);
				if (sFile.CompareNoCase("install.exe") == 0 || sFile.CompareNoCase("setup.exe") == 0) {
					MessageBox("Where did you install the application?\r\n\r\nPlease choose the path...", "Installation directory", MB_OK | MB_ICONQUESTION);
					OnWizPickSource();
				}
				if (m_b2ndExtraction) {
					m_b3rdExtraction = TRUE;
				}
				m_b2ndExtraction = TRUE;
			}
		}
	}

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
}

CString CWizPageAction::CreateTipText()
{
	tagWizData * pData = GetData();

	CString sTipText;
	CString sFile, sAction;
	for(int i = 0; i < m_ActionCtrl.GetItemCount(); i++) {
		sFile = m_ActionCtrl.GetItemText(i, 0);
		sAction = m_ActionCtrl.GetItemText(i, 1);
		
		if (sFile.CompareNoCase("install.exe") == 0 || sFile.CompareNoCase("setup.exe") == 0) {
			sTipText += "- Run " + sFile + " to install the program\r\n";
		}
		
		if (sFile.CompareNoCase("setup.bat") == 0) {
			sTipText += "- " + sFile + " probably unpacks some stuff\r\n        >>> Run it!\r\n";
		}
		else if (sFile.CompareNoCase("unpack.exe") == 0) {
			sTipText += "- " + sFile + " probably unpacks some stuff\r\n        >>> Run it!\r\n";
		}

		if (sFile.CompareNoCase("regsetup.exe") == 0 || sFile.CompareNoCase("setupreg.exe") == 0) {
			sTipText += "- " + sFile + " adds informations to the registry\r\n        >>> Run it!\r\n";
		}

		CString sExt = sFile.Right(sFile.GetLength() - sFile.ReverseFind('.') - 1);
		if (sExt.CompareNoCase("ace") == 0 || sExt.CompareNoCase("rar") == 0) {
			CFileFind finder;

			BOOL bFind = finder.FindFile(*pData->m_psCurTarget + sFile.Left(sFile.ReverseFind('.')) + "*");
			int nFiles = 0;
			while(bFind) {
				bFind = finder.FindNextFile();
				nFiles++;
				if (nFiles > 2) {
					break;
				}
			}
			finder.Close();

			if (nFiles > 2) {
				sTipText += "- " + sFile + " seems to be the main archive.\r\n        >>> Extract it\r\n";
			}
			else {
				sTipText += "- " + sFile + " is a single archive; probably a crack\r\n        >>> Extract it to the installation dir\r\n";
			}
		}
		else if (sExt.CompareNoCase("exe") == 0) {
			if (sAction == "Unace" || sAction == "Unrar" || sAction == "Unzip") {
				sTipText += "- " + sFile + " is a selfextracting Archive.\r\n        >>> Extract it\r\n";
			}
		}
		else if (sExt.CompareNoCase("reg") == 0) {
			sTipText += "- " + sFile + " contains registry information.\r\n        >>> Run it\r\n";
		}

	}
	CFileFind finder;
	
	return sTipText;
}
