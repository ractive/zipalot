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

// Extract.cpp: implementation of the CExtract class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <imagehlp.h>
#include "zipalot.h"
#include "Extract.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CExtract * CExtract::m_pPseudoThis = NULL;
CString CExtract::m_sPassword = "";
tagUnzipInfo CExtract::unzipInfo = {0};
BOOL CExtract::m_bPwdSet = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern BOOL Pump(HWND hWnd);
extern HANDLE hStop;
extern CStringList g_sListCurZIPfiles;
//extern char curfile[_MAX_PATH];
extern char curzipfile[_MAX_PATH];

CExtract::CExtract(CDialog * pUnzipDlg)
{
	m_pPseudoThis = this;
	m_pUnzipDlg = pUnzipDlg;
	memset(&unzipInfo, 0, sizeof(unzipInfo));
	m_bPwdSet = FALSE;
	m_sPassword = "";
}

CExtract::~CExtract()
{
	FreeDLL();
}

BOOL CExtract::FindDLL()
{
	const CString sUnzipDllVer = "5.4";
	const CString sCompanyName = "Info-ZIP";
	char szFullPath[MAX_PATH];
	char *ptr;

	if (SearchPath(
			NULL,               /* address of search path               */
			m_sDLLName,       /* address of filename                  */
			NULL,               /* address of extension                 */
			_MAX_PATH,           /* size, in characters, of buffer       */
			szFullPath,         /* address of buffer for found filename */
			&ptr                /* address of pointer to file component */
			) == 0) {
		return FALSE;
	}

	/* Now we'll check the unzip dll version information. Note that this is
	not the same information as is returned from a call to UzpVersion()
	*/

	DWORD dwVerHnd;
	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);

	if (dwVerInfoSize) {
		BOOL  fRet, fRetName;
		char str[256];
		//LPSTR   lpstrVffInfo; /* Pointer to block to hold info */
		LPSTR lszVer = NULL;
		LPSTR lszVerName = NULL;
		UINT  cchVer = 0;


		LPSTR lpstrVffInfo = new char[dwVerInfoSize];

		/* Get the version information */
		if (GetFileVersionInfo(szFullPath, 0L, dwVerInfoSize, lpstrVffInfo)) {
			fRet = VerQueryValue(lpstrVffInfo, TEXT("\\StringFileInfo\\040904E4\\FileVersion"), (void **)&lszVer, &cchVer);
			fRetName = VerQueryValue(lpstrVffInfo, TEXT("\\StringFileInfo\\040904E4\\CompanyName"), (void **)&lszVerName, &cchVer);
			if (!fRet || !fRetName || (lstrcmpi(lszVer, sUnzipDllVer) != 0) || (lstrcmpi(lszVerName, sCompanyName) != 0)) {
				wsprintf (str, "The dll %s has the wrong version number.\r\nPlease use Version %s", m_sDLLName, sUnzipDllVer);
				delete [] lpstrVffInfo;
				MessageBox(NULL, str, "Error", MB_OK);
				return FALSE;
			}
		}
		delete [] lpstrVffInfo;
	}
	else {
		char str[256];
		wsprintf (str, "Cannot find %s", m_sDLLName);
		MessageBox(NULL, str, "Error", MB_OK);
		return FALSE;
	}
	
	return TRUE;
}

int CExtract::List(LPCTSTR sFileName, LPCTSTR sDestination)
{
	return 0;
}

BOOL CExtract::LoadDLL()
{
	m_sDLLName = GetDLLName();
	m_hDLL = LoadLibrary(m_sDLLName);
	if (m_hDLL == NULL) {
		return FALSE;
	}

	return GetProcAddresses();
}

void CExtract::FreeDLL()
{
	if (m_hDLL != NULL) {
		FreeLibrary(m_hDLL);
	}
}

BOOL CExtract::Init()
{
	if (!LoadDLL()) {
		char str[256];
		wsprintf (str, "Could not load %s", m_sDLLName);
		MessageBox(NULL, str, "Error", MB_OK);
		return FALSE;	
	}

	return DLLInit();
}

BOOL CExtract::DLLInit()
{
	return TRUE;
}

errorCode CExtract::ExtractAll(const LPCTSTR sSource, const LPCTSTR sTarget)
{
	CWaitCursor wait;

	// Update the unzip-info
	unzipInfo.sSource = sSource;
	unzipInfo.sTarget = sTarget;

	// Create the thread
	CWinThread * pThread = AfxBeginThread(ExtractThreadProc, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	pThread->m_bAutoDelete = FALSE;
	// Fire it up
	pThread->ResumeThread();

	// Hmm, not sure but here could occur a "deadlock". If the Window gets not messages
	// any more and we call Pump at the wrong time, Pump will wait will a new
	// message arrives. Very unlikely, but possible.
	DWORD dwRet;
	while ((dwRet = WaitForSingleObject(pThread->m_hThread, 100)) != WAIT_OBJECT_0) {
		if (dwRet == WAIT_FAILED) {
			break;
		}
		Pump(m_pPseudoThis->m_pUnzipDlg->m_hWnd);
	}

	DWORD exit;
	// If the thread exited successfully, it returns 0;
	::GetExitCodeThread(pThread->m_hThread, &exit);
	delete pThread;

	return (errorCode)exit;
}

UINT CExtract::ExtractThreadProc(LPVOID lpParam)
{
	CFileFind finder;
	CString sFind = m_pPseudoThis->unzipInfo.sSource;

	m_bPwdSet = FALSE;

	if (sFind.Right(1) != "\\") {
		sFind += '\\';
	}

	// Make sure the filter makes sense
	// If the filter is NULL, we add the normal filter
	if (m_pPseudoThis->unzipInfo.sFilter == NULL || strlen(m_pPseudoThis->unzipInfo.sFilter) < 3) {
		if (m_pPseudoThis->unzipInfo.type == ACE) {
			sFind += "*.ace";
		}
		else if (m_pPseudoThis->unzipInfo.type == RAR) {
			sFind += "*.rar";
		}
		else {
			sFind += "*.zip";
		}
	}
	else {
		sFind += m_pPseudoThis->unzipInfo.sFilter;
		sFind.TrimRight();
	}

	int nFiles = 0;
	int nFileNo = 1;
	BOOL bFound = FALSE;

	if (m_pPseudoThis->m_sDLLName == "UNACEV2.DLL" || m_pPseudoThis->m_sDLLName == "UNRAR.DLL") {
		bFound = finder.FindFile(sFind);
		while(bFound) {
			bFound = finder.FindNextFile();
			nFiles += m_pPseudoThis->CountFiles(finder.GetFilePath());
		}
		finder.Close();
	}
	/*else if(m_pPseudoThis->m_sDLLName == "UNRAR.DLL") {
		CString sFileTitle = sFind.Left(sFind.ReverseFind('.')) + "*";
		bFound = finder.FindFile(sFileTitle);
		while (bFound) {
			nFiles++;
			bFound = finder.FindNextFile();
		}

	}*/
	else {
		// Count the *.zip files in the directory.
		bFound = finder.FindFile(sFind);
		while (bFound) {
			nFiles++;
			bFound = finder.FindNextFile();
		}
		finder.Close();
	}


	if (nFiles == 0) {
		return EX_NO_ARCHIVES;
	}
	strcpy(curzipfile, m_pPseudoThis->unzipInfo.sSource);
	m_pPseudoThis->m_pUnzipDlg->SendMessage(WM_EXTRACT_SETSOURCE, 0, (long)curzipfile);
	strcpy(curzipfile, m_pPseudoThis->unzipInfo.sTarget);
	m_pPseudoThis->m_pUnzipDlg->SendMessage(WM_EXTRACT_SETTARGET, 0, (long)curzipfile);

	if (m_pPseudoThis->m_pUnzipDlg) {
		m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_RANGE, 0, nFiles);
	}

	finder.Close();

	CString sPath;
	CString sFileName;
	errorCode nRet = EX_NO_FILES;
	
	// Process all files
	bFound = finder.FindFile(sFind);
	while (bFound) {
		// Check for cancel event
		if (WaitForSingleObject(hStop, 0) == WAIT_OBJECT_0) {
			return EX_CANCELLED;
		}
		bFound = finder.FindNextFile();

		// Get the file informations
		sPath = finder.GetFilePath();
		sFileName = finder.GetFileName();

		if (m_pPseudoThis->m_pUnzipDlg) {
			g_sListCurZIPfiles.AddTail(sFileName);
			m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURZIPFILE, 0, 0);
		}

		if (m_pPseudoThis->unzipInfo.type != ZIP) {
			// Clear the password if it's not a ZIP FILE
			m_pPseudoThis->m_sPassword = "";
		}

		CString sNewDir;

		if (unzipInfo.bOwnDir) {
			CString sFName = sFileName;
			sFName = sFName.Mid(sFileName.ReverseFind('\\') + 1);
			sNewDir = m_pPseudoThis->unzipInfo.sTarget;
			sNewDir += "\\";
			sNewDir += sFName.Left(sFName.ReverseFind('.'));
			if (sNewDir.Right(1) != "\\") {
				sNewDir += "\\";
			}
			MakeSureDirectoryPathExists(sNewDir);
		}
		else if (m_pPseudoThis->unzipInfo.sTarget != NULL) {
			sNewDir = m_pPseudoThis->unzipInfo.sTarget;
		}

		nRet = m_pPseudoThis->Extract((LPTSTR)(LPCTSTR)sPath, sNewDir);
		if (nRet != EX_OK) {
			ReportError(nRet);
			// If the disk is full, we don't try processing the other
			// files
			/*if (nRet == EX_DISK) {
				return EX_DISK;
			}*/
		}
		if (m_pPseudoThis->m_pUnzipDlg) {
			m_pPseudoThis->m_pUnzipDlg->SendMessage(WM_EXTRACT_STEPIT, 0, 0);
		}
	}

	return nRet;
}

LPCTSTR CExtract::GetFileName(LPCTSTR sFilePath)
{
	LPCTSTR s = strrchr(sFilePath, '\\');
	if (s == NULL) {
		// No backslash found
		return sFilePath;
	}
	// Skip the backslash
	return ++s;
}

int CExtract::PromptPassword(LPTSTR *lpszPassword, LPCTSTR lpszFileName, LPCTSTR lpszArchive, LPCTSTR lpszMessage, BOOL bForce /* = FALSE */)
{
	if (!bForce && !m_sPassword.IsEmpty()) {
		*lpszPassword = (LPTSTR)(LPCTSTR)m_sPassword;
		return IDOK;
	}
	CPasswordDlg dlg;
	dlg.m_sFileName = lpszFileName;
	dlg.m_sMessage = lpszMessage;
	dlg.m_sZipPath = lpszArchive;
	if (dlg.DoModal() == IDOK) {
		m_sPassword = dlg.m_sPassword;
		*lpszPassword = (LPTSTR)(LPCTSTR)m_sPassword;
		return IDOK;
	}
	else {
		return IDCANCEL;
	}
}

void CExtract::ReportError(errorCode error, LPCTSTR sFile /* = NULL */)
{
	if (sFile) {
		strcpy(curzipfile, sFile);
	}
	m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_ERROR, (WPARAM)curzipfile, error);
}

int CExtract::RequestFile(LPTSTR lpszFileName)
{
	OPENFILENAME openfilename;
	openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = AfxGetMainWnd()->m_hWnd;
	openfilename.hInstance = AfxGetInstanceHandle();
	openfilename.lpstrFilter = NULL;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0;
	openfilename.nFilterIndex = 0;
	openfilename.lpstrFile = lpszFileName;
	openfilename.nMaxFile = MAX_PATH;
	openfilename.lpstrFileTitle = NULL;
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = NULL;
	openfilename.lpstrTitle = "Open next archive...";
	openfilename.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	openfilename.lpstrDefExt = NULL;
	openfilename.lCustData = 0;
	openfilename.lpfnHook = NULL;
	openfilename.lpTemplateName = NULL;
	return GetOpenFileName(&openfilename);
}

int CExtract::CountFiles(LPCTSTR sFilePath)
{
	int nFiles = 0;
	CString sFile = sFilePath;
	sFile = sFile.Left(sFile.ReverseFind('.')) + "*";
	CFileFind finder;
	BOOL bFound = finder.FindFile(sFile);
	while (bFound) {
		nFiles++;
		bFound = finder.FindNextFile();
	}

	return nFiles;
}
