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

// Unrar.h: interface for the CUnrar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNRAR_H__6D6A9012_0616_42A7_B3F6_0E8B973C456B__INCLUDED_)
#define AFX_UNRAR_H__6D6A9012_0616_42A7_B3F6_0E8B973C456B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Extract.h"
#include "unrar_.h"

class CUnrar : public CExtract  
{
public:
	virtual BOOL GetProcAddresses(void);
	CUnrar(CDialog * pUnzipDlg);
	CString GetDLLName(void);
	virtual ~CUnrar();
	virtual errorCode Extract(const LPCTSTR sFileName, const LPCTSTR sDestination);
	virtual errorCode Test(LPCTSTR sFilePath);
	virtual BOOL IsArchive(LPCTSTR sFilePath);
	
private:
	void GetDestName(CString &sDest, LPCTSTR sFileName);
	int CheckForPassword(HANDLE hRarFile, RARHeaderData * HeaderData);
	static void ReportError(int error, LPCTSTR sFile = NULL);
	static int ProcessDataProc(unsigned char *Addr,int Size);
	static int ChangeVolProc(char *ArcName,int Mode);

	HANDLE (__stdcall *RAROpenArchive)(RAROpenArchiveData *ArchiveData);

	int	(__stdcall *RARCloseArchive)(HANDLE hArcData);
	int (__stdcall *RARReadHeader)(HANDLE hArcData, RARHeaderData *HeaderData);
	int	(__stdcall *RARProcessFile)(HANDLE hArcData, int Operation, char *DestPath, char *DestName);
	void	(__stdcall *RARSetChangeVolProc)(HANDLE hArcData, int (*ChangeVolProc)(char *ArcName,int Mode));
	void	(__stdcall *RARSetProcessDataProc)(HANDLE hArcData, int (*ProcessDataProc)(unsigned char *Addr,int Size));
	void	(__stdcall *RARSetPassword)(HANDLE hArcData, char *Password);
};

#endif // !defined(AFX_UNRAR_H__6D6A9012_0616_42A7_B3F6_0E8B973C456B__INCLUDED_)
