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

// UnzipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZipALot.h"
#include "Unzip.h"
#include "UnzipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnzipDlg dialog

extern HANDLE hStop;
extern CStringList g_sListCurfiles;
extern CStringList g_sListCurZIPfiles;

CUnzipDlg * CUnzipDlg::m_pPseudoThis = 0;


CUnzipDlg::CUnzipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnzipDlg::IDD, pParent), m_Mutex(FALSE, NULL)
{
	//{{AFX_DATA_INIT(CUnzipDlg)
	m_pbCloseWhenDone = NULL;
	//}}AFX_DATA_INIT

	m_pPseudoThis = this;
	m_htiCurFile = NULL;
}


void CUnzipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnzipDlg)
	DDX_Control(pDX, IDC_DETAILGROUP, m_DetailGroup);
	DDX_Control(pDX, IDC_FILETREE, m_FileTree);
	DDX_Control(pDX, IDC_ANIMATION, m_Animation);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Check(pDX, IDC_CLOSE, *m_pbCloseWhenDone);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnzipDlg, CDialog)
	//{{AFX_MSG_MAP(CUnzipDlg)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SHOWDETAILS, OnShowDetails)
	ON_BN_CLICKED(IDC_CANCEL, OnClose)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_EXTRACT_RANGE, OnRange)
	ON_MESSAGE(WM_EXTRACT_ERROR, OnError)
	ON_MESSAGE(WM_EXTRACT_STEPIT, OnStepIt)
	ON_MESSAGE(WM_EXTRACT_CURFILE, OnCurFile)
	ON_MESSAGE(WM_EXTRACT_CURZIPFILE, OnCurZIPFile)
	ON_MESSAGE(WM_EXTRACT_SETSOURCE, OnSetSource)
	ON_MESSAGE(WM_EXTRACT_SETTARGET, OnSetTarget)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnzipDlg message handlers

BOOL CUnzipDlg::Pump()
{
	MSG msg;

	// Retrieve and dispatch any waiting messages.

	while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp ()->PumpMessage ()) {
			::PostQuitMessage (0);
			return FALSE;
		}
	}

	return TRUE;

}


void CUnzipDlg::UpdateDlg(LPCTSTR sFileName)
{
	CString sText;
	CString sName(sFileName);
	// The DLL returns Unix-like file-separators. Replace them
	sName.Replace('/', '\\');
	int nFirst = sName.ReverseFind('\\') + 1;
	sName = sName.Mid(nFirst);
	m_pPseudoThis->AddExtractedFile(sName);
}

void CUnzipDlg::SetRange(UINT nRange)
{
	m_pPseudoThis->ShowWindow(SW_SHOW);

	// Set the range of the Progress bar
	m_pPseudoThis->m_Progress.SetRange32(0, nRange);
	m_pPseudoThis->m_noOfFiles = nRange;
	// Set the step width to 1
	m_pPseudoThis->m_Progress.SetStep(1);
	m_pPseudoThis->m_Progress.SetPos(0);
	m_pPseudoThis->m_nErrors = 0;
	m_pPseudoThis->m_nFileNo = 1;

	//m_pPseudoThis->ShowDetails(FALSE);
}

void CUnzipDlg::StepIt()
{
	m_pPseudoThis->m_Progress.StepIt();
}

void CUnzipDlg::ShowCurZIPFile(LPCTSTR sFileName)
{
	// Show information with the new ZIP file.
	CString sFiles;
	sFiles.Format("Extracting file %d from %d", m_pPseudoThis->m_nFileNo++, m_pPseudoThis->m_noOfFiles);
	m_pPseudoThis->SetDlgItemText(IDC_FILEOF, sFiles);
	m_pPseudoThis->SetDlgItemText(IDC_STATICFILENAME, sFileName);
	m_pPseudoThis->AddCurFile(sFileName);
}


BOOL CUnzipDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Load the indizes of these two file types so that
	// they don't have to be determined every time.
	CString sExt = "C:\\*.zip";
	m_nZIPIconIndex = GetIconIndex(sExt);
	sExt = "C:\\*.txt";
	m_nTXTIconIndex = GetIconIndex(sExt);

	HIMAGELIST hSmallSystemImageList;
	SHFILEINFO shinfo;

	// Get the handle of the (normal) system imagelist.
	hSmallSystemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_ImageList.Attach(hSmallSystemImageList);
	::SendMessage(m_FileTree.m_hWnd, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hSmallSystemImageList);
	m_ImageList.Detach();
	m_nErrors = 0;
	m_nFileNo = 1;

	m_bInfoShown = TRUE;

	ShowDetails(FALSE);

	m_Animation.Open(IDR_EXTRACT);
	SetDlgItemText(IDC_SOURCE, m_sSourceDir);
	SetDlgItemText(IDC_TARGET, m_sTargetDir);

	m_noOfFiles = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUnzipDlg::OnClose() 
{
	if (!m_bCanClose) {
		SetEvent(hStop);
		return;
	}
	
	UpdateData();

	CDialog::OnClose();
}

void CUnzipDlg::EndStatus()
{
	m_Animation.Stop();
	m_Animation.Seek(0);
	CString sMsg;
	if (m_nErrors > 0) {
		sMsg.Format("All files processed. %d Errors!", m_nErrors);
	}
	else {
		sMsg = "All files extracted";
	}
	SetDlgItemText(IDC_FILEOF, sMsg);
	m_Progress.SetPos(0);
	m_FileTree.Invalidate();
	SetWindowText("Finished");
	GetDlgItem(IDC_CANCEL)->ModifyStyle(WS_VISIBLE, 0);
	RECT updateRect;
	GetDlgItem(IDC_CANCEL)->GetWindowRect(&updateRect);
	ScreenToClient(&updateRect);
	InvalidateRect(&updateRect, TRUE);
	UpdateWindow();
	MessageBeep(MB_OK);
}

void CUnzipDlg::CancelStatus()
{
	m_Animation.Stop();
	m_Animation.Seek(0);
	SetDlgItemText(IDC_FILEOF, "Extraction cancelled by user!");
	SetWindowText("CANCELLED");
	MessageBeep(MB_ICONEXCLAMATION);
}


void CUnzipDlg::AddCurFile(LPCTSTR sName)
{
	m_htiCurFile = m_FileTree.InsertItem(sName, m_nZIPIconIndex, m_nZIPIconIndex);
	m_FileTree.EnsureVisible(m_htiCurFile);
}

void CUnzipDlg::AddExtractedFile(LPCTSTR sName)
{
	if (m_htiCurFile) {
		CString sExt = sName;
		sExt = "C:\\*" + sExt.Mid(sExt.ReverseFind('.'));
		int nImage = GetIconIndex(sExt);
		HTREEITEM hInserted = m_FileTree.InsertItem(sName, nImage, nImage, m_htiCurFile);
		HTREEITEM hParent = NULL;
		if (hParent = m_FileTree.GetParentItem(hInserted)) {
			if (m_FileTree.GetItemState(hParent, TVIF_STATE) & TVIS_EXPANDED) {
				m_FileTree.EnsureVisible(hInserted);
			}
		}
	}
}

int CUnzipDlg::GetIconIndex(const CString &sPath)
{
	SHFILEINFO shinfo;

	// We get the index of the file from the ImageList.

	// WARNING: These calls are hideously expensive in terms of time!!!
	SHGetFileInfo(sPath, FILE_ATTRIBUTE_NORMAL, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
	return shinfo.iIcon;
}

void CUnzipDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	//m_ImageList.Detach();
}

void CUnzipDlg::ErrorInCurFile(errorCode error, LPCTSTR sFile /* = NULL */)
{
	CString sErrorMsg;
	CString sFileName;
	if (sFile) {
		sFileName = sFile;
	}
	switch(error) {
		case EX_OK:
			return;
		case EX_ARCHIVE:
			sErrorMsg = "Error in Archive file " + sFileName;
			break;
		case EX_MEM:
			sErrorMsg = "Not enough free memory available";
			break;
		case EX_NO_ARCHIVES:
			sErrorMsg = "No archives found";
			break;
		case EX_NO_FILES:
			sErrorMsg = "No files in archive found " + sFileName;
			break;
		case EX_DISK:
			sErrorMsg = "The disk is probably full";
			break;
		case EX_EOF:
			sErrorMsg = "Unexpected end of file found. Archive is probably corrupt";
			break;
		case EX_UNSUP:
			sErrorMsg = "Unsupported archive type";
			break;
		case EX_BADPWD:
			sErrorMsg = "Bad password for file " + sFileName;
			break;
		case EX_CRC:
			sErrorMsg = "CRC error in " + sFileName + ". Data is probably corrupt";
			break;
		case EX_OPEN:
			sErrorMsg = sFileName + " could not be opened";
			break;
		case EX_CLOSE:
			sErrorMsg = sFileName + " could not be closed";
			break;
		case EX_READ:
			sErrorMsg = "No data could be read from " + sFileName;
			break;
		case EX_WRITE:
			sErrorMsg = "No data could be written to " + sFileName;
			break;
		case EX_CREATE:
			sErrorMsg = sFileName + " could not be created";
			break;
		default:
			sErrorMsg = "Unknown Error while processing files";
	}
	m_pPseudoThis->m_nErrors++;
	// Bold this entry
	m_pPseudoThis->m_FileTree.SetItemState(m_pPseudoThis->m_htiCurFile, TVIS_BOLD, TVIS_BOLD);
	// Delete the added files from this item
	m_pPseudoThis->DeleteChildren(m_pPseudoThis->m_htiCurFile);
	// Change the item image to the txt image
	m_pPseudoThis->m_FileTree.SetItemImage(m_pPseudoThis->m_htiCurFile, m_pPseudoThis->m_nTXTIconIndex, m_pPseudoThis->m_nTXTIconIndex);
	// Insert the error message
	m_pPseudoThis->m_FileTree.InsertItem(sErrorMsg, m_pPseudoThis->m_nTXTIconIndex, m_pPseudoThis->m_nTXTIconIndex, m_pPseudoThis->m_htiCurFile);
	m_pPseudoThis->m_FileTree.Expand(m_pPseudoThis->m_htiCurFile, TVE_EXPAND);
}

void CUnzipDlg::DeleteChildren(HTREEITEM hParent)
{
	if (!hParent) {
		return;
	}

	HTREEITEM hChild = m_FileTree.GetChildItem(hParent), hTemp;
	while (hChild != NULL) {
		hTemp = m_FileTree.GetNextSiblingItem(hChild);
		m_FileTree.DeleteItem(hChild);
		hChild = hTemp;
	}
}



void CUnzipDlg::OnShowDetails() 
{
	ShowDetails(!m_bInfoShown);
}

void CUnzipDlg::ShowDetails(BOOL bShow)
{
	if (m_bInfoShown && bShow) {
		return;
	}
	if (!m_bInfoShown && !bShow) {
		return;
	}



	RECT dlgrect, inforect;
	GetWindowRect(&dlgrect);
	m_DetailGroup.GetWindowRect(&inforect);


	if (bShow) {
		// Expand the dialog
		dlgrect.bottom += (inforect.bottom - inforect.top);
		SetDlgItemText(IDC_SHOWDETAILS, "<<< Details");
		m_bInfoShown = TRUE;
	}
	else {
		// Make the dialog smaller.
		dlgrect.bottom -= (inforect.bottom - inforect.top);
		SetDlgItemText(IDC_SHOWDETAILS, "Details >>>");
		m_bInfoShown = FALSE;
	}

	MoveWindow(&dlgrect);

	// Force the window to be redrawn
	InvalidateRect(NULL);
	UpdateWindow();
}

BOOL CUnzipDlg::CloseIfDone()
{
	UpdateData();
	if (m_bCanClose && m_nErrors == 0 && *m_pbCloseWhenDone) {
		EndDialog(0);
		return TRUE;
	}

	return FALSE;
}

void CUnzipDlg::OnRange(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);

	lock.Lock();
	SetRange(lParam);
	lock.Unlock();
}

void CUnzipDlg::OnCurFile(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);
	lock.Lock();
	UpdateDlg(g_sListCurfiles.RemoveHead());
	lock.Unlock();
}

void CUnzipDlg::OnCurZIPFile(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);
	lock.Lock();
	ShowCurZIPFile(g_sListCurZIPfiles.RemoveHead());
	lock.Unlock();
}

void CUnzipDlg::OnStepIt(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);
	StepIt();
	lock.Unlock();
}	

void CUnzipDlg::OnError(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);
	lock.Lock();
	ErrorInCurFile((errorCode)lParam, (LPTSTR)pParam);
	lock.Unlock();
}


void CUnzipDlg::OnSetSource(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);
	lock.Lock();
	GetDlgItem(IDC_SOURCE)->SetWindowText((LPTSTR)lParam);
	lock.Unlock();
}

void CUnzipDlg::OnSetTarget(UINT pParam, long lParam)
{
	CSingleLock lock(&m_Mutex);
	lock.Lock();
	GetDlgItem(IDC_TARGET)->SetWindowText((LPTSTR)lParam);
	lock.Unlock();
}


void CUnzipDlg::Kill()
{
	CSingleLock lock(&m_Mutex);
	lock.Lock();
	UpdateData();
	EndDialog(0);
	DestroyWindow();
	lock.Unlock();
}
