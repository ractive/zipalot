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

// Unzip.h: interface for the CUnzip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNZIP_H__501FE254_CCFB_11D3_837B_0000B4A265C3__INCLUDED_)
#define AFX_UNZIP_H__501FE254_CCFB_11D3_837B_0000B4A265C3__INCLUDED_

#include "structs.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int (WINAPI * _USER_FUNCTIONS)(LPUSERFUNCTIONS);

#include "Extract.h"

class CUnzip : public CExtract
{

public:
	BOOL SetOptions();
	errorCode GetErrorCode(int nError);
	virtual errorCode Extract(const LPCTSTR sFileName, const LPCTSTR sDestination);
	virtual errorCode Test(LPCTSTR sFilePath);
	virtual BOOL IsArchive(LPCTSTR sFilePath);

	CUnzip(CDialog * pUnzipDlg);
	virtual ~CUnzip();
	//void (*InsertFile)(CString sName, DWORD dwSize, DWORD dwPackedSize, int nRatio, char cEncrypted, CString sDateTime);
	CString GetDLLName();

private:
	HANDLE m_hDCL;
	DCL * m_lpDCL;
	CString m_sPassword;

	/*// Callback file pointers
	void (*ShowCurFile)(LPCTSTR sFileName);
	void (*ShowCurZIPFile)(LPCTSTR sFileName);
	void (*SetRange)(UINT nRange);
	void (*Error)(CString sErrorMsg);
	void (*StepIt)();*/

	static LPCTSTR m_szCurZipFile;

	// DLL callback functions
	static int WINAPI ServCallBk(LPCSTR lpInfo, unsigned long size);
	static int WINAPI Replace(LPSTR lpFileName);
	static void WINAPI Sound();
	static void WINAPI ReceiveDllMessage(unsigned long nUnpacked, unsigned long nPacked, unsigned int nRatio, unsigned int nMonth, unsigned int nDay, unsigned int nYear, unsigned int nHour, unsigned int nMinute, char i, LPSTR pszFileName, LPSTR k, unsigned long CRC, char cEncrypted);
	static int WINAPI GetReplaceDlgRetVal(char *);
	static int WINAPI password(char *, int, const char *, const char *);
	static int WINAPI DisplayBuf(LPSTR, unsigned long);

	int (__stdcall *Wiz_SingleEntryUnzip)(int, char **, int, char **, LPDCL, LPUSERFUNCTIONS);
	int	(__stdcall *Wiz_Validate)(LPTSTR, int);

	static CUnzip * m_pPseudoThis;


	USERFUNCTIONS UserFunctions;
//	DCL DCL;
protected:
	virtual BOOL GetProcAddresses(void);
};

#endif // !defined(AFX_UNZIP_H__501FE254_CCFB_11D3_837B_0000B4A265C3__INCLUDED_)
