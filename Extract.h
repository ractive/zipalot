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

// Extract.h: interface for the CExtract class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTRACT_H__AE815976_6465_4039_AAFF_569876BE83D0__INCLUDED_)
#define AFX_EXTRACT_H__AE815976_6465_4039_AAFF_569876BE83D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UnzipDlg.h"
#include "unzipinfo.h"
#include "structs.h"

//typedef enum {SUCCESS, CANCELLED, NOFILES} ThreadRet;

#define PK_OK              0   /* no error */
#define PK_WARN            1   /* warning error */
#define PK_ERR             2   /* error in zipfile */
#define PK_BADERR          3   /* severe error in zipfile */
#define PK_MEM             4   /* insufficient memory (during initialization) */
#define PK_MEM2            5   /* insufficient memory (password failure) */
#define PK_MEM3            6   /* insufficient memory (file decompression) */
#define PK_MEM4            7   /* insufficient memory (memory decompression) */
#define PK_MEM5            8   /* insufficient memory (not yet used) */
#define PK_NOZIP           9   /* zipfile not found */
#define PK_PARAM          10   /* bad or illegal parameters specified */
#define PK_FIND           11   /* no files found */
#define PK_DISK           50   /* disk full */
#define PK_EOF            51   /* unexpected EOF */

#define IZ_CTRLC          80   /* user hit ^C to terminate */
#define IZ_UNSUP          81   /* no files found: all unsup. compr/encrypt. */
#define IZ_BADPWD         82   /* no files found: all had bad password */

/* return codes of password fetches (negative = user abort; positive = error) */
#define IZ_PW_ENTERED      0   /* got some password string; use/try it */
#define IZ_PW_CANCEL      -1   /* no password available (for this entry) */
#define IZ_PW_CANCELALL   -2   /* no password, skip any further pwd. request */
#define IZ_PW_ERROR        5   /* = PK_MEM2 : failure (no mem, no tty, ...) */

#define COMMENTBUFSIZE 8192

class CExtract  
{
public:
	static int PromptPassword(LPTSTR *lpszPassword, LPCTSTR lpszFileName, LPCTSTR lpszArchive, LPCTSTR lpszMessage, BOOL bForce = FALSE);
	static LPCTSTR GetFileName(LPCTSTR sFilePath);
	errorCode ExtractAll(const LPCTSTR sSource, const LPCTSTR sTarget);
	BOOL Init();
	
	virtual CString GetDLLName() = 0;
	virtual BOOL GetProcAddresses() = 0;

	virtual errorCode Extract(const LPCTSTR sFileName, const LPCTSTR sDestination) = 0;
	virtual int List(LPCTSTR sFileName, LPCTSTR sDestination);
	virtual errorCode Test(LPCTSTR sFilePath) = 0;
	virtual BOOL IsArchive(LPCTSTR sFilePath) = 0;

	virtual BOOL DLLInit();

	CExtract(CDialog * m_pUnzipDlg);
	virtual ~CExtract();
	BOOL FindDLL();

	// Callback file pointers
/*	void (*InsertFile)(CString sName, DWORD dwSize, DWORD dwPackedSize, int nRatio, char cEncrypted, CString sDateTime);
	void (*ShowCurFile)(LPCTSTR sFileName);
	void (*ShowCurZIPFile)(LPCTSTR sFileName);
	void (*SetRange)(UINT nRange);
	void (*Error)(CString sErrorMsg);
	void (*StepIt)();*/
	CDialog * m_pUnzipDlg;
	static tagUnzipInfo unzipInfo;

protected:
	int CountFiles(LPCTSTR sFilePath);
	static BOOL m_bPwdSet;
	static int RequestFile(LPTSTR lpszFileName);
	static void ReportError(errorCode error, LPCTSTR sFile = NULL);
	void FreeDLL();
	BOOL LoadDLL();
	
	HINSTANCE m_hDLL;

	char m_sCommentBuf[COMMENTBUFSIZE];

	CString m_sDLLName;
	static CExtract * m_pPseudoThis;

private:
	static CString m_sPassword;
	static UINT ExtractThreadProc(LPVOID lpParam);

};

#endif // !defined(AFX_EXTRACT_H__AE815976_6465_4039_AAFF_569876BE83D0__INCLUDED_)
