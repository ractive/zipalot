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


// WhatIfDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZipALot.h"
#include "WhatIfDlg.h"
#include "Unzip.h"
#include "Unace.h"
#include "Unrar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_READ (WM_USER + 23)

extern HANDLE hStop;

CWhatIfDlg * CWhatIfDlg::m_pPseudoThis = NULL;

/////////////////////////////////////////////////////////////////////////////
// CWhatIfDlg dialog


CWhatIfDlg::CWhatIfDlg(tagUnzipInfo * pInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CWhatIfDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWhatIfDlg)
	m_sInfo = _T("");
	m_sStatus = _T("");
	//}}AFX_DATA_INIT

	m_pUnzipInfo = pInfo;

	m_pPseudoThis = this;
	m_bCanClose = TRUE;
	m_bSortAscending = TRUE;
	m_dwPackedSize = 0;
	m_dwUnpackedSize = 0;
	m_nEncrypted = 0;
	m_nFiles = 0;
	m_nSortedCol = 0;
}


void CWhatIfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhatIfDlg)
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
	DDX_Text(pDX, IDC_INFO, m_sInfo);
	DDX_Text(pDX, IDC_STATUS, m_sStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWhatIfDlg, CDialog)
	//{{AFX_MSG_MAP(CWhatIfDlg)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHeaderClick)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_READ, OnRead)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhatIfDlg message handlers

BOOL CWhatIfDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int nItem = 0;
	
	int nSubItem = 0;

	// Add the list control headers
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 200;
	lvc.pszText = _T("Name");
	m_FileList.InsertColumn(0, &lvc);
	
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 70;
	lvc.pszText = _T("Unpacked");
	m_FileList.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 70;
	lvc.pszText = _T("Packed");
	m_FileList.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 40;
	lvc.pszText = _T("Ratio");
	m_FileList.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Date/Time");
	m_FileList.InsertColumn(4, &lvc);

	HIMAGELIST hSmallSystemImageList;
	SHFILEINFO shinfo;

	// Get the handle of the (normal) system imagelist.
	hSmallSystemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_ImageList.Attach(hSmallSystemImageList);
	::SendMessage(m_FileList.m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)hSmallSystemImageList);
	m_ImageList.Detach();

	m_sInfo = "Output of unzip operation to ";
	m_sInfo += m_pUnzipInfo->sTarget;
	UpdateData(FALSE);

	AfxBeginThread(FillFileList, NULL);

	m_nFiles = 0;
	m_dwPackedSize = 0;
	m_dwUnpackedSize = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWhatIfDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	//m_ImageList.Detach();	
}

void CWhatIfDlg::OnClose() 
{
	if (!m_bCanClose) {
		return;
	}
	//m_ImageList.Detach();
	
	CDialog::OnClose();
}

void CWhatIfDlg::InsertFile(CString sName, DWORD dwSize, DWORD dwPackedSize, int nRatio, char cEncrypted, CString sDateTime)
{
	// Check for directories in the filename
	// Remove them if we dont have bRecreateDir
	int nFind = -1;
	if ((nFind = sName.ReverseFind('\\')) != -1 && !m_pPseudoThis->m_pUnzipInfo->bRecreateDir) {
		// Extract only the filename
		sName = sName.Mid(nFind + 1);
	}

	LVFINDINFO fi;
	fi.flags = LVFI_STRING;
	fi.psz = sName;
	int nReplace = FALSE;
	nReplace = m_pPseudoThis->m_FileList.FindItem(&fi);
	if (nReplace != -1 ) {
		if (!m_pPseudoThis->m_pUnzipInfo->bOverwrite) {
			// The file is already present and shall not be overwritten
			// We leave.
			return;
		}
	}

	int nItem = 0;
	int nSubItem = 0;
	LV_ITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE;

	item.iItem = nItem++;
	item.iSubItem = nSubItem++;
	CString sExt = "C:\\*";
	sExt += sName.Mid(sName.ReverseFind('.'));
	item.iImage = m_pPseudoThis->GetIconIndex(sExt);
	if (cEncrypted == 'E') {
		//sName += "+";
		m_pPseudoThis->m_nEncrypted++;
	}
	item.pszText = (LPTSTR)(LPCTSTR)sName;

	if (m_pPseudoThis->m_pUnzipInfo->bOverwrite) {
		// Replace the existing item
		item.iItem = nReplace;
		m_pPseudoThis->m_FileList.SetItem(&item);
	}
	else {
		item.iItem = m_pPseudoThis->m_FileList.InsertItem(&item);
	}

	item.mask = LVIF_TEXT;

	// Add size
	CString sEntryText;
	sEntryText.Format("%ld", dwSize);
	item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
	item.iSubItem = 1;
	m_pPseudoThis->m_FileList.SetItem(&item);

	// Add Unpacked Size
	sEntryText.Format("%ld", dwPackedSize);
	item.iSubItem = 2;
	item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
	m_pPseudoThis->m_FileList.SetItem(&item);

	// Add Ratio
	sEntryText.Format("%d%%", nRatio);
	item.iSubItem = 3;
	item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
	m_pPseudoThis->m_FileList.SetItem(&item);

	// Add Date
	item.iSubItem = 4;
	item.pszText = (LPTSTR)(LPCTSTR)sDateTime;
	m_pPseudoThis->m_FileList.SetItem(&item);

	m_pPseudoThis->m_nFiles++;
	m_pPseudoThis->m_dwPackedSize += dwPackedSize;
	m_pPseudoThis->m_dwUnpackedSize += dwSize;
}

int CWhatIfDlg::GetIconIndex(const CString &sPath)
{
		SHFILEINFO shinfo;

		// We get the index of the file from the ImageList.

		// WARNING: These calls are hideously expensive in terms of time!!!
		SHGetFileInfo(sPath, FILE_ATTRIBUTE_NORMAL, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
		return shinfo.iIcon;
}

UINT CWhatIfDlg::FillFileList(LPVOID lParam)
{
	CWaitCursor wait;

	CExtract * pExtractor = NULL;

	// Process all the filters
	CString sFilter = m_pPseudoThis->m_pUnzipInfo->sFilter;
	sFilter.Replace("/", " / ");
	
	int nCounter = 0;
	errorCode nRet;
	BOOL bNoArchives = TRUE;
	m_pPseudoThis->m_pUnzipInfo->bExtract = FALSE;

	char seps[] = "/";
	char * token = strtok((LPTSTR)(LPCTSTR)sFilter, seps);
	while( token != NULL )
	{
		// While there are tokens in "string"
		nCounter++;

		// Remove trailing spaces
		while (*token == ' ') {
			token++;
		}

		if (strlen(token) < 2) {
			token = strtok( NULL, seps );
			continue;
		}
		switch (nCounter) {
			case 1:
				pExtractor = new CUnzip(m_pPseudoThis);
				m_pPseudoThis->m_pUnzipInfo->type = ZIP;
				break;
			case 2:
				pExtractor = new CUnrar(m_pPseudoThis);
				m_pPseudoThis->m_pUnzipInfo->type = RAR;
				break;
			case 3:
				pExtractor = new CUnace(m_pPseudoThis);
				m_pPseudoThis->m_pUnzipInfo->type = ACE;
				break;
		}
		
		pExtractor->InsertFile = &InsertFile;

		if (pExtractor == NULL) {
			break;
		}

		sFilter = token;
		m_pPseudoThis->m_pUnzipInfo->sFilter = sFilter;

		if (!pExtractor->Init()) {
			return 1;
		}
		pExtractor->unzipInfo = *m_pPseudoThis->m_pUnzipInfo;
		
		m_pPseudoThis->m_bCanClose = FALSE;
		nRet = pExtractor->ExtractAll(m_pPseudoThis->m_pUnzipInfo->sSource, m_pPseudoThis->m_pUnzipInfo->sTarget);
		m_pPseudoThis->m_bCanClose = TRUE;
		//m_pPseudoThis->SendMessage(WM_READ);
		m_pPseudoThis->OnRead();

		bNoArchives &= nRet == EX_NO_ARCHIVES;
		TRACE("Return value thread %d\r\n", nRet);
		delete pExtractor;
		pExtractor = NULL;

		if (nRet == EX_CANCELLED) {
			//m_pPseudoThis->CancelStatus();
			break;
		}
		if (nRet == EX_OK) {
			
		}
		// Get next token:
		token = strtok( NULL, seps );
	}

	return nRet;
}

BOOL CWhatIfDlg::SortItems(int nCol, BOOL bAscending, int low, int high)
{
	if (nCol >= m_FileList.GetHeaderCtrl()->GetItemCount()) {
		return FALSE;
	}

	if( high == -1 )
		high = m_FileList.GetItemCount() - 1;
	int lo = low;
	int hi = high;
	CString midItem;
	if( hi <= lo )
		return FALSE;
	midItem = m_FileList.GetItemText( (lo+hi)/2, nCol );
	// loop through the list until indices cross
	while( lo <= hi )	{
 		// rowText will hold all column text for one row 
		CStringArray rowText;
 		// find the first element that is greater than or equal to
 		// the partition element starting from the left Index.
		if( bAscending ) {
  			//while( ( lo < high ) && (atoi(GetItemText(lo, nCol)) < midItem ) )
			while( ( lo < high ) && ( CmpItems( m_FileList.GetItemText(lo, nCol) , midItem, nCol) < 0 ) )
   				++lo;
		}
		else {
  			//while( ( lo < high ) && (atoi(GetItemText(lo, nCol)) > midItem ) )
			while( ( lo < high ) && ( CmpItems( m_FileList.GetItemText(lo, nCol) , midItem, nCol) > 0 ) )
   				++lo;
		}
		// find an element that is smaller than or equal to
 		// the partition element starting from the right Index.
		if( bAscending ) {
  			//while( ( hi > low ) && (atoi(GetItemText(hi, nCol)) > midItem ) )
			while( ( hi > low ) && ( CmpItems(m_FileList.GetItemText(hi, nCol) , midItem, nCol ) > 0 ) )
   				--hi;
		}
		else {
  			//while( ( hi > low ) && (atoi(GetItemText(hi, nCol)) < midItem ) )
			while( ( hi > low ) && ( CmpItems(m_FileList.GetItemText(hi, nCol) , midItem, nCol ) < 0 ) )
   				--hi;
		}
		// if the indexes have not crossed, swap
 		// and if the items are not equal
		if( lo <= hi ) {
			// swap only if the items are not equal
			//if(atoi(GetItemText(lo, nCol)) != atoi(GetItemText(hi, nCol)) )			/*!!!*
			if( CmpItems (m_FileList.GetItemText(lo, nCol) , m_FileList.GetItemText(hi, nCol), nCol  ) != 0) {
	   			// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
	   			int nColCount = m_FileList.GetHeaderCtrl()->GetItemCount();
	   			rowText.SetSize( nColCount );
				int i;
	   			for( i=0; i < nColCount; i++)
					rowText[i] = m_FileList.GetItemText(lo, i);
	   			lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	   			lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
	   			lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
	      					LVIS_FOCUSED |  LVIS_SELECTED |
	      					LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
	   			lvitemhi.iItem = hi;
				m_FileList.GetItem( &lvitemlo );
	   			m_FileList.GetItem( &lvitemhi );
				for( i=0; i< nColCount; i++)
	      			m_FileList.SetItemText(lo, i, m_FileList.GetItemText(hi, i) );
				lvitemhi.iItem = lo;
	   			m_FileList.SetItem( &lvitemhi );
				for( i=0; i< nColCount; i++)
	      			m_FileList.SetItemText(hi, i, rowText[i]);
				lvitemlo.iItem = hi;
	   			m_FileList.SetItem( &lvitemlo );
			}	  		
			
			++lo;
			--hi;
		}
	}
	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
 		SortItems( nCol, bAscending , low, hi);
	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
 		SortItems( nCol, bAscending , lo, high );
	return TRUE;

}

int CWhatIfDlg::CmpItems(CString cstL, CString cstR, int nCol)
{
	if (nCol == 0 || nCol == 3) {
		return cstL.CompareNoCase(cstR);
	}

	else if (nCol == 1 || nCol == 2) {
		if (cstR == cstL)
			return 0;
		else if (atol((LPCTSTR)cstL) < atol((LPCTSTR)cstR))
			return -1;
		else
			return 1;
	}
	else if (nCol == 4) {
		COleDateTime odtL;
		COleDateTime odtR;
		odtL.ParseDateTime(cstL);
		odtR.ParseDateTime(cstR);
		COleDateTimeSpan spanElapsed = odtL - odtR;
		if (spanElapsed.GetStatus() == COleDateTimeSpan::invalid) {
			return 0;
		}
		return (int)spanElapsed.GetTotalSeconds();

	}
	return 0;

}

void CWhatIfDlg::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here

	m_nSortedCol = phdn->iItem;
	m_bSortAscending = !m_bSortAscending;
	SortItems(m_nSortedCol, m_bSortAscending, 0, -1);
	
	*pResult = 0;
}

void CWhatIfDlg::OnRead()
{
	m_sStatus.Format("%d files - unpacked: %s - packed: %s Bytes", m_nFiles, FormatNumber(m_dwUnpackedSize), FormatNumber(m_dwPackedSize));
	CString sEnc;
	if (m_nEncrypted > 0) {
		sEnc.Format(" - %d encrypted [+]", m_nEncrypted);
		m_sStatus += sEnc;
	}
	
	ULARGE_INTEGER ulFreeToCaller;
	ULARGE_INTEGER ulFreeDisk;
	ULARGE_INTEGER ulFreeTotal;
	BOOL bRet = GetDiskFreeSpaceEx(m_pUnzipInfo->sTarget, &ulFreeToCaller, &ulFreeDisk, &ulFreeTotal);
	// bRet may be FALSE, if the target directory does not exist yet.
	if (bRet && ulFreeToCaller.LowPart < m_dwUnpackedSize) {
		MessageBox("ATTENTION! This operation would exceed your free disk space!", "DISK FULL", MB_ICONEXCLAMATION);
	}
	
	if (bRet) {
		sEnc.Format(" - Free disk space: %s", FormatNumber(ulFreeToCaller.LowPart));
		m_sStatus += sEnc;
	}

	UpdateData(FALSE);

}

CString CWhatIfDlg::FormatNumber(DWORD dwNumber)
{
	CString sNumber;
	sNumber.Format("%ld", dwNumber);
	int nLen = sNumber.GetLength() - 3;

	for (int i = nLen; i > 0; i -= 3) {
		sNumber.Insert(i, '\'');
	}

	return sNumber;
}

