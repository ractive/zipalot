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

// ZipALotDlg.cpp : implementation file
//

#include "stdafx.h"
#include <imagehlp.h>
#include "ZipALot.h"
#include "ZipALotDlg.h"
#include "DirectoryPicker.h"
#include "PasswordDlg.h"
#include "Unzip.h"
#include "Unace.h"
#include "Unrar.h"
#include "FileCheckDlg.h"
#include "HyperLink.h"
#include "PostProcessDlg.h"
#include "WizPageSource.h"
#include "WizPageTarget.h"
#include "WizUnzip.h"
#include "WizPageNFO.h"
#include "WizPageAction.h"
#include "WizError.h"
#include "WizEnd.h"
#include "ConfigDlg.h"
#include "WizSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* return codes of password fetches (negative = user abort; positive = error) */
#define IZ_PW_ENTERED      0   /* got some password string; use/try it */
#define IZ_PW_CANCEL      -1   /* no password available (for this entry) */
#define IZ_PW_CANCELALL   -2   /* no password, skip any further pwd. request */
#define IZ_PW_ERROR        5   /* = PK_MEM2 : failure (no mem, no tty, ...) */


// Global function to the a windows error message
// You won't win a design competition with a global funtion, but it's easy to use ;-)

HANDLE hStop;

BOOL Pump(HWND hWnd) {
	if (!::IsWindow(hWnd)) {
		return FALSE;
	}

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

CString GetSystemMessage(DWORD dwError)
{
	CString sError;
	// There's a windows error code.
	// Convert that to a string and show it, too.
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	sError = (LPCTSTR)lpMsgBuf;
	
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return sError;
}



////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_Email;
	CHyperLink	m_HomePage;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EMAIL, m_Email);
	DDX_Control(pDX, IDC_HOMEPAGE, m_HomePage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZipALotDlg dialog

CZipALotDlg::CZipALotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZipALotDlg::IDD, pParent), rb("Settings")
{
	//{{AFX_DATA_INIT(CZipALotDlg)
	m_sSourceDir = _T("");
	m_sTargetDir = _T("");
	m_bOverwrite = FALSE;
	m_bOwnDir = FALSE;
	m_bCaseInsensitive = FALSE;
	m_bOnlyNewer = FALSE;
	m_bRecreateDir = FALSE;
	m_bLockTarget = FALSE;
	//}}AFX_DATA_INIT
	// Set the standard values
	m_UnzipInfo.bCaseInsensitive = FALSE;
	m_UnzipInfo.bExtract = TRUE;
	m_UnzipInfo.bOnlyNewer = FALSE;
	m_UnzipInfo.bOverwrite = FALSE;
	m_UnzipInfo.bRecreateDir = TRUE;
	m_UnzipInfo.sDontExFilter = NULL;
	m_UnzipInfo.sExFilter = NULL;
	m_UnzipInfo.sFilter = NULL;
	m_UnzipInfo.sSource = NULL;
	m_UnzipInfo.sTarget = NULL;
	m_UnzipInfo.bOwnDir = FALSE;

	CString sProgDir;
	// Get the value from the Registry...
	sProgDir.Empty();
	unsigned long lLen = _MAX_PATH;
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ, &hKey);
	RegQueryValueEx(hKey, "ProgramFilesDir", NULL, NULL, (LPBYTE)sProgDir.GetBuffer(_MAX_PATH), &lLen);
	sProgDir.ReleaseBuffer();
	RegCloseKey(hKey);

	// And "save" them
	rb.Bind(m_bLockTarget, "LockTarget", FALSE);
	rb.Bind(m_sTargetDir, "TargetDir", "");
	rb.Bind(m_bCloseWhenDone, "CloseIfDone", FALSE);
	rb.Bind(m_sDefaultInstallDir, "Games dir", sProgDir);
	rb.Bind(m_sStartSource, "Start Source", "");
	rb.Bind(m_sStartTarget, "Start Target", "");
	rb.Bind(m_sUnzipped, "Unzipped", "unzipped");
	rb.Bind(m_bTargetLockedStart, "Target locked startup", FALSE);
	rb.Bind(m_bFilterSettings, "Show filter settings", FALSE);
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZipALotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZipALotDlg)
	DDX_Control(pDX, IDC_PROPSHEET, m_PropCtrl);
	DDX_Control(pDX, IDC_SOURCEDIR, m_SourceCombo);
	DDX_Control(pDX, IDC_TARGETPICKER, m_TargetPickerButton);
	DDX_Control(pDX, IDC_TARGETDIR, m_TargetBox);
	DDX_Text(pDX, IDC_SOURCEDIR, m_sSourceDir);
	DDX_Text(pDX, IDC_TARGETDIR, m_sTargetDir);
	DDX_Check(pDX, IDC_LOCKTARGET, m_bLockTarget);
	//}}AFX_DATA_MAP
	/*DDX_Check(pDX, IDC_OVERWRITE, m_UnzipInfo.bOverwrite);
	DDX_Check(pDX, IDC_OWNDIR, m_UnzipInfo.bOwnDir);
	DDX_Check(pDX, IDC_CASEINSENSITIV, m_UnzipInfo.bCaseInsensitive);
	DDX_Check(pDX, IDC_ONLYNEWER, m_UnzipInfo.bOnlyNewer);
	DDX_Check(pDX, IDC_RECREATEDIR, m_UnzipInfo.bRecreateDir);

	unzip_info.bExtract = TRUE;

	unzip_info.sDontExFilter = NULL;
	unzip_info.sExFilter = NULL;
	unzip_info.sFilter = NULL;*/

	if (::IsWindow(m_ExpertPage.m_hWnd)) {
		m_ExpertPage.UpdateData();
	}


	if (m_bFilter && !m_sFilter.IsEmpty()) {
		m_UnzipInfo.sFilter = (LPTSTR)(LPCTSTR)m_sFilter;
	}
	
	// Set the pointer to the filter, but only if it is not empty
	if (m_bExtractCheck && !m_sExtractFilter.IsEmpty()) {
		m_UnzipInfo.sExFilter = (LPTSTR)(LPCTSTR)m_sExtractFilter;
	}
	else {
		m_UnzipInfo.sExFilter = NULL;
	}
	if (m_bDontExtractCheck && !m_sDontExtractFilter.IsEmpty()) {
		m_UnzipInfo.sDontExFilter = (LPTSTR)(LPCTSTR)m_sDontExtractFilter;
	}
	else {
		m_UnzipInfo.sDontExFilter = NULL;
	}
}

BEGIN_MESSAGE_MAP(CZipALotDlg, CDialog)
	//{{AFX_MSG_MAP(CZipALotDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SOURCEPICKER, OnSourcepicker)
	ON_BN_CLICKED(IDC_TARGETPICKET, OnTargetpicket)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECKFILES, OnCheckFiles)
	ON_BN_CLICKED(IDC_FILTERBUTTON, OnFilterButton)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LOCKTARGET, OnTargetLock)
	ON_BN_CLICKED(IDC_SOURCESHOW, OnSourceShow)
	ON_BN_CLICKED(IDC_TARGETSHOW, OnTargetShow)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_USETEMP, OnUseTemp)
	ON_BN_CLICKED(IDC_WAREZWIZARD, OnWarezWizard)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_CBN_SELCHANGE(IDC_SOURCEDIR, OnSourceDirSelChange)
	ON_CBN_EDITCHANGE(IDC_SOURCEDIR, OnSourceDirEditChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZipALotDlg message handlers

BOOL CZipALotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ALWAYSONTOP);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ALWAYSONTOP, strAboutMenu);
		}
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}

	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_sFilter = "*.zip/*.rar/*.ace";


	m_SimplePage.m_pUnzipInfo = &m_UnzipInfo;

	m_SimplePage.m_psFilter = &m_sFilter;
	m_SimplePage.m_psExtractFilter = &m_sExtractFilter;
	m_SimplePage.m_psDontExtractFilter = &m_sDontExtractFilter;
	m_SimplePage.m_pbExtractCheck = &m_bExtractCheck;

	m_ExpertPage.m_pUnzipInfo = &m_UnzipInfo;
	
	m_ExpertPage.m_psFilter = &m_sFilter;
	m_ExpertPage.m_psExtractFilter = &m_sExtractFilter;
	m_ExpertPage.m_psDontExtractFilter = &m_sDontExtractFilter;

	
	m_ExpertPage.m_pbFilter = &m_bFilter;
	m_ExpertPage.m_pbExtractCheck = &m_bExtractCheck;
	m_ExpertPage.m_pbDontExtractCheck = &m_bDontExtractCheck;

	m_PropSheet.AddPage(&m_SimplePage);
	m_PropSheet.AddPage(&m_ExpertPage);


	m_PropSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	CTabCtrl * pTabCtrl = m_PropSheet.GetTabControl();
	DWORD dwStyle= pTabCtrl->GetStyle();
	pTabCtrl->ModifyStyle(0, TCS_HOTTRACK | TCS_RIGHTJUSTIFY | WS_TABSTOP);
	m_PropSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_PropSheet.ModifyStyle( 0, WS_TABSTOP);



	CRect rcSheet;
	GetDlgItem(IDC_PROPSHEET)->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_PropSheet.SetWindowPos(NULL, rcSheet.left-7, rcSheet.top-7, rcSheet.right - 100, rcSheet.bottom, 
			SWP_NOZORDER | SWP_NOACTIVATE);
	m_PropSheet.ModifyStyle(0, WS_EX_TOOLWINDOW);

	/*CUnzip unzipper(NULL);

	if (!unzipper.Init()) {
		CString sMsg = "Cannot find Unzip32.dll\r\nPlease download if from\r\nhttp://www.ractive.ch/gpl/download/Unzip32.dll";
		MessageBox(sMsg, "Error", MB_OK);
		EndDialog(1);
	}*/

	m_pUnzipDlg = NULL;

	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CHANGEDIR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_SOURCEPICKER))->SetIcon(hIcon);
		((CButton *)GetDlgItem(IDC_TARGETPICKER))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CHECKOUT), IMAGE_ICON, 43, 36, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_CHECKFILES))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_EXTRACT), IMAGE_ICON, 43, 36, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDOK))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_WIZARD), IMAGE_ICON, 43, 36, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_WAREZWIZARD))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CONFIG), IMAGE_ICON, 43, 36, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_CONFIG))->SetIcon(hIcon);
	}


	/*if (m_hIcon) {
		((CButton *)GetDlgItem(IDOK))->SetIcon(m_hIcon);
	}*/

	// TODO

	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDOK), IDOK);
	m_ToolTip.AddTool(GetDlgItem(IDC_CHECKFILES), IDC_CHECKFILES);
	m_ToolTip.AddTool(GetDlgItem(IDC_WAREZWIZARD), IDC_WAREZWIZARD);

	m_ToolTip.AddTool(GetDlgItem(IDC_SOURCEPICKER), IDC_SOURCEPICKER);
	m_ToolTip.AddTool(GetDlgItem(IDC_TARGETPICKER), IDC_TARGETPICKER);
	m_ToolTip.AddTool(GetDlgItem(IDC_FILTERBUTTON), IDC_FILTERBUTTON);
	m_ToolTip.AddTool(GetDlgItem(IDC_LOCKTARGET), IDC_LOCKTARGET);

	m_ToolTip.AddTool(GetDlgItem(IDC_SOURCEDIR), IDC_SOURCEDIR);
	m_ToolTip.AddTool(GetDlgItem(IDC_TARGETDIR), IDC_TARGETDIR);
	m_ToolTip.AddTool(GetDlgItem(IDC_SOURCESHOW), IDC_SOURCESHOW);
	m_ToolTip.AddTool(GetDlgItem(IDC_TARGETSHOW), IDC_TARGETSHOW);
	m_ToolTip.AddTool(GetDlgItem(IDC_CONFIG), IDC_CONFIG);

	GenTargetName();

	m_bFilterShown = TRUE;

	ShowDetails(m_bFilterSettings);

	FillSourceHistoryCombo();

	if (m_SourceCombo.GetCount() > 0) {
		m_SourceCombo.SetCurSel(0);
		m_SourceCombo.GetLBText(0, m_sSourceDir);
		GenTargetName();
	}

	if (!m_sStartSource.IsEmpty()) {
		m_sSourceDir = m_sStartSource;
		UpdateSourceHistory(m_sStartSource);
	}

	if (!m_sStartTarget.IsEmpty()) {
		m_sTargetDir = m_sStartTarget;
	}

	if (!m_sShellDir.IsEmpty()) {
		m_sSourceDir = m_sShellDir;
		UpdateData(FALSE);
		if (m_sStartTarget.IsEmpty()) {
			GenTargetName();
		}
	}

	m_bLockTarget = m_bTargetLockedStart;

	UpdateData(FALSE);

	OnTargetLock();

	DragAcceptFiles(TRUE);
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZipALotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == IDM_ALWAYSONTOP) {
		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu != NULL)
		{
			if (pSysMenu->GetMenuState(IDM_ALWAYSONTOP, MF_BYCOMMAND) == MF_CHECKED) {
				SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				pSysMenu->CheckMenuItem(IDM_ALWAYSONTOP, MF_UNCHECKED);
			}
			else {
				SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				pSysMenu->CheckMenuItem(IDM_ALWAYSONTOP, MF_CHECKED);
			}
		}
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZipALotDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZipALotDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CZipALotDlg::OnSourcepicker() 
{
	UpdateData();
	// Open a browse dialog
	CDirectoryPicker picker(m_hWnd, &m_sSourceDir);
	m_SourceCombo.SetCurSel(-1);
	UpdateData(FALSE);
	GenTargetName();
}

void CZipALotDlg::OnTargetpicket() 
{
	UpdateData();
	CDirectoryPicker picker(m_hWnd, &m_sTargetDir);
	ToolUpdate();
	UpdateData(FALSE);
}

void CZipALotDlg::OnOK() 
{
	TRACE("\r\n======================\r\n\r\n");
	m_PropSheet.PressButton(PSBTN_APPLYNOW);

	UpdateData();

	if (m_sSourceDir.IsEmpty() || m_sTargetDir.IsEmpty()) {
		MessageBox("Please enter the source and target directory!", "Error", MB_ICONEXCLAMATION);
		return;
	}
	// Check if the directories exist
	if (!DirExists(m_sSourceDir)) {
		MessageBox("The source directory does not exist. Please choose another one?", "Error", MB_ICONEXCLAMATION);
		return;
	}

	if (!DirExists(m_sTargetDir)) {
		if (MessageBox("The target directory does not exist. Do you want to create it?", "Warning", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
			if (!MakeSureDirectoryPathExists(m_sTargetDir + "\\")) {
				// The directory could not be created.
				DWORD dwError = GetLastError();
				CString sMsg = "The target directory could not be created\r\n\r\nThe Windows error message is:\r\n\r\n";
				sMsg += GetSystemMessage(dwError);
				MessageBox(sMsg, "Error", MB_ICONEXCLAMATION);
				return;
			}
		}
		else {
			// The user does not want to create the directory
			return;
		}
	}

	KillUnzipDlg();

	m_pUnzipDlg = new CUnzipDlg;
	m_pUnzipDlg->m_bCanClose = FALSE;
	m_pUnzipDlg->m_pbCloseWhenDone = &m_bCloseWhenDone;

	if (m_pUnzipDlg != NULL) {
		// Prepare the unzip dialog

		hStop = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!::IsWindow(m_pUnzipDlg->m_hWnd)) {
			m_pUnzipDlg->Create(IDD_UNZIPDLG, this);
		}
	}

	CExtract * pExtractor = NULL;

	// Process all the filters
	m_sFilter.Replace("/", " / ");
	m_UnzipInfo.sFilter = m_sFilter;
	
	int nCounter = 0;
	errorCode nRet;
	BOOL bNoArchives = TRUE;

	char seps[] = "/";
	char * token = strtok((LPTSTR)m_UnzipInfo.sFilter, seps);
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
				pExtractor = new CUnzip(m_pUnzipDlg);
				m_UnzipInfo.type = ZIP;
				break;
			case 2:
				pExtractor = new CUnrar(m_pUnzipDlg);
				m_UnzipInfo.type = RAR;
				break;
			case 3:
				pExtractor = new CUnace(m_pUnzipDlg);
				m_UnzipInfo.type = ACE;
				break;
		}
		
		if (pExtractor == NULL) {
			break;
		}

		m_UnzipInfo.sFilter = token;
		
		if (!pExtractor->Init()) {
			return;
		}
		pExtractor->unzipInfo = m_UnzipInfo;

		nRet = pExtractor->ExtractAll(m_sSourceDir, m_sTargetDir);
		bNoArchives &= nRet == EX_NO_ARCHIVES;
		TRACE("Return value thread %d\r\n", nRet);
		delete pExtractor;
		pExtractor = NULL;

		if (nRet == EX_CANCELLED) {
			m_pUnzipDlg->CancelStatus();
			break;
		}
		if (nRet == EX_OK) {
			
		}
		// Get next token:
		token = strtok( NULL, seps );
	}

	m_pUnzipDlg->m_bCanClose = TRUE;
	m_pUnzipDlg->UpdateData();
	m_bCloseWhenDone = *m_pUnzipDlg->m_pbCloseWhenDone;

	if (nRet == EX_CANCELLED) {
		m_pUnzipDlg->CancelStatus();
	}

	else if (bNoArchives) {
		KillUnzipDlg();
		MessageBox("No files found that could be extracted", "Info", MB_ICONINFORMATION);
	}

	else if (m_pUnzipDlg->CloseIfDone()) {
		KillUnzipDlg();	
	}
	else {
		m_pUnzipDlg->EndStatus();
	}


//	PostProcess();

	UpdateSourceHistory(m_sSourceDir);
}

void CZipALotDlg::OnClose() 
{
	KillUnzipDlg();
	
	CDialog::OnClose();
}

void CZipALotDlg::OnCancel() 
{
	KillUnzipDlg();
	
	CDialog::OnCancel();
}

BOOL CZipALotDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message== WM_LBUTTONDOWN || pMsg->message== WM_LBUTTONUP || pMsg->message== WM_MOUSEMOVE) {
		m_ToolTip.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CZipALotDlg::OnCheckFiles() 
{
	m_UnzipInfo.sSource = m_sSourceDir;
	m_UnzipInfo.sTarget = m_sTargetDir;

	CWaitCursor wait;
	UpdateData();
	CFileCheckDlg dlg(&m_UnzipInfo);
	dlg.DoModal();
}

void CZipALotDlg::OnSourceDirSelChange()
{
	int nSel = -1;

	if ((nSel = m_SourceCombo.GetCurSel()) != -1) {
		m_SourceCombo.GetLBText(nSel, m_sSourceDir);
	}

	GenTargetName();
}

void CZipALotDlg::OnSourceDirEditChange()
{
	m_SourceCombo.GetWindowText(m_sSourceDir);

	GenTargetName();
}


void CZipALotDlg::GenTargetName()
{
	if (m_sSourceDir.IsEmpty()) {
		return;
	}
	// Build the default unzip directory name
	if (m_sTargetDir.IsEmpty() || !m_bLockTarget) {
		m_sTargetDir = m_sSourceDir;
		if (m_sSourceDir.Right(1) != "\\")
			m_sTargetDir += "\\";
		
		m_sTargetDir += m_sUnzipped;
		
	}

	m_TargetBox.SetWindowText(m_sTargetDir);
	ToolUpdate();
}

void CZipALotDlg::ToolUpdate()
{
	// The tooltips shall show the full name of the directories
	m_ToolTip.UpdateTipText(m_sSourceDir, GetDlgItem(IDC_SOURCEDIR));
	m_ToolTip.UpdateTipText(m_sTargetDir, GetDlgItem(IDC_TARGETDIR));
}

void CZipALotDlg::OnFilterButton() 
{
	ShowDetails(!m_bFilterShown);
}

void CZipALotDlg::ShowDetails(BOOL bShow)
{
	if (m_bFilterShown && bShow) {
		return;
	}
	if (!m_bFilterShown && !bShow) {
		return;
	}


	RECT dlgrect, inforect, wholeRect;
	GetWindowRect(&dlgrect);
	m_PropCtrl.GetWindowRect(&inforect);
	static int offset = dlgrect.right - inforect.right;
	wholeRect = dlgrect;
	if (bShow) {
		// Expand the dialog
		wholeRect.right += ((inforect.right - inforect.left) + offset);
		SetDlgItemText(IDC_FILTERBUTTON, "<<< Filter settings");
		m_ToolTip.UpdateTipText("Close the filter settings...", GetDlgItem(IDC_FILTERBUTTON));
	}
	else {
		// Make the dialog smaller.
		wholeRect.right -= ((inforect.right - inforect.left) + offset);
		SetDlgItemText(IDC_FILTERBUTTON, "Filter settings >>>");
		m_ToolTip.UpdateTipText("Open the filter settings...", GetDlgItem(IDC_FILTERBUTTON));
	}
	
	// If the window is visible, we create a nice sliding efffect...
	if (IsWindowVisible()) {
		RECT updateRect;
		updateRect = dlgrect;

		// Get the desktopwindow
		CWnd * pWnd = GetDesktopWindow();

		int nStep = 16;

		// How many times to we have to update the window
		int nTimes = ((inforect.right - inforect.left) + offset) / nStep - 1;
		for(int x = 0; x < nTimes; x++) {
			if (bShow) {
				// Move the right border of the window to the right
				dlgrect.right += nStep;
				// Get the rectangle the window has grown
				updateRect.left = updateRect.right;
				updateRect.right = updateRect.left + nStep;
			}
			else {
				// Move the right border of the window to the left
				dlgrect.right -= nStep;
				// Get the rectangle the window became smaller
				updateRect.right = updateRect.left;
				updateRect.left = updateRect.right - nStep;
			}
			// Resize and repaint the dialog
			MoveWindow(&dlgrect, TRUE);
			// Update the new area
			pWnd->InvalidateRect(&updateRect);
			UpdateWindow();
			Sleep(10);
		}
	}

	m_bFilterShown = bShow;

	MoveWindow(&wholeRect, TRUE);


	/*if (bShow) {
		// Expand the dialog
		dlgrect.right += ((inforect.right - inforect.left) + offset);
		SetDlgItemText(IDC_FILTERBUTTON, "<<< Filter settings");
		m_bFilterShown = TRUE;
		m_ToolTip.UpdateTipText("Close the filter settings...", GetDlgItem(IDC_FILTERBUTTON));
	}
	else {
		// Make the dialog smaller.
		dlgrect.right -= ((inforect.right - inforect.left) + offset);
		SetDlgItemText(IDC_FILTERBUTTON, "Filter settings >>>");
		m_bFilterShown = FALSE;
		m_ToolTip.UpdateTipText("Open the filter settings...", GetDlgItem(IDC_FILTERBUTTON));
	}
	
	if (true || IsWindowVisible()) {
		RECT rtClip;

		m_PropSheet.GetClientRect(&rtClip);

		int nWidth = 16;

		if (bShow) {
			MoveWindow(&dlgrect);		
		}
		int x = 0;
		for(x = 0; x <= (inforect.right - inforect.left); x += nWidth) {
			rtClip.right = bShow ? x + 20 : inforect.right - x;
			m_PropSheet.ScrollWindow(bShow ? nWidth : -nWidth, 0, NULL, &rtClip);
			m_PropSheet.InvalidateRect(&rtClip, FALSE);
			m_PropSheet.UpdateWindow();
			Sleep(10);
		}
		if (!bShow) {
			MoveWindow(&dlgrect);		
		}

	}
	else {
		MoveWindow(&dlgrect);
	}*/
	
	/*MoveWindow(&dlgrect);
	for(int x = 0; x < 10; x++) {
		m_PropSheet.ScrollWindow(x, 0);
		m_PropSheet.InvalidateRect(NULL, TRUE);
		m_PropSheet.UpdateWindow();
		Sleep(20);
	}*/

	
	// Force the window to be redrawn
	/*InvalidateRect(NULL);
	UpdateWindow();*/
}

void CZipALotDlg::OnDestroy() 
{
	WinHelp(0l, HELP_QUIT);

	CDialog::OnDestroy();
}


BOOL CZipALotDlg::DirExists(CString sDir)
{
	if (sDir.Right(1) != "\\") {
		sDir += "\\";
	}

	sDir += "*.*";

	CFileFind finder;
	BOOL bFound = finder.FindFile(sDir);
	finder.Close();
	return bFound;
}

void CZipALotDlg::OnTargetLock()
{
	UpdateData();
	if (m_bLockTarget) {
		m_TargetBox.SetReadOnly(TRUE);
		m_TargetPickerButton.ModifyStyle(0, WS_DISABLED);
	}
	else {
		m_TargetBox.SetReadOnly(FALSE);
		m_TargetPickerButton.ModifyStyle(WS_DISABLED, 0);
	}

	// Update the window 
	RECT pickRect;
	m_TargetPickerButton.GetWindowRect(&pickRect);
	ScreenToClient(&pickRect);
	InvalidateRect(&pickRect, TRUE);

	UpdateWindow();
}


void CZipALotDlg::FillSourceHistoryCombo()
{
	m_SourceCombo.ResetContent();
	CZipALotApp * pApp = (CZipALotApp *)AfxGetApp();
	if (pApp) {
		CString sName;
		for (int i = 0; i < HISTORY_SIZE; i++) {
			pApp->GetMRUDirName(i, sName);
			if (!sName.IsEmpty()) {
				m_SourceCombo.AddString(sName);
			}
		}
	}

	UpdateData(FALSE);
}

void CZipALotDlg::UpdateSourceHistory(CString &sPath)
{
	if (!sPath.IsEmpty()) {
		AfxGetApp()->AddToRecentFileList(sPath);
		FillSourceHistoryCombo();
	}
}


void CZipALotDlg::OnSourceShow() 
{
	UpdateData();
	HINSTANCE hInst = ShellExecute(m_hWnd, "explore", m_sSourceDir, NULL, NULL, SW_SHOWDEFAULT);
	if ((long)hInst == ERROR_FILE_NOT_FOUND) {
		MessageBox("The directory " + m_sSourceDir + " doesn't exist!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}

void CZipALotDlg::OnTargetShow() 
{
	UpdateData();
	UpdateData();
	HINSTANCE hInst = ShellExecute(m_hWnd, "explore", m_sTargetDir, NULL, NULL, SW_SHOWDEFAULT);
	if ((long)hInst == ERROR_FILE_NOT_FOUND) {
		MessageBox("The directory " + m_sTargetDir + " doesn't exist!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}

void CZipALotDlg::OnDropFiles(HDROP hDropInfo) 
{

	CString sFileName;
	// Get the first filename
	DragQueryFile(hDropInfo, 0, sFileName.GetBuffer(_MAX_PATH), _MAX_PATH);
	sFileName.ReleaseBuffer();
	CFileFind finder;
	if (finder.FindFile(sFileName)) {
		finder.FindNextFile();
		// Remove any selection from the combo box
		m_SourceCombo.SetCurSel(-1);
		if (finder.IsDirectory()) {
			m_sSourceDir = sFileName;
		}
		else {
			// Cut off the path if it's a file
			m_sSourceDir = sFileName.Left(sFileName.ReverseFind('\\'));
			if (m_sSourceDir.GetAt(1) == ':') {
				// Append Backslash to drive names
				m_sSourceDir += "\\";
			}
		}
		UpdateData(FALSE);
		GenTargetName();
	}
	finder.Close();

	CDialog::OnDropFiles(hDropInfo);
}

void CZipALotDlg::PostProcess()
{
	CPostProcessDlg dlg(m_sTargetDir);
	dlg.DoModal();
}

void CZipALotDlg::OnUseTemp() 
{
	CString sTemp;
	LPTSTR sTempBuf = m_sTargetDir.GetBuffer(_MAX_PATH);
	GetTempPath(_MAX_PATH, sTempBuf);
	m_sTargetDir.ReleaseBuffer();
	
	m_bLockTarget = TRUE;
	UpdateData(FALSE);
	OnTargetLock();	
}

void CZipALotDlg::OnWarezWizard() 
{
	CWizSheet propSheet(IDC_WAREZWIZARD);

	*propSheet.m_Data.m_psCurSource = m_sSourceDir;
	*propSheet.m_Data.m_psCurTarget = m_sTargetDir;
	propSheet.m_Data.m_sDefaultInstallDir = m_sDefaultInstallDir;

	CWizPageSource wizSource;
	CWizPageTarget wizTarget;
	wizTarget.m_sUnzipped = m_sUnzipped;
	CWizPageNFO wizNFO;
	CWizPageAction wizPageAction;
	CWizUnzip wizUnzip;
	CWizEnd wizEnd;
	CWizError wizError;

	/*wizPage1.m_psSource = &propSheet.m_sSource;

	wizPage2.m_psTarget = &propSheet.m_sTarget;
	wizPage2.m_psSource = &propSheet.m_sSource;
	wizPage2.m_psTempExtractDir = &propSheet.m_sTempExtractDir;

	wizPage3.m_psTempExtractDir = &propSheet.m_sTempExtractDir;

	wizPage4.m_psTempExtractDir = &propSheet.m_sTempExtractDir;
	wizPage4.m_psTarget = &propSheet.m_sTarget;
	wizPage4.m_psSource = &propSheet.m_sSource;

	wizPage4.m_psFileToExtract = &propSheet.m_sFileToExtract;
	wizPage4.m_psGameDir = &m_sDefaultGameDir;
	wizPage4.m_psInstalledDir = &propSheet.m_sInstalledDir;

	wizUnzip.m_psTarget = &propSheet.m_sTarget;
	wizUnzip.m_psSource = &propSheet.m_sSource;
	wizUnzip.m_pError = &propSheet.error;
	wizUnzip.m_psFileToExtract = &propSheet.m_sFileToExtract;

	wizError.m_pError = &propSheet.error;*/

	propSheet.AddPage(&wizSource);
	propSheet.AddPage(&wizTarget);
	propSheet.AddPage(&wizUnzip);
	propSheet.AddPage(&wizNFO);
	propSheet.AddPage(&wizPageAction);
	propSheet.AddPage(&wizEnd);
	propSheet.AddPage(&wizError);

	propSheet.SetWizardMode();
	propSheet.DoModal();
	
}

void CZipALotDlg::KillUnzipDlg()
{
	if (m_pUnzipDlg == NULL) {
		return;
	}
	TRACE("Kill DLG\r\n");
	m_pUnzipDlg->Kill();
	delete m_pUnzipDlg;
	m_pUnzipDlg = NULL;
	TRACE("DLG killed\r\n");
}

void CZipALotDlg::OnConfig() 
{
	CConfigDlg dlg;
	dlg.m_sInstallDir = m_sDefaultInstallDir;
	dlg.m_sStartSource = m_sStartSource;
	dlg.m_sStartTarget = m_sStartTarget;
	dlg.m_sUnzipped = m_sUnzipped;
	dlg.m_bTargetLocked = m_bTargetLockedStart;
	dlg.m_bCloseWhenDone = m_bCloseWhenDone;
	dlg.m_bFilterSettings = m_bFilterSettings;
	dlg.DoModal();
	m_sDefaultInstallDir = dlg.m_sInstallDir;
	m_sStartSource = dlg.m_sStartSource;
	m_sStartTarget = dlg.m_sStartTarget;
	m_sUnzipped = dlg.m_sUnzipped;
	m_bTargetLockedStart = dlg.m_bTargetLocked;
	m_bCloseWhenDone = dlg.m_bCloseWhenDone;
	m_bFilterSettings = dlg.m_bFilterSettings;
	if (dlg.m_bHistoryDeleted) {
		FillSourceHistoryCombo();
	}
	
	//GenTargetName();
}

