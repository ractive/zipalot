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

// PostProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "PostProcessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostProcessDlg dialog


CPostProcessDlg::CPostProcessDlg(CString sTarget, CWnd* pParent /*=NULL*/)
	: CDialog(CPostProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPostProcessDlg)
	//}}AFX_DATA_INIT
	m_sTargetDir = sTarget;
}


void CPostProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostProcessDlg)
	DDX_Control(pDX, IDC_ACTIONS, m_ActionCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPostProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CPostProcessDlg)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_UNSELECTALL, OnUnselectall)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostProcessDlg message handlers

BOOL CPostProcessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_UP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_UP))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_DOWN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_DOWN))->SetIcon(hIcon);
	}

	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDC_UP), IDC_UP);
	m_ToolTip.AddTool(GetDlgItem(IDC_DOWN), IDC_DOWN);
	
	DWORD dw_ExStyle = m_ActionCtrl.GetExtendedStyle();
	m_ActionCtrl.SetExtendedStyle(dw_ExStyle | LVS_EX_CHECKBOXES);

	AddListHeaders();

	int i = 0;

	CFileFind finder;
	if (finder.FindFile(m_sTargetDir + "\\*.rar")) {
		finder.FindNextFile();
		AddAction(finder.GetFileName(), "unrar");
	}
	finder.Close();
	
	if (finder.FindFile(m_sTargetDir + "\\*.ace")) {
		finder.FindNextFile();
		AddAction(finder.GetFileName(), "unace");
	}
	finder.Close();

	if (finder.FindFile(m_sTargetDir + "\\*.exe")) {
		while(finder.FindNextFile()) {
			AddAction(finder.GetFileName(), "execute");
		}
	}
	finder.Close();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPostProcessDlg::AddListHeaders()
{
	int nItem = 0;
	
	int nSubItem = 0;

	// Add the list control headers
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 140;
	lvc.pszText = _T("Filename");
	m_ActionCtrl.InsertColumn(0, &lvc);
	
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Action");
	m_ActionCtrl.InsertColumn(1, &lvc);
}

BOOL CPostProcessDlg::AddAction(LPCTSTR lpszFileName, LPCTSTR lpszAction)
{
	int nSubItem = 0;
	LVITEM item;
	item.mask = LVIF_TEXT;
	item.iItem = 0;
	item.iSubItem = nSubItem++;
	item.pszText = (LPTSTR)lpszFileName;
	int nRet = m_ActionCtrl.InsertItem(&item);
	if (nRet == -1) {
		return FALSE;
	}

	item.iItem = nRet;
	item.iSubItem = nSubItem++;
	item.pszText = (LPTSTR)lpszAction;
	return m_ActionCtrl.SetItem(&item) != -1;

}

void CPostProcessDlg::OnUp() 
{
	POSITION pos = NULL;
	if(pos = m_ActionCtrl.GetFirstSelectedItemPosition()) {
		int nIndex = m_ActionCtrl.GetNextSelectedItem(pos);
		if (nIndex != 0) {
			SwapRows(nIndex - 1, nIndex);
		}
		m_ActionCtrl.SetFocus();
	}
	
}

void CPostProcessDlg::OnDown() 
{
	POSITION pos = NULL;
	if(pos = m_ActionCtrl.GetFirstSelectedItemPosition()) {
		int nIndex = m_ActionCtrl.GetNextSelectedItem(pos);
		if (nIndex + 1 < m_ActionCtrl.GetItemCount()) {
			SwapRows(nIndex, nIndex + 1);
		}

		m_ActionCtrl.SetFocus();
	}
}

void CPostProcessDlg::SwapRows(int lo, int hi)
{
	CStringArray rowText;

	LV_ITEM lvitemlo, lvitemhi;
	int nColCount = m_ActionCtrl.GetHeaderCtrl()->GetItemCount();
	rowText.SetSize( nColCount );
	int i;
	for( i=0; i < nColCount; i++)
		rowText[i] = m_ActionCtrl.GetItemText(lo, i);
	lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvitemlo.iItem = lo;
	lvitemlo.iSubItem = 0;
	lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
	      		LVIS_FOCUSED |  LVIS_SELECTED |
	      		LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
	lvitemhi = lvitemlo;
	lvitemhi.iItem = hi;
	m_ActionCtrl.GetItem( &lvitemlo );
	m_ActionCtrl.GetItem( &lvitemhi );
	for( i=0; i< nColCount; i++)
	    m_ActionCtrl.SetItemText(lo, i, m_ActionCtrl.GetItemText(hi, i) );
	lvitemhi.iItem = lo;
	m_ActionCtrl.SetItem( &lvitemhi );
	for( i=0; i< nColCount; i++)
	    m_ActionCtrl.SetItemText(hi, i, rowText[i]);
	lvitemlo.iItem = hi;
	m_ActionCtrl.SetItem( &lvitemlo );
}

BOOL CPostProcessDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message== WM_LBUTTONDOWN || pMsg->message== WM_LBUTTONUP || pMsg->message== WM_MOUSEMOVE) {
		m_ToolTip.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPostProcessDlg::OnSelectall() 
{
	int nCount = m_ActionCtrl.GetItemCount();
	for(int i = 0; i < nCount; i++) {
		m_ActionCtrl.SetCheck(i);
	}
}

void CPostProcessDlg::OnUnselectall() 
{
	int nCount = m_ActionCtrl.GetItemCount();
	for(int i = 0; i < nCount; i++) {
		m_ActionCtrl.SetCheck(i, FALSE);
	}
	
}

void CPostProcessDlg::OnInvert() 
{
	int nCount = m_ActionCtrl.GetItemCount();
	for(int i = 0; i < nCount; i++) {
		m_ActionCtrl.SetCheck(i, !m_ActionCtrl.GetCheck(i));
	}	
}

void CPostProcessDlg::OnOK() 
{

	
	CDialog::OnOK();
}
