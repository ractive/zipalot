/*************************************************************************
                     ZipALot
**************************************************************************

Copyright (C) October, 2000 Jean-Pierre Bergamin, james@ractive.ch

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

// Unzip.cpp: implementation of the CUnzip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipALot.h"
#include "Unzip.h"
#include "PasswordDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern char curfile[_MAX_PATH];
extern CStringList g_sListCurfiles;
extern char curzipfile[_MAX_PATH];


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPCTSTR CUnzip::m_szCurZipFile = NULL;
//CUnzipDlg * CUnzip::pUnzipDlg = NULL;
CUnzip * CUnzip::m_pPseudoThis = NULL;

CUnzip::CUnzip(CDialog * pUnzipDlg) : CExtract(pUnzipDlg)
{
	UserFunctions.password = password;
	UserFunctions.print = DisplayBuf;
	UserFunctions.sound = Sound;
	UserFunctions.replace = GetReplaceDlgRetVal;
	UserFunctions.SendApplicationMessage = ReceiveDllMessage;
	UserFunctions.replace = Replace;
	UserFunctions.ServCallBk = ServCallBk;

	m_hDCL = NULL;
	m_lpDCL = NULL;

	m_pPseudoThis = this;
}

CUnzip::~CUnzip()
{
	if (m_hDCL != NULL) {
		GlobalFree(m_hDCL);
		m_lpDCL = NULL;
		m_hDCL = NULL;
	}

	FreeDLL();
}

int WINAPI CUnzip::ServCallBk(LPCSTR lpInfo, unsigned long size)
{
	if (m_pPseudoThis->m_pUnzipDlg) {
		g_sListCurfiles.AddTail(lpInfo);
		m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURFILE, 0, 0);
	}
	return 0;
}

void WINAPI CUnzip::Sound()
{
	//MessageBeep(MB_OK);
}

int WINAPI CUnzip::Replace(LPSTR lpFileName)
{
	return 0;
}

void WINAPI CUnzip::ReceiveDllMessage(unsigned long nUnpacked, unsigned long nPacked, unsigned int nRatio, unsigned int nMonth, unsigned int nDay, unsigned int nYear, unsigned int nHour, unsigned int nMinute, char i, LPSTR pszFileName, LPSTR k, unsigned long CRC, char cEncrypted)
{
	if (m_pPseudoThis->InsertFile != NULL) {

		CString sName = pszFileName;
		sName.Replace('/', '\\');
		if (m_pPseudoThis->unzipInfo.bOwnDir) {
			CString sFileName = m_szCurZipFile;
			sFileName = sFileName.Mid(sFileName.ReverseFind('\\') + 1);
			sFileName = sFileName.Left(sFileName.ReverseFind('.'));
			sName = sFileName + "\\" + sName;
		}

		// Build the date
		// Y2K fix ;-)
		if (nYear > 38) {
			nYear += 1900;
		}
		else {
			nYear += 2000;
		}

		CTime time(nYear, nMonth, nDay, nHour, nMinute, 00);

		SYSTEMTIME st;
		time.GetAsSystemTime(st);
		char sTime[15];
		char sDate[15];
		char sDateTime[31];
		sDateTime[0] = '\0';

		// Or use LOCALE_USER_DEFAULT???
		GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &st, NULL, sDate, 15);
		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_NOSECONDS, &st, NULL, sTime, 15);
		strcpy(sDateTime, sDate);
		strcat(sDateTime, " ");
		strcat(sDateTime, sTime);
		m_pPseudoThis->InsertFile(sName, nUnpacked, nPacked, nRatio, cEncrypted, sDateTime);
	}
}

int WINAPI CUnzip::password(char * szPassword, int n, const char * szMessage, const char * szFileName)
{
	if (m_szCurZipFile == NULL) {
		return IZ_PW_CANCELALL;
	}

	BOOL bForce = strcmp(szMessage, "Password incorrect--reenter: ") == 0;

	LPTSTR sPwd;
	int nRet = PromptPassword(&sPwd, szFileName, m_szCurZipFile, szMessage, TRUE);
	if (nRet == IDOK && strlen(sPwd) > 0) {
		strcpy(szPassword, sPwd);
		return IZ_PW_ENTERED;
	}
	else {
		return IZ_PW_CANCELALL;
	}
	return 0;
}

int WINAPI CUnzip::GetReplaceDlgRetVal(char *)
{
	return 0;
}

int WINAPI CUnzip::DisplayBuf(LPSTR buf, unsigned long size)
{
	return 0;
}

errorCode CUnzip::Test(LPCTSTR sFilePath)
{
	int nRet = Wiz_Validate((LPTSTR)sFilePath, TRUE);
	return GetErrorCode(nRet);
}

BOOL CUnzip::IsArchive(LPCTSTR sFilePath)
{
	int nRet = Wiz_Validate((LPTSTR)sFilePath, TRUE);
	return GetErrorCode(nRet) == EX_OK;
}

errorCode CUnzip::Extract(LPCTSTR sZIPFile, LPCTSTR sDestination)
{
	SetOptions();

	// The ZIP-file to extract
	m_lpDCL->lpszZipFN = (LPTSTR)sZIPFile;

	// The directory to extract to. This is set to NULL if you are extracting to the
	// current directory.
	m_lpDCL->lpszExtractDir = (LPTSTR)sDestination;

	m_szCurZipFile = sZIPFile;

	int nExtract = 0, nDontExtract = 0;


	char ** ppExFilter = NULL;
	char ** ppDontExFilter = NULL;

	CStringArray saExFilters;
	CStringArray saDontExFilters;
	
	// Now we get all the filters that are separated with "/" from the string

	int nPos = 0;
	// First we put all the entries in a StringArray
	if (unzipInfo.sExFilter != NULL) {
		CString sExFilter = unzipInfo.sExFilter;

		while ((nPos = sExFilter.Find('/')) != -1) {
			saExFilters.Add(sExFilter.Left(nPos));
			sExFilter.Delete(0, nPos + 1);
		}

		saExFilters.Add(sExFilter);
		
		int nFilter = saExFilters.GetSize();
		// Now create the char **

		ppExFilter = new char * [nFilter];
		
		for (int i = 0; i < nFilter; i++) {
			ppExFilter[i] = (LPTSTR)(LPCTSTR)saExFilters.GetAt(i);
		}

		nExtract = nFilter;
	}

	nPos = 0;
	// The same with sDontExFilter
	if (unzipInfo.sDontExFilter != NULL) {
		CString sDontExFilter = unzipInfo.sDontExFilter;
		while ((nPos = sDontExFilter.Find('/')) != -1) {
			saDontExFilters.Add(sDontExFilter.Left(nPos));
			sDontExFilter.Delete(0, nPos + 1);
		}

		saDontExFilters.Add(sDontExFilter);
		
		int nFilter = saDontExFilters.GetSize();
		// Now create the char **

		ppDontExFilter = new char * [nFilter];
		
		for (int i = 0; i < nFilter; i++) {
			ppDontExFilter[i] = (LPTSTR)(LPCTSTR)saDontExFilters.GetAt(i);
		}

		nDontExtract = nFilter;
	}


	int nRet = (*Wiz_SingleEntryUnzip)(nExtract, ppExFilter, nDontExtract, ppDontExFilter, m_lpDCL, &UserFunctions);
	m_szCurZipFile = NULL;

	if (ppExFilter != NULL) {
		delete [] ppExFilter;
	}

	if (ppDontExFilter != NULL) {
		delete [] ppDontExFilter;
	}

	return GetErrorCode(nRet);
}

errorCode CUnzip::GetErrorCode(int nError)
{
	errorCode error;

	switch (nError) {
	case PK_OK:
	case PK_WARN:          /* warning error */
		error = EX_OK;
		break;
	case PK_ERR:           /* error in zipfile */
	case PK_BADERR:        /* severe error in zipfile */
		error = EX_ARCHIVE;
		break;
	case PK_MEM:           /* insufficient memory (during initialization) */
	case PK_MEM2:          /* insufficient memory (password failure) */
	case PK_MEM3:          /* insufficient memory (file decompression) */
	case PK_MEM4:          /* insufficient memory (memory decompression) */
	case PK_MEM5:          /* insufficient memory (not yet used) */
		error = EX_MEM;
		break;
	case PK_NOZIP:         /* zipfile not found */
	case PK_FIND:          /* no files found */
		error = EX_NO_FILES;
		break;
	case PK_DISK:          /* disk full */
		error = EX_DISK;
		break;
	case PK_EOF:           /* unexpected EOF */
		error = EX_EOF;
		break;
	case IZ_CTRLC:          /* user hit ^C to terminate */
		error = EX_CANCELLED;
		break;
	case IZ_UNSUP:          /* no files found: all unsup. compr/encrypt. */
		error = EX_UNSUP;
		break;
	case IZ_BADPWD:         /* no files found: all had bad password */
		error = EX_BADPWD;
		break;
	case PK_PARAM:         /* bad or illegal parameters specified */
	default:
		error = EX_UNKNOWN;
	}

	return error;
}

BOOL CUnzip::SetOptions()
{
	if (m_hDCL != NULL) {
		return TRUE;
	}
	m_hDCL = GlobalAlloc(GPTR, (DWORD)sizeof(DCL));
	if (!m_hDCL) {
		return FALSE;
	}

	m_lpDCL = (LPDCL)GlobalLock(m_hDCL);
	if (!m_lpDCL) {
		GlobalFree(m_hDCL);
		return FALSE;
	}

	// Here is where the actual extraction process begins. First we set up the
	// flags to be passed into the dll.
	
	// true for "update" without interaction (extract only newer/new files, without queries)
	m_lpDCL->ExtractOnlyNewer = unzipInfo.bOnlyNewer;
	// true if convert space to underscore
	m_lpDCL->SpaceToUnderscore = 0;	
	// true if prompt to overwrite is wanted
	m_lpDCL->PromptToOverwrite = 0;
	// get zip info if true
	m_lpDCL->nZIflag = 0;
	// be case insensitive if TRUE
	m_lpDCL->C_flag = unzipInfo.bCaseInsensitive;
	
	//lpDCL->fPrivilege = 1;			// 1 => restore Acl's, 2 => Use privileges
	
	// Write to stdout if true
	m_lpDCL->ncflag = 0;
	
	m_lpDCL->fQuiet = 0;	// We want all messages.
							//		1 = fewer messages,
							//		2 = no messages 
	// test zip file if true
	m_lpDCL->ntflag = 0;
	// give a verbose listing if true
	m_lpDCL->nvflag = !unzipInfo.bExtract;
	// Do not extract only newer
	m_lpDCL->nUflag = !unzipInfo.bOnlyNewer;
	// display a zip file comment if true
	m_lpDCL->nzflag = 0;
	// Recreate directories if true
	m_lpDCL->ndflag = unzipInfo.bRecreateDir;
	// Over-write all files if true
	m_lpDCL->noflag = unzipInfo.bOverwrite;
	// Do not convert CR to CRLF
	m_lpDCL->naflag = 0;
	
	m_lpDCL->lpszZipFN = NULL;
	m_lpDCL->lpszExtractDir = NULL;

	return TRUE;	
}


BOOL CUnzip::GetProcAddresses(void)
{
	Wiz_SingleEntryUnzip =	(int (__stdcall *)(int, char **, int, char **, LPDCL, LPUSERFUNCTIONS))GetProcAddress(m_hDLL, "Wiz_SingleEntryUnzip");
	Wiz_Validate =	(int (__stdcall *)(LPTSTR sFilePath, int AllCodes))GetProcAddress(m_hDLL, "Wiz_Validate");

	return Wiz_SingleEntryUnzip && Wiz_Validate;
}

CString CUnzip::GetDLLName(void)
{
	return "UNZIP32.DLL";	
}