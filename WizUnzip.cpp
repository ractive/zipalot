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

// WizUnzip.cpp : implementation file
//

#include "stdafx.h"
#include <imagehlp.h>
#include "zipalot.h"
#include "WizUnzip.h"
#include "Unzip.h"
#include "Unrar.h"
#include "Unace.h"
#include "WizSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_STARTNOW (WM_USER + 33)

extern HANDLE hStop;
extern CString GetSystemMessage(DWORD dwError);
extern CStringList g_sListCurfiles;
extern CStringList g_sListCurZIPfiles;

/////////////////////////////////////////////////////////////////////////////
// CWizUnzip property page

IMPLEMENT_DYNCREATE(CWizUnzip, CPropertyPage)

CWizUnzip::CWizUnzip() : CWizPage(CWizUnzip::IDD)
{
	//{{AFX_DATA_INIT(CWizUnzip)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWizUnzip::~CWizUnzip()
{
}

void CWizUnzip::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizUnzip)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_ANIMATE, m_Animation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizUnzip, CPropertyPage)
	//{{AFX_MSG_MAP(CWizUnzip)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EXTRACT_RANGE, OnRange)
	ON_MESSAGE(WM_EXTRACT_ERROR, OnError)
	ON_MESSAGE(WM_EXTRACT_STEPIT, OnStepIt)
	ON_MESSAGE(WM_EXTRACT_CURFILE, OnCurFile)
	ON_MESSAGE(WM_EXTRACT_CURZIPFILE, OnCurZIPFile)
	ON_MESSAGE(WM_STARTNOW, DoExtraction)
	ON_MESSAGE(WM_EXTRACT_SETSOURCE, OnSetSource)
	ON_MESSAGE(WM_EXTRACT_SETTARGET, OnSetTarget)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizUnzip message handlers

BOOL CWizUnzip::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizUnzip::OnSetActive() 
{
	tagWizData * pData = GetData();

	if (pData->m_sExtractFilter.IsEmpty()) {
		// Check if we can find archives...
		AnalyzeSourceDir();

		if (pData->m_sExtractFilter.IsEmpty()) {
			return FALSE;
		}
	}
	else {
		CString sExt = pData->m_sExtractFilter.Right(pData->m_sExtractFilter.GetLength() - pData->m_sExtractFilter.ReverseFind('.') - 1);
		if (sExt.CompareNoCase("zip") == 0) {
			pData->m_Type = ZIP;			
		}
		else if (sExt.CompareNoCase("ace") == 0) {
			pData->m_Type = ACE;			
		}
		else if (sExt.CompareNoCase("rar") == 0) {
			pData->m_Type = RAR;			
		}
	}

	m_Animation.Open(IDR_EXTRACT);

	((CPropertySheet *)GetParent())->SetWizardButtons(0);

	PostMessage(WM_STARTNOW, 0, 0);

	return CPropertyPage::OnSetActive();
}

void CWizUnzip::OnRange(UINT pParam, long lParam)
{
	m_Progress.SetStep(1);
	m_Progress.SetRange32(0, lParam);
	m_Progress.SetPos(0);
}

void CWizUnzip::OnCurFile(UINT pParam, long lParam)
{
	SetDlgItemText(IDC_CURFILE, GetFileName((LPTSTR)(LPCTSTR)g_sListCurfiles.RemoveHead()));
}

void CWizUnzip::OnSetSource(UINT pParam, long lParam)
{
	SetDlgItemText(IDC_WIZSOURCE, (LPTSTR)lParam);
}

void CWizUnzip::OnSetTarget(UINT pParam, long lParam)
{
	SetDlgItemText(IDC_WIZTARGET, (LPTSTR)lParam);
}

void CWizUnzip::OnCurZIPFile(UINT pParam, long lParam)
{
	SetDlgItemText(IDC_CURZIPFILE, GetFileName((LPTSTR)(LPCTSTR)g_sListCurZIPfiles.RemoveHead()));
}

void CWizUnzip::OnStepIt(UINT pParam, long lParam)
{
	m_Progress.StepIt();
}	

void CWizUnzip::OnError(UINT pParam, long lParam)
{
//	ErrorInCurFile((errorCode)lParam, (LPCTSTR)pParam);
	tagWizData * pData = GetData();
	pData->m_Error = (errorCode)lParam;
}

void CWizUnzip::DoExtraction(UINT pParam, long lParam)
{
	tagWizData * pData = GetData();

	UpdateData();

	if (!MakeSureDirectoryPathExists(*pData->m_psCurTarget + "\\")) {
		// The directory could not be created.
		DWORD dwError = GetLastError();
		CString sMsg = "The target directory could not be created\r\n\r\nThe Windows error message is:\r\n\r\n";
		sMsg += GetSystemMessage(dwError);
		MessageBox(sMsg, "Error", MB_ICONEXCLAMATION);
		return;
	}


	CExtract * pExtractor = NULL;

	// Process all the filters
	
	m_UnzipInfo.bCaseInsensitive = TRUE;
	m_UnzipInfo.bExtract = TRUE;
	m_UnzipInfo.bOnlyNewer = FALSE;
	m_UnzipInfo.bOverwrite = FALSE;
	m_UnzipInfo.bOwnDir = FALSE;
	m_UnzipInfo.bRecreateDir = TRUE;
	m_UnzipInfo.sDontExFilter = "";
	m_UnzipInfo.sExFilter = NULL;
	m_UnzipInfo.sFilter = NULL;
	m_UnzipInfo.sSource = (LPTSTR)(LPCTSTR)*pData->m_psCurSource;
	m_UnzipInfo.sTarget = (LPTSTR)(LPCTSTR)*pData->m_psCurTarget;

	
	//m_UnzipInfo.sFilter = "*.zip/*.rar/*.ace";
	m_UnzipInfo.sFilter = pData->m_sExtractFilter;

	errorCode nRet;

	switch (pData->m_Type) {
		case ZIP:
			pExtractor = new CUnzip(this);
			break;
		case RAR:
			pExtractor = new CUnrar(this);
			break;
		case ACE:
			pExtractor = new CUnace(this);
			break;
	}
	
	if (pExtractor == NULL) {
		((CPropertySheet *)GetParent())->SetActivePage(WIZ_ERROR);
		return;
	}
	
	
	pExtractor->Init();
	pExtractor->unzipInfo = m_UnzipInfo;

	pData->m_Error = EX_OK;

	nRet = pExtractor->ExtractAll(*pData->m_psCurSource, *pData->m_psCurTarget);

	delete pExtractor;
	pExtractor = NULL;

	if (nRet == EX_OK) {
		if(pData->m_nNextPage != -1) {
			int nNext = pData->m_nNextPage;
			pData->m_nNextPage = -1;
			((CPropertySheet *)GetParent())->SetActivePage(nNext);
		}
		else {
			((CPropertySheet *)GetParent())->SetActivePage(WIZ_NFO);
		}
		return;
	}
	else {
		pData->m_Error = nRet;
		//((CPropertySheet *)GetParent())->PressButton(PSBTN_NEXT);
		((CPropertySheet *)GetParent())->SetActivePage(WIZ_ERROR);
		((CPropertySheet *)GetParent())->PressButton(PSBTN_NEXT);
		InvalidateRect(NULL);
		UpdateWindow();
		((CPropertySheet *)GetParent())->InvalidateRect(NULL);
		((CPropertySheet *)GetParent())->UpdateWindow();

		return;
	}
}

LPCTSTR CWizUnzip::GetFileName(LPTSTR sFilePath)
{
	LPCTSTR s = strrchr(sFilePath, '\\');
	if (s == NULL) {
		// No backslash found
		return sFilePath;
	}
	// Skip the backslash
	return ++s;
}

void CWizUnzip::OnCancel() 
{
	
	//CPropertyPage::OnCancel();
}

BOOL CWizUnzip::OnQueryCancel() 
{
	SetEvent(hStop);	
	return CPropertyPage::OnQueryCancel();
}
