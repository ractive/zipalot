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


// FileCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZipALot.h"
#include "FileCheckDlg.h"
#include "Unzip.h"
#include "WhatIfDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileCheckDlg dialog

CListCtrl CFileCheckDlg::m_ZIPDetail;
int CFileCheckDlg::m_nFiles = 0;
DWORD CFileCheckDlg::m_dwPackedSize = 0;
DWORD CFileCheckDlg::m_dwUnpackedSize = 0;
int CFileCheckDlg::m_nEncrypted = 0;

CFileCheckDlg::CFileCheckDlg(tagUnzipInfo * pInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CFileCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileCheckDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pUnzipInfo = pInfo;
	m_bInfoShown = TRUE;
	m_bSortAscending = TRUE;
	m_bSorting = FALSE;
	m_dwPackedSize = 0;
	m_dwUnpackedSize = 0;
	m_nEncrypted = 0;
	m_nFiles = 0;
	m_nSortedCol = 0;
	nLastProcessed = -2;
}


void CFileCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileCheckDlg)
	DDX_Control(pDX, IDC_DETAILGROUP, m_DetailGroup);
	DDX_Control(pDX, IDC_ZIPDETAIL, m_ZIPDetail);
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileCheckDlg, CDialog)
	//{{AFX_MSG_MAP(CFileCheckDlg)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHeaderClick)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILELIST, OnItemChangedFileList)
	ON_BN_CLICKED(IDC_FULLLIST, OnFullList)
	ON_BN_CLICKED(IDC_HIDEINFO, HideZIPDetails)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileCheckDlg message handlers

BOOL CFileCheckDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_pUnzipInfo->sFilter == NULL) {
		m_sFilter = "*.zip";
	}
	else {
		m_sFilter = m_pUnzipInfo->sFilter;
	}

	SetWindowText("Unzip File List [" + m_sFilter + "]");

	AddFileListHeaders();
	AddZIPFileHeaders();

	AttachSystemImageList();

	DWORD dwTotalSize = 0;
	int nFiles = FillFileList(&dwTotalSize);

	if (nFiles == 0) {
		MessageBox("No files found that match " + m_sFilter, "Info", MB_ICONINFORMATION);
		EndDialog(1);
	}

	CString sTotal;
	sTotal.Format("Total: %d files, %s Bytes.    Applied filter: ", nFiles, FormatNumber(dwTotalSize));
	sTotal += m_sFilter;
	SetDlgItemText(IDC_TOTAL, sTotal);

	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_HIDEINFO), IMAGE_ICON, 16, 8, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_HIDEINFO))->SetIcon(hIcon);
	}
	
	HideZIPDetails();

	nLastProcessed = -2;
	m_bSorting = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileCheckDlg::OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here

	m_nSortedCol = phdn->iItem;
	m_bSortAscending = !m_bSortAscending;

	// Avoid that OnItemChangedFileList is called during the sort
	m_bSorting = TRUE;
	SortItems(m_nSortedCol, m_bSortAscending, 0, -1);
	m_bSorting = FALSE;
	
	*pResult = 0;
}



BOOL CFileCheckDlg::SortItems(int nCol, BOOL bAscending, int low, int high)
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

int CFileCheckDlg::CmpItems(CString cstL, CString cstR, int nCol)
{
	if (nCol == 0) {
		return cstL.CompareNoCase(cstR);
	}

	else if (nCol == 1) {
		if (cstR == cstL)
			return 0;
		else if (atol((LPCTSTR)cstL) < atol((LPCTSTR)cstR))
			return -1;
		else
			return 1;
	}
	else if (nCol == 2) {
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

void CFileCheckDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	//m_ImageList.Detach();
}

CString CFileCheckDlg::FormatNumber(DWORD dwNumber)
{
	CString sNumber;
	sNumber.Format("%ld", dwNumber);
	int nLen = sNumber.GetLength() - 3;

	for (int i = nLen; i > 0; i -= 3) {
		sNumber.Insert(i, '\'');
	}

	return sNumber;
}

void CFileCheckDlg::InsertFile(CString sName, DWORD dwSize, DWORD dwPackedSize, int nRatio, char cEncrypted, CString sDateTime)
{
	int nItem = 0;
	int nSubItem = 0;
	LV_ITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE;

	item.iItem = nItem++;
	item.iSubItem = nSubItem++;
	CString sExt = "C:\\*";
	sExt += sName.Mid(sName.ReverseFind('.'));
	item.iImage = GetIconIndex(sExt);

	if (cEncrypted == 'E') {
		sName += "+";
		m_nEncrypted++;
	}
	item.pszText = (LPTSTR)(LPCTSTR)sName;

	int iItem = m_ZIPDetail.InsertItem(&item);

	item.mask = LVIF_TEXT;

	// Add size
	CString sEntryText;
	sEntryText.Format("%ld", dwSize);
	item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
	item.iItem = iItem;
	item.iSubItem = 1;
	m_ZIPDetail.SetItem(&item);

	// Add Unpacked Size
	sEntryText.Format("%ld", dwPackedSize);
	item.iSubItem = 2;
	item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
	m_ZIPDetail.SetItem(&item);

	// Add Ratio
	sEntryText.Format("%d%%", nRatio);
	item.iSubItem = 3;
	item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
	m_ZIPDetail.SetItem(&item);

	// Add Date
	item.iSubItem = 4;
	item.pszText = (LPTSTR)(LPCTSTR)sDateTime;
	m_ZIPDetail.SetItem(&item);

	m_nFiles++;
	m_dwPackedSize += dwPackedSize;
	m_dwUnpackedSize += dwSize;
}

void CFileCheckDlg::AddZIPFileHeaders()
{
	int nItem = 0;
	
	int nSubItem = 0;

	// Add the list control headers
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 140;
	lvc.pszText = _T("Name");
	m_ZIPDetail.InsertColumn(0, &lvc);
	
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 70;
	lvc.pszText = _T("Unpacked");
	m_ZIPDetail.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 70;
	lvc.pszText = _T("Packed");
	m_ZIPDetail.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 40;
	lvc.pszText = _T("Ratio");
	m_ZIPDetail.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Date/Time");
	m_ZIPDetail.InsertColumn(4, &lvc);


}

void CFileCheckDlg::AddFileListHeaders()
{
	int nItem = 0;
	
	int nSubItem = 0;

	// Add the list control headers
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 140;
	lvc.pszText = _T("Name");
	m_FileList.InsertColumn(0, &lvc);
	
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Size");
	m_FileList.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Date/Time");
	m_FileList.InsertColumn(2, &lvc);

}

int CFileCheckDlg::FillFileList(DWORD * pdwTotalSize)
{
	// Add all files that match the filter

	CFileFind finder;
	CString sFind = m_pUnzipInfo->sSource;

	if (sFind.Right(1) != "\\") {
		sFind += "\\";
	}

	*pdwTotalSize = 0;
	int nFiles = 0;

	// Process all the filters
	m_sFilter.Replace("/", " / ");
	
	int nCounter = 0;
	BOOL bNoArchives = TRUE;

	BOOL bFoundAny = FALSE;

	char seps[] = "/";
	char * token = strtok((LPTSTR)(LPCTSTR)m_sFilter, seps);
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

		BOOL bFound = finder.FindFile(sFind + token);

		int nFiles = 0;
		int nItem = 0;

		LV_ITEM item;
		while (bFound) {
			bFoundAny = TRUE;
			item.mask = LVIF_TEXT | LVIF_IMAGE;
			bFound = finder.FindNextFile();
			if (finder.IsDots() || finder.IsDirectory()) {
				// Ignore the directories and the dots in the file list
				continue;
			}
			nFiles++;
			CString sEntryText = finder.GetFileName();
			item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
			item.iItem = nItem++;
			item.iSubItem = 0;

			CString sFilePath = finder.GetFilePath();
			
			item.iImage = GetIconIndex(sFilePath);
			
			int iItem = m_FileList.InsertItem(&item);

			item.mask = LVIF_TEXT;

			// Add size
			DWORD dwSize = finder.GetLength();
			*pdwTotalSize += dwSize;
			sEntryText.Format("%ld", dwSize);
			item.pszText = (LPTSTR)(LPCTSTR)sEntryText;
			item.iItem = iItem;
			item.iSubItem = 1;
			m_FileList.SetItem(&item);

			// Add Date
			item.iSubItem = 2;
			
			FILETIME ftutc, ft;
			SYSTEMTIME st;

			finder.GetLastWriteTime(&ftutc);

			FileTimeToLocalFileTime(&ftutc, &ft);
			FileTimeToSystemTime(&ft, &st);
			

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

			item.pszText = sDateTime;
			m_FileList.SetItem(&item);
		}
		// Get next token:
		token = strtok( NULL, seps );
	}

	return bFoundAny;

}

void CFileCheckDlg::HideZIPDetails()
{
	if (m_bInfoShown) {
		// Hide the two controls

		RECT dlgrect, inforect;
		GetWindowRect(&dlgrect);
		m_DetailGroup.GetWindowRect(&inforect);

		// Make the dialog smaller.
		dlgrect.bottom -= (inforect.bottom - inforect.top);
		MoveWindow(&dlgrect);

		m_bInfoShown = FALSE;

		// Force the window to be redrawn
		InvalidateRect(NULL);
		UpdateWindow();
	}

}

void CFileCheckDlg::ShowZIPDetails()
{
	if (!m_bInfoShown) {
		// Hide the two controls
		//m_InfoCtrl.ModifyStyle(WS_VISIBLE, 0);
		//m_DetailGroup.InvalidateRect(NULL);
		RECT dlgrect, inforect;
		GetWindowRect(&dlgrect);
		m_DetailGroup.GetWindowRect(&inforect);

		// Make the dialog smaller.
		dlgrect.bottom += (inforect.bottom - inforect.top);
		MoveWindow(&dlgrect);

		m_bInfoShown = TRUE;

		// Force the window to be redrawn
		InvalidateRect(NULL);
		UpdateWindow();
	}

}


int CFileCheckDlg::GetIconIndex(const CString &sPath)
{
		SHFILEINFO shinfo;

		// We get the index of the file from the ImageList.

		// WARNING: These calls are hideously expensive in terms of time!!!
		SHGetFileInfo(sPath, FILE_ATTRIBUTE_NORMAL, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
		return shinfo.iIcon;
}

void CFileCheckDlg::OnItemChangedFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem == -1 || nLastProcessed == pNMListView->iItem || m_bSorting) {
		return;
	}

	nLastProcessed = pNMListView->iItem;

	m_nFiles = 0;
	m_dwPackedSize = 0;
	m_dwUnpackedSize = 0;
	m_nEncrypted = 0;

	CUnzip unzipper(NULL);
	unzipper.Init();
	CString sZIPFile = m_pUnzipInfo->sSource;
	if (sZIPFile.Right(1) != "\\") {
		sZIPFile += "\\";
	}
	unzipper.InsertFile = &InsertFile;
	sZIPFile += m_FileList.GetItemText(pNMListView->iItem, 0);
	m_ZIPDetail.DeleteAllItems();
	
	// Only show the info
	m_pUnzipInfo->bExtract = FALSE;
	BOOL bTemp = m_pUnzipInfo->bOwnDir;
	m_pUnzipInfo->bOwnDir = FALSE;
	unzipper.unzipInfo = *m_pUnzipInfo;

	int nRet = unzipper.Extract(sZIPFile, NULL);
	m_pUnzipInfo->bOwnDir = bTemp;

	CString sMsg;
	sMsg.Format("%d files - unpacked: %s - packed: %s Bytes", m_nFiles, FormatNumber(m_dwUnpackedSize), FormatNumber(m_dwPackedSize));
	if (m_nEncrypted > 0) {
		CString sEnc;
		sEnc.Format(" - %d encrypted [+]", m_nEncrypted);
		sMsg += sEnc;
	}
	SetDlgItemText(IDC_ZIPTOTAL, sMsg);

	ShowZIPDetails();
	
	*pResult = 0;
}

void CFileCheckDlg::OnFullList() 
{
	//m_ImageList.Detach();
	CWhatIfDlg dlg(m_pUnzipInfo);
	dlg.DoModal();
	AttachSystemImageList();
}

void CFileCheckDlg::AttachSystemImageList()
{
	HIMAGELIST hSmallSystemImageList;
	SHFILEINFO shinfo;

	// Get the handle of the (normal) system imagelist.
	hSmallSystemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_ImageList.Attach(hSmallSystemImageList);
	::SendMessage(m_FileList.m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)hSmallSystemImageList);
	::SendMessage(m_ZIPDetail.m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)hSmallSystemImageList);
	m_ImageList.Detach();
}
