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

// Unace.cpp: implementation of the CUnace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "zipalot.h"
#include "Unace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL CUnace::m_bQuiet = FALSE;
errorCode CUnace::m_OrigError = EX_OK;
extern HANDLE hStop;
extern CStringList g_sListCurZIPfiles;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//extern char curfile[_MAX_PATH];
extern CStringList g_sListCurfiles;
extern CStringList g_sListCurZIPfile;
extern char curzipfile[_MAX_PATH];

CUnace::CUnace(CDialog * pUnzipDlg) : CExtract(pUnzipDlg)
{
}

CUnace::~CUnace()
{

}



void CUnace::BuildFileList(LPTSTR * ppsFileList, BOOL bInclude)
{
	LPCTSTR sFilter = NULL;
	if (bInclude) {
		sFilter = unzipInfo.sExFilter;
	}
	else {
		sFilter = unzipInfo.sDontExFilter;
	}


	if (sFilter == NULL) {
		if (bInclude) {
			// Extract all if nothing is set
			*ppsFileList = "*";		
		}
		else {
			// Don't exclude anyting is nothing is set
			*ppsFileList = "";	
		}

		return;
	}

	CString sF(sFilter);
	sF.Replace('/', 0xd);

	*ppsFileList = (LPTSTR)(LPCTSTR)sF;
}

errorCode CUnace::Test(LPCTSTR sFilePath)
{
	m_bQuiet = TRUE;
	tACETestStruc Test = {0};

	Test.Files.SourceDir   = "";        // archive main directory is
									  // base directory for FileList
	Test.Files.FileList= "*";      // set FileList
	Test.Files.ExcludeList = "";        // no files to exclude
	Test.Files.FullMatch   = 0;         // also test files partially matching

	Test.DecryptPassword   = "";        // no encrypted file expected

	int nRet = ACETestProc((LPTSTR)sFilePath, &Test);

	return GetErrorCode(nRet);
}

BOOL CUnace::IsArchive(LPCTSTR sFilePath)
{
	m_bQuiet = TRUE;
	tACETestStruc Test = {0};

	Test.Files.SourceDir   = "";        // archive main directory is
									  // base directory for FileList
	Test.Files.FileList= "*";      // set FileList
	Test.Files.ExcludeList = "";        // no files to exclude
	Test.Files.FullMatch   = 0;         // also test files partially matching

	Test.DecryptPassword   = "";        // no encrypted file expected

	int nRet = ACETestProc((LPTSTR)sFilePath, &Test);

	return GetErrorCode(nRet) == EX_OK;
}


errorCode CUnace::Extract(const LPCTSTR sFileName, const LPCTSTR sDestination)
{
	m_bQuiet = FALSE;
	tACEExtractStruc Extract = {0};
	CString sInc, sEx;

	if (unzipInfo.sExFilter == NULL) {
		Extract.Files.FileList = "*";
	}
	else {
		sInc = unzipInfo.sExFilter;
		sInc.Replace('/', 0xd);
		Extract.Files.FileList = (LPTSTR)(LPCTSTR)sInc;
	}

	if (unzipInfo.sDontExFilter == NULL) {
		Extract.Files.ExcludeList = "";
	}
	else {
		sEx = unzipInfo.sDontExFilter;
		sEx.Replace('/', 0xd);
		Extract.Files.ExcludeList = (LPTSTR)(LPCTSTR)sEx;
	}

	Extract.Files.SourceDir      = "";      // archive main directory is
											// base dir for FileList
	Extract.Files.FullMatch      = 0;       // also extract files
											// partially matching

	Extract.DestinationDir       = (LPTSTR)(LPCTSTR)sDestination; // directory to extract to
	Extract.ExcludePath          = !unzipInfo.bRecreateDir;  // extract files with path?
	Extract.DecryptPassword      = "";             // no encrypted file expected
	
	m_OrigError = EX_OK;

	int nRet = ACEExtractProc((LPTSTR)(LPCTSTR)sFileName, &Extract);

	if (m_OrigError != EX_OK) {
		return m_OrigError;
	}

	return GetErrorCode(nRet);
}

BOOL CUnace::GetProcAddresses(void)
{
	if (m_hDLL)	{
		if (!(ACEInitDllProc               = (int (__stdcall *)(pACEInitDllStruc DllData))
				  GetProcAddress(m_hDLL, "ACEInitDll"))
			|| !(ACEReadArchiveDataProc    = (int (__stdcall *)(LPSTR ArchiveName, pACEReadArchiveDataStruc ArchiveData))
				  GetProcAddress(m_hDLL, "ACEReadArchiveData"))
			|| !(ACEListProc               = (int (__stdcall *)(LPSTR ArchiveName, pACEListStruc List))
				  GetProcAddress(m_hDLL, "ACEList"))
			|| !(ACETestProc               = (int (__stdcall *)(LPSTR ArchiveName, pACETestStruc Test))
				  GetProcAddress(m_hDLL, "ACETest"))
			|| !(ACEExtractProc            =  (int (__stdcall *)(LPSTR ArchiveName, pACEExtractStruc Extract))
				  GetProcAddress(m_hDLL, "ACEExtract"))) {
		  TRACE("\nCould not find function in UnAceV2.Dll!\n\n");
		  FreeDLL();

		  return FALSE;
		}
	}

	return TRUE;
}

CString CUnace::GetDLLName(void)
{
	return "UNACEV2.DLL";	
}


/*-----------------HandleStateStartArchive-------------------------------*/

int CUnace::HandleStateStartArchive(pACECallbackArchiveStruc Archive)
{
#ifdef _DEBUG	
	LPSTR     ActionStr;

	switch (Archive->Operation)
	{
		case ACE_CALLBACK_OPERATION_LIST:
		{
		  ActionStr = "Listing %s\n\n";

		  break;
		}

		case ACE_CALLBACK_OPERATION_TEST:
		{
		  ActionStr = "Testing %s\n\n";

		  break;
		}

		case ACE_CALLBACK_OPERATION_EXTRACT:
		{
		  ActionStr = "Extracting %s\n\n";

		  break;
		}

		default:
		{
		  ActionStr = "unknown operation on %s";

		  break;
		}
	}

	//TRACE(ActionStr, Archive->ArchiveData->ArchiveName);
#endif
	/*if (!m_bQuiet && m_pPseudoThis->m_pUnzipDlg) {
		CString sFileName = Archive->ArchiveData->ArchiveName;
		sFileName = sFileName.Right(sFileName.GetLength() - sFileName.ReverseFind('\\') - 1);
		strcpy(curfile, Archive->ArchiveData->ArchiveName);
		m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURFILE, 0, (long)curfile);
	}*/

	return CancelStatus();
}

/*-----------------HandleStateStartFile----------------------------------*/

int CUnace::HandleStateStartFile(pACECallbackArchivedFileStruc ArchivedFile)
{
#ifdef _DEBUG	

	LPSTR     ActionStr;

	switch (ArchivedFile->Operation)
	{
		case ACE_CALLBACK_OPERATION_LIST:
		{
		  ActionStr = "Found";

		  break;
		}

		case ACE_CALLBACK_OPERATION_TEST:
		{
		  ActionStr = "Testing";

		  break;
		}

		case ACE_CALLBACK_OPERATION_ANALYZE:
		{
		  ActionStr = "Analyzing";

		  break;
		}

		case ACE_CALLBACK_OPERATION_EXTRACT:
		{
		  ActionStr = "Extracting";
		  break;
		}

		default:
		{
		  ActionStr = "unknown operation on";

		  break;
		}
	}

/*	TRACE("  %s %s  (%I64d byte uncompressed, %I64d byte compressed)\n",
		 ActionStr, ArchivedFile->FileData->SourceFileName,
		 ArchivedFile->FileData->Size,
		 ArchivedFile->FileData->CompressedSize);*/
#endif
	if (!m_bQuiet && m_pPseudoThis->m_pUnzipDlg) {
		g_sListCurfiles.AddTail(ArchivedFile->FileData->SourceFileName);
		//m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURFILE, 0, (long)ArchivedFile->FileData->SourceFileName);
		m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURFILE, 0, 0);
	}

	return CancelStatus();
}

int CUnace::HandleErrorGlobal(pACECallbackGlobalStruc Error)
{
	int nResult = ACE_CALLBACK_RETURN_OK;
	errorCode error;

	switch (Error->Code) {
		case ACE_CALLBACK_ERROR_MEMORY:
			error = EX_MEM;
			break;
		case ACE_CALLBACK_ERROR_UNCSPACE:
			error = EX_DISK;
			break;

		default:
		  error = EX_UNKNOWN;
		  nResult = ACE_CALLBACK_RETURN_CANCEL;
		  break;
	}
	m_OrigError = error;

	CExtract::ReportError(error);

	return nResult;
}

int CUnace::HandleErrorArchive(pACECallbackArchiveStruc Error)
{
	int nResult = ACE_CALLBACK_RETURN_OK;
	errorCode error;

	switch (Error->Code) {
		case ACE_CALLBACK_ERROR_AV:
			error = EX_ARCHIVE;
			break;

		case ACE_CALLBACK_ERROR_OPENARCHIVEREAD:
			error = EX_OPEN;
			break;

		case ACE_CALLBACK_ERROR_READARCHIVE:
			error = EX_READ;
			break;

		case ACE_CALLBACK_ERROR_ARCHIVEBROKEN:
			error = EX_ARCHIVE;
			break;

		case ACE_CALLBACK_ERROR_NOFILES:
			error = EX_NO_FILES;
			break;

		case ACE_CALLBACK_ERROR_ISNOTANARCHIVE:
			error = EX_UNSUP;
			break;

		case ACE_CALLBACK_ERROR_HIGHERVERSION:
			error = EX_UNSUP;
			break;

		default:
			error = EX_UNKNOWN;
			nResult   = ACE_CALLBACK_RETURN_CANCEL;
			break;
	}
	if (!m_bQuiet) {
		CExtract::ReportError(error, GetFileName(Error->ArchiveData->ArchiveName));
	}

	m_OrigError = error;

	return nResult;
}

int CUnace::HandleErrorArchivedFile(pACECallbackArchivedFileStruc Error)
{
	int nResult = ACE_CALLBACK_RETURN_OK;
	errorCode error;

	switch (Error->Code) {
		case ACE_CALLBACK_ERROR_CREATIONNAMEINUSE:
			error = EX_CREATE;
			break;

		case ACE_CALLBACK_ERROR_WRITE:
			error = EX_WRITE;
			break;

		case ACE_CALLBACK_ERROR_OPENWRITE:
			error = EX_OPEN;
			break;

		case ACE_CALLBACK_ERROR_METHOD:
			error = EX_UNSUP;
			break;

		case ACE_CALLBACK_ERROR_EXTRACTSPACE:
			error = EX_DISK;
			nResult   = ACE_CALLBACK_RETURN_CANCEL;
			break;

		case ACE_CALLBACK_ERROR_CREATION:
			error = EX_CREATE;
			break;

		default:
			error = EX_UNKNOWN;
			nResult   = ACE_CALLBACK_RETURN_CANCEL;
			break;
	}

	m_OrigError = error;
	CExtract::ReportError(error, GetFileName(Error->FileData->SourceFileName));

	return nResult;
}


int __stdcall CUnace::ErrorProc(pACEErrorCallbackProcStruc Error)
{
	switch (Error->StructureType) {
		case ACE_CALLBACK_TYPE_GLOBAL:
			return HandleErrorGlobal(&Error->Global);

		case ACE_CALLBACK_TYPE_ARCHIVE:
			return HandleErrorArchive(&Error->Archive);

		case ACE_CALLBACK_TYPE_ARCHIVEDFILE:
			return HandleErrorArchivedFile(&Error->ArchivedFile);

		case ACE_CALLBACK_TYPE_REALFILE:
		case ACE_CALLBACK_TYPE_SPACE:
		case ACE_CALLBACK_TYPE_SFXFILE:
		default:
			ReportError(EX_UNKNOWN);
			return ACE_CALLBACK_RETURN_CANCEL;
	}
}

int CUnace::HandleRequestArchive(pACECallbackArchiveStruc Request)
{
	int nResult = ACE_CALLBACK_RETURN_OK;

	switch (Request->Code)
	{
		case ACE_CALLBACK_REQUEST_CHANGEVOLUME: {
			if (m_bQuiet || m_pPseudoThis->m_pUnzipDlg == NULL) {
				return ACE_CALLBACK_RETURN_OK;
			}
			else {
				CFileFind finder;
				if (!finder.FindFile(Request->ArchiveData->ArchiveName)) {
					if (RequestFile(Request->ArchiveData->ArchiveName) != IDOK) {
						nResult = ACE_CALLBACK_RETURN_CANCEL;
						break;
					}
				}
				LPCTSTR lpszFileName = GetFileName(Request->ArchiveData->ArchiveName);
				g_sListCurZIPfiles.AddTail(lpszFileName);
				m_pPseudoThis->m_pUnzipDlg->PostMessage(WM_EXTRACT_CURZIPFILE, 0, 0);
				m_pPseudoThis->m_pUnzipDlg->SendMessage(WM_EXTRACT_STEPIT, 0, 0);
				break;
			}
		}
		default:
			nResult = ACE_CALLBACK_RETURN_CANCEL;
	}

	return nResult;
}

int CUnace::HandleRequestArchivedFile(pACECallbackArchivedFileStruc Request)
{
	int nResult = ACE_CALLBACK_RETURN_OK;

	switch (Request->Code) {
		case ACE_CALLBACK_REQUEST_OVERWRITE:
		case ACE_CALLBACK_REQUEST_OVERWRITESYSFILE:
			// Hmmm? Strange naming of the return values...
			nResult = unzipInfo.bOverwrite ? ACE_CALLBACK_RETURN_OK : ACE_CALLBACK_RETURN_NO;
		
			break;

		case ACE_CALLBACK_REQUEST_PASSWORD:
		{
			// The file is password protected
			int nRet = PromptPassword(&Request->GlobalData->DecryptPassword, Request->FileData->SourceFileName, Request->ArchiveData->ArchiveName, "The file is password protected");
			if (nRet == IDOK && strlen(Request->GlobalData->DecryptPassword) > 0) {
				m_bPwdSet = TRUE;
			}			
			else if (nRet == IDCANCEL) {
				return ACE_CALLBACK_RETURN_CANCEL;
			}
			break;
		}

		default:
		  return ACE_CALLBACK_RETURN_CANCEL;
	}

	return nResult;
}

int __stdcall CUnace::RequestProc(pACERequestCallbackProcStruc Request)
{
	switch (Request->StructureType)	{
		case ACE_CALLBACK_TYPE_ARCHIVE: {
			return HandleRequestArchive(&Request->Archive);
		}

		case ACE_CALLBACK_TYPE_ARCHIVEDFILE: {
			return HandleRequestArchivedFile(&Request->ArchivedFile);
		}

		case ACE_CALLBACK_TYPE_GLOBAL:
		case ACE_CALLBACK_TYPE_REALFILE:
		default:
			return ACE_CALLBACK_RETURN_CANCEL;
	}
}

int __stdcall CUnace::StateProc(pACEStateCallbackProcStruc State)
{
	switch (State->StructureType) {
		case ACE_CALLBACK_TYPE_ARCHIVE:	{
			if (State->Archive.Code == ACE_CALLBACK_STATE_STARTARCHIVE) {
				return HandleStateStartArchive(&State->Archive);
			}

			break;
		}

		case ACE_CALLBACK_TYPE_ARCHIVEDFILE: {
			switch (State->ArchivedFile.Code) {
				case ACE_CALLBACK_STATE_STARTFILE: {
					return HandleStateStartFile(&State->ArchivedFile);
				}

				case ACE_CALLBACK_STATE_ENDNOCRCCHECK: {

				}
			}

			break;
		}

		case ACE_CALLBACK_TYPE_PROGRESS: {
			if (State->Progress.Code == ACE_CALLBACK_STATE_PROGRESS) {
/*				TRACE("    processed: %I64d of %I64d bytes (%I64d of %I64d bytes)\n",
					   State->Progress.ProgressData->FileProcessedSize,
					   State->Progress.ProgressData->FileSize,
					   State->Progress.ProgressData->TotalProcessedSize,
					   State->Progress.ProgressData->TotalSize);*/
			}

			break;
		}

		case ACE_CALLBACK_TYPE_CRCCHECK: {
			if (State->CRCCheck.Code == ACE_CALLBACK_STATE_ENDCRCCHECK) {
				if(!State->CRCCheck.CRCOk) {
					ReportError(m_bPwdSet ? EX_BADPWD : EX_CRC, GetFileName(State->ArchivedFile.FileData->SourceFileName));
				}
			}

			break;
		}
	}

	return CancelStatus();
}

int __stdcall CUnace::InfoProc(pACEInfoCallbackProcStruc Info)
{
#ifdef _DEBUG
	LPSTR     InfoStr;

	switch (Info->Global.Code) {
		case ACE_CALLBACK_INFO_FILELISTCREATE:
		{
			InfoStr = "creating file list";

			break;
		}

		case ACE_CALLBACK_INFO_FILELISTCREATEEND:
		{
			InfoStr = "finished creating file list";

			break;
		}

		case ACE_CALLBACK_INFO_FILELISTADD:
		{
			InfoStr = "adding file to file list";

			break;
		}

		default:
		{
			InfoStr = "";

			break;
		}
	}

//	TRACE("Info: %s\n", InfoStr);
#endif
	return CancelStatus();
}

BOOL CUnace::DLLInit()
{
	tACEInitDllStruc DllData = {0};

	DllData.GlobalData.Obj = m_pUnzipDlg;
	DllData.GlobalData.MaxArchiveTestBytes = 0x1ffFF; // search for archive
													// header in first 128k
													// of file
	DllData.GlobalData.MaxFileBufSize      = 0x2ffFF; // read/write buffer size
													// is 256k

	DllData.GlobalData.Comment.BufSize     = sizeof(m_sCommentBuf);
	DllData.GlobalData.Comment.Buf         = m_sCommentBuf; // set comment bufffer
													   // to receive comments
													   // of archive and/or
													   // set comments

	DllData.GlobalData.TempDir             = "C:\\TEMP"; // set temp dir

	// set callback function pointers
	DllData.GlobalData.InfoCallbackProc    = InfoProc;
	DllData.GlobalData.ErrorCallbackProc   = ErrorProc;
	DllData.GlobalData.RequestCallbackProc = RequestProc;
	DllData.GlobalData.StateCallbackProc   = StateProc;

	return ACEInitDllProc(&DllData) == ACE_CALLBACK_RETURN_OK;
}


errorCode CUnace::GetErrorCode(int nError)
{
	errorCode error;

	switch(nError) {
		case ACE_ERROR_NOERROR:
			error = EX_OK;
			break;
		case ACE_ERROR_MEM:
			error = EX_MEM;
			break;	
		case ACE_ERROR_FILES:
			error = EX_NO_FILES;
			break;	
		case ACE_ERROR_FOUND:
			error = EX_NO_ARCHIVES;
			break;	
		case ACE_ERROR_FULL:
			error = EX_DISK;
			break;	
		case ACE_ERROR_OPEN:
			error = EX_OPEN;
			break;
		case ACE_ERROR_READ:
			error = EX_READ;
			break;	
		case ACE_ERROR_WRITE:
			error = EX_WRITE;
			break;	
		case ACE_ERROR_CLINE:
			error = EX_UNKNOWN;
			break;	
		case ACE_ERROR_CRC:
			error = EX_CRC;
			break;	
		case ACE_ERROR_OTHER:
			error = EX_UNKNOWN;
			break;	
		case ACE_ERROR_EXISTS:
			error = EX_EXISTS;
			break;
		case ACE_ERROR_USER:
			error =  EX_CANCELLED;
			break;
		default:
			error = EX_UNKNOWN;
	}

	return error;
}

int CUnace::CancelStatus()
{
	if (WaitForSingleObject(hStop, 0) == WAIT_OBJECT_0) {
		return ACE_CALLBACK_RETURN_CANCEL;
	}
	
	return ACE_CALLBACK_RETURN_OK;
}
