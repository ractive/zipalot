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

// Unrar.cpp: implementation of the CUnrar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "zipalot.h"
#include "Unrar.h"
#include "RegExp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CStringList g_sListCurfiles;
extern CStringList g_sListCurZIPfiles;
extern char curzipfile[_MAX_PATH];
extern HANDLE hStop;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnrar::CUnrar(CDialog * pUnzipDlg) : CExtract(pUnzipDlg)
{

}

CUnrar::~CUnrar()
{

}

CString CUnrar::GetDLLName(void)
{
	return "UNRAR.DLL";	
}

BOOL CUnrar::GetProcAddresses()
{
	if (m_hDLL) {
		RAROpenArchive  =		(HANDLE (__stdcall *)(struct RAROpenArchiveData *ArchiveData))GetProcAddress(m_hDLL, "RAROpenArchive");
		RARCloseArchive =		(int	(__stdcall *)(HANDLE hArcData))GetProcAddress(m_hDLL, "RARCloseArchive");
		RARReadHeader =			(int	(__stdcall *)(HANDLE hArcData, struct RARHeaderData *HeaderData))GetProcAddress(m_hDLL, "RARReadHeader");
		RARProcessFile =		(int	(__stdcall *)(HANDLE hArcData, int Operation, char *DestPath, char *DestName))GetProcAddress(m_hDLL, "RARProcessFile");
		RARSetChangeVolProc =	(void	(__stdcall *)(HANDLE hArcData, int (*ChangeVolProc)(char *ArcName,int Mode)))GetProcAddress(m_hDLL, "RARSetChangeVolProc");
		RARSetProcessDataProc = (void	(__stdcall *)(HANDLE hArcData, int (*ProcessDataProc)(unsigned char *Addr,int Size)))GetProcAddress(m_hDLL, "RARSetProcessDataProc");
		RARSetPassword =		(void	(__stdcall *)(HANDLE hArcData, char *Password))GetProcAddress(m_hDLL, "RARSetPassword");
	}
	return RAROpenArchive && RARCloseArchive && RARReadHeader && RARProcessFile && RARSetChangeVolProc && RARSetProcessDataProc && RARSetPassword;
}

int CUnrar::ChangeVolProc(char *ArcName, int Mode)
{
	if (Mode == RAR_VOL_ASK) {
		int bRet = RequestFile(ArcName);
    
		if (bRet != IDOK) {
			return 0;
		}

	}
	else {
		g_sListCurZIPfiles.AddTail(GetFileName(ArcName));
		m_pPseudoThis->m_pUnzipDlg->SendMessage(WM_EXTRACT_CURZIPFILE, 0, 0);
		m_pPseudoThis->m_pUnzipDlg->SendMessage(WM_EXTRACT_STEPIT, 0, 0);
	}
	return 1;
}

int CUnrar::ProcessDataProc(unsigned char *Addr, int Size)
{
	return 1;
}

errorCode CUnrar::Test(LPCTSTR sFilePath)
{
	RAROpenArchiveData ArchiveData = {0};
	RARHeaderData HeaderData;

	ArchiveData.ArcName = (LPTSTR)sFilePath;
	ArchiveData.OpenMode = RAR_OM_EXTRACT;
	ArchiveData.CmtBufSize = COMMENTBUFSIZE;
	ArchiveData.CmtBuf = m_sCommentBuf;
	HANDLE hRARFile = RAROpenArchive(&ArchiveData);
	if (!hRARFile) {
		return EX_OPEN;
	}
	
	if (ArchiveData.OpenResult != 0) {
		return EX_OPEN;
	}

	RARSetChangeVolProc(hRARFile, ChangeVolProc);
    RARSetProcessDataProc(hRARFile,ProcessDataProc);

	HeaderData.CmtBuf=NULL;
	int RHCode, PFCode;
	char * sOEMStr = new char[_MAX_PATH];
	CharToOem("C:\\", (LPSTR)sOEMStr);
	while ((RHCode=RARReadHeader(hRARFile, &HeaderData))==0) {
		if (HeaderData.Flags & 0x04) {
			// The file is password protected
			LPTSTR lpszPwd;
			int nRet = PromptPassword(&lpszPwd, HeaderData.FileName, HeaderData.ArcName, "The file is password protected");
			if (nRet == IDOK && strlen(lpszPwd) > 0) {
				RARSetPassword(hRARFile, (LPTSTR)lpszPwd);
				m_bPwdSet = TRUE;
			}
			else if (nRet == IDCANCEL) {
				delete [] sOEMStr;
				return EX_CANCELLED;
			}
		}
		PFCode=RARProcessFile(hRARFile, RAR_TEST, sOEMStr, NULL);
		if (PFCode != 0) {
			LPCTSTR sFile;
			switch(PFCode) {
			case ERAR_END_ARCHIVE:
			case ERAR_BAD_ARCHIVE:
				sFile = GetFileName(HeaderData.ArcName);
				break;
			case ERAR_EOPEN:
			case ERAR_ECREATE:
			case ERAR_ECLOSE:
			case ERAR_EREAD:
			case ERAR_EWRITE:
				sFile = GetFileName(HeaderData.FileName);
				break;
			case ERAR_NO_MEMORY:
			case ERAR_BAD_DATA:
			case ERAR_UNKNOWN_FORMAT:
			case ERAR_SMALL_BUF:
			default:
				sFile = NULL;
				break;
			}
			ReportError(PFCode, sFile);
			if (PFCode == ERAR_EOPEN) {
				delete [] sOEMStr;
				return EX_OPEN;
			}
		}
	}

	delete [] sOEMStr;

	if (RHCode==ERAR_BAD_DATA) {
		ReportError(RHCode);		
	}

	RARCloseArchive(hRARFile);


	return EX_OK;
}

BOOL CUnrar::IsArchive(LPCTSTR sFilePath)
{
	RAROpenArchiveData ArchiveData = {0};

	ArchiveData.ArcName = (LPTSTR)sFilePath;
	ArchiveData.OpenMode = RAR_OM_EXTRACT;
	ArchiveData.CmtBufSize = COMMENTBUFSIZE;
	ArchiveData.CmtBuf = m_sCommentBuf;
	HANDLE hRARFile = RAROpenArchive(&ArchiveData);
	if (!hRARFile) {
		return FALSE;
	}
	RARCloseArchive(hRARFile);

	return TRUE;
}

errorCode CUnrar::Extract(const LPCTSTR sFileName, const LPCTSTR sDestination)
{
	CString sD = sDestination;
	if (sD.Right(1) != "\\") {
		sD += "\\";
	}
	
	RAROpenArchiveData ArchiveData = {0};
	RARHeaderData HeaderData;

	ArchiveData.ArcName = (LPTSTR)sFileName;
	ArchiveData.OpenMode = RAR_OM_EXTRACT;
	ArchiveData.CmtBufSize = COMMENTBUFSIZE;
	ArchiveData.CmtBuf = m_sCommentBuf;
	HANDLE hRARFile = RAROpenArchive(&ArchiveData);
	if (!hRARFile) {
		return EX_OPEN;
	}
	
	if (ArchiveData.OpenResult != 0) {
		return EX_OPEN;
	}

	RARSetChangeVolProc(hRARFile, ChangeVolProc);
    RARSetProcessDataProc(hRARFile,ProcessDataProc);

	HeaderData.CmtBuf=NULL;
	int RHCode, PFCode;

	while ((RHCode=RARReadHeader(hRARFile, &HeaderData))==0) {
		if (WaitForSingleObject(hStop, 0) == WAIT_OBJECT_0) {
			RARCloseArchive(hRARFile);
			return EX_CANCELLED;
		}
		if (CheckForPassword(hRARFile, &HeaderData) == EX_CANCELLED) {
			RARCloseArchive(hRARFile);
			return EX_CANCELLED;
		}
		// We assume that everything is extracted
		BOOL bExtract = TRUE;
		BOOL bIsDir = HeaderData.Flags & 0x38;
		

		CString sFinalDest = sD;
		GetDestName(sFinalDest, HeaderData.FileName);
		
		bExtract = CanOverwrite(sFinalDest, HeaderData.FileTime);

		if (bExtract && unzipInfo.sExFilter != NULL) {
			bExtract = FALSE;
			TCHAR seps[] = "/";
			TCHAR * token = strtok((LPTSTR)unzipInfo.sExFilter, seps);
			while( token != NULL )
			{
				CRegExp regExp;
				CString sExpr = token;
				sExpr.Replace(".", "\\.");
				sExpr.Replace("*", ".*");
				sExpr.Replace("?", ".?");

				if (regExp.RegComp(sExpr) != NULL) {
					if (regExp.RegFind(HeaderData.FileName) != -1) {
						// This filter matches -> Extract the file
						// Don't process the other filters
						bExtract = TRUE;
						break;
					}
				}			
				// Get next filter
				token = strtok( NULL, seps );			
			}
			
			if (unzipInfo.sDontExFilter != NULL) {
				token = strtok((LPTSTR)unzipInfo.sDontExFilter, seps);
				while( token != NULL )
				{
					CRegExp regExp;
					if (regExp.RegComp(token) != NULL) {
						if (regExp.RegFind(HeaderData.FileName) == -1) {
							// This filter matches -> DON'T extract the file
							// Don't process the other filters
							bExtract = FALSE;
							break;
						}
					}			
					// Get next filter
					token = strtok( NULL, seps );			
				}
			}

		}

		int nOp = 0;
		
		
		if (bExtract) {
			nOp = unzipInfo.bExtract ? RAR_EXTRACT : RAR_TEST;
			if (bIsDir) {
				nOp = unzipInfo.bRecreateDir ? RAR_EXTRACT : RAR_SKIP;
			}
			else {
				g_sListCurfiles.AddTail(GetFileName(HeaderData.FileName));
				m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURFILE, 0, 0);
				if (m_pPseudoThis->unzipInfo.bExtract == FALSE) {
					if (m_pPseudoThis->InsertFile != NULL) {
						FILETIME ft;
						DosDateTimeToFileTime(HIWORD(HeaderData.FileTime), LOWORD(HeaderData.FileTime), &ft);
						SYSTEMTIME st;
						FileTimeToSystemTime(&ft, &st);


						//time.GetAsSystemTime(st);
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

						m_pPseudoThis->InsertFile(GetFileName(HeaderData.FileName), HeaderData.UnpSize, HeaderData.PackSize, 1, 'c', sDateTime);
					}	
				}
			}
		}
		else {
			nOp = RAR_SKIP;
		}
		
		PFCode=RARProcessFile(hRARFile, nOp, NULL, (LPTSTR)(LPCTSTR)sFinalDest);
		if (bExtract && PFCode != 0) {
			LPCTSTR sFile;
			switch(PFCode) {
			case ERAR_END_ARCHIVE:
			case ERAR_BAD_ARCHIVE:
				sFile = GetFileName(HeaderData.ArcName);
				break;
			case ERAR_EOPEN:
			case ERAR_ECREATE:
			case ERAR_ECLOSE:
			case ERAR_EREAD:
			case ERAR_EWRITE:
				sFile = GetFileName(HeaderData.FileName);
				break;
			case ERAR_NO_MEMORY:
			case ERAR_BAD_DATA:
			case ERAR_UNKNOWN_FORMAT:
			case ERAR_SMALL_BUF:
			default:
				sFile = NULL;
				break;
			}
			ReportError(PFCode, sFile);
			if (PFCode == ERAR_EOPEN) {
				return EX_OPEN;
			}
		}
	}

	if (RHCode==ERAR_BAD_DATA) {
		ReportError(RHCode);		
	}

	RARCloseArchive(hRARFile);


	return EX_OK;
}

void CUnrar::ReportError(int nError, LPCTSTR sFile /* = NULL */)
{
	errorCode error;
	switch(nError) {
		case ERAR_END_ARCHIVE:
			error = EX_EOF;
			break;
		case ERAR_NO_MEMORY:
			error = EX_MEM;
			break;
		case ERAR_BAD_DATA: {
			if (m_bPwdSet) {
				error = EX_BADPWD;
			}
			else {
				error = EX_CRC;
			}
			break;
		}
		case ERAR_BAD_ARCHIVE:
			error = EX_ARCHIVE;
			break;
		case ERAR_UNKNOWN_FORMAT:
			error = EX_UNSUP;
			break;
		case ERAR_EOPEN:
			error = EX_OPEN;
			break;
		case ERAR_ECREATE:
			error = EX_CREATE;
			break;
		case ERAR_ECLOSE:
			error = EX_CLOSE;
			break;
		case ERAR_EREAD:
			error = EX_READ;
			break;
		case ERAR_EWRITE:
			error = EX_WRITE;
			break;
		case ERAR_SMALL_BUF:
			error = EX_BUF;
			break;
		default:
			error = EX_UNKNOWN;
	}

	CExtract::ReportError(error, sFile);
}

int CUnrar::CheckForPassword(HANDLE hRARFile, RARHeaderData *HeaderData)
{
	if (HeaderData->Flags & 0x04) {
		// The file is password protected
		LPTSTR lpszPwd;
		int nRet = PromptPassword(&lpszPwd, HeaderData->FileName, HeaderData->ArcName, "The file is password protected");
		if (nRet == IDOK && strlen(lpszPwd) > 0) {
			RARSetPassword(hRARFile, (LPTSTR)lpszPwd);
			m_bPwdSet = TRUE;
		}
		else if (nRet == IDCANCEL) {
			return EX_CANCELLED;
		}
	}

	return EX_OK;
}

void CUnrar::GetDestName(CString &sDest, LPCTSTR sFileName)
{
	if (unzipInfo.bRecreateDir) {
		sDest += sFileName;
	}
	else {
		CString sName = sFileName;
		sName = sName.Right(sName.GetLength() - sName.ReverseFind('\\') - 1);
		sDest += sName;
	}
}

