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

// Unace.h: interface for the CUnace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNACE_H__CFB12CCA_6DA8_43B5_902B_509E040968E3__INCLUDED_)
#define AFX_UNACE_H__CFB12CCA_6DA8_43B5_902B_509E040968E3__INCLUDED_

#include "ZipALot.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Extract.h"
#include "unace_.h"

class CUnace : public CExtract  
{
public:
	CUnace(CDialog * pUnzipDlg);
	virtual ~CUnace();
	virtual errorCode Extract(const LPCTSTR sFileName, const LPCTSTR sDestination);
	virtual errorCode Test(LPCTSTR sFilePath);
	virtual BOOL IsArchive(LPCTSTR sFilePath);

	CString GetDLLName(void);
protected:
	errorCode GetErrorCode(int nError);
	virtual BOOL DLLInit();
	static int HandleStateStartFile(pACECallbackArchivedFileStruc ArchivedFile);
	static int HandleStateStartArchive(pACECallbackArchiveStruc Archive);
	static int HandleErrorGlobal(pACECallbackGlobalStruc Error);
	static int HandleErrorArchive(pACECallbackArchiveStruc Error);
	static int HandleErrorArchivedFile(pACECallbackArchivedFileStruc Error);
	static int HandleRequestArchive(pACECallbackArchiveStruc Request);
	static int HandleRequestArchivedFile(pACECallbackArchivedFileStruc Request);
	virtual BOOL GetProcAddresses();
private:
	static BOOL m_bOverwriteReq;
	static errorCode m_OrigError;
	static int CancelStatus();
	static BOOL m_bQuiet;
	void BuildFileList(LPTSTR * sFileList, BOOL bInclude);
	int (__stdcall *ACEInitDllProc)(pACEInitDllStruc DllData);
	int (__stdcall *ACEReadArchiveDataProc)(LPSTR ArchiveName, pACEReadArchiveDataStruc ArchiveData);
	int (__stdcall *ACEListProc)(LPSTR ArchiveName, pACEListStruc List);
	int (__stdcall *ACETestProc)(LPSTR ArchiveName, pACETestStruc Test);
	int (__stdcall *ACEExtractProc)(LPSTR ArchiveName, pACEExtractStruc Extract);

	static int __stdcall ErrorProc(pACEErrorCallbackProcStruc Error);
	static int __stdcall RequestProc(pACERequestCallbackProcStruc Request);
	static int __stdcall StateProc(pACEStateCallbackProcStruc State);
	static int __stdcall InfoProc(pACEInfoCallbackProcStruc Info);

};

#endif // !defined(AFX_UNACE_H__CFB12CCA_6DA8_43B5_902B_509E040968E3__INCLUDED_)
