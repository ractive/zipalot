// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "zipalot.h"
#include "ConfigDlg.h"
#include "DirectoryPicker.h"
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_sInstallDir = _T("");
	m_sStartSource = _T("");
	m_sStartTarget = _T("");
	m_sUnzipped = _T("");
	m_bTargetLocked = FALSE;
	m_bCloseWhenDone = FALSE;
	m_bFilterSettings = FALSE;
	//}}AFX_DATA_INIT

	m_bHistoryDeleted = FALSE;
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_Text(pDX, IDC_INSTALLDIR, m_sInstallDir);
	DDX_Text(pDX, IDC_STARTSOURCE, m_sStartSource);
	DDX_Text(pDX, IDC_STARTTARGET, m_sStartTarget);
	DDX_Text(pDX, IDC_UNZIPPED, m_sUnzipped);
	DDX_Check(pDX, IDC_TARGETLOCKED, m_bTargetLocked);
	DDX_Check(pDX, IDC_CLOSEDONE, m_bCloseWhenDone);
	DDX_Check(pDX, IDC_FILTERSETTINGS, m_bFilterSettings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_BN_CLICKED(IDC_SOURCEPICKER, OnSourcePicker)
	ON_BN_CLICKED(IDC_TARGETPICKER, OnTargetPicker)
	ON_BN_CLICKED(IDC_INSTALLPICKER, OnInstallPicker)
	ON_BN_CLICKED(IDC_DELETEHIST, OnDeleteHistory)
	ON_BN_CLICKED(IDC_NEWVERSION, OnNewVersion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	HICON hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CHANGEDIR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_SOURCEPICKER))->SetIcon(hIcon);
		((CButton *)GetDlgItem(IDC_TARGETPICKER))->SetIcon(hIcon);
		((CButton *)GetDlgItem(IDC_INSTALLPICKER))->SetIcon(hIcon);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnSourcePicker() 
{
	CDirectoryPicker picker(m_hWnd, &m_sStartSource);
	UpdateData(FALSE);
}

void CConfigDlg::OnTargetPicker() 
{
	CDirectoryPicker picker(m_hWnd, &m_sStartTarget);
	UpdateData(FALSE);
	
}

void CConfigDlg::OnInstallPicker() 
{
	CDirectoryPicker picker(m_hWnd, &m_sInstallDir);
	UpdateData(FALSE);
	
}

void CConfigDlg::OnDeleteHistory() 
{
	((CZipALotApp *)AfxGetApp())->RemoveAllMRUs();	
	m_bHistoryDeleted = TRUE;
}

int CConfigDlg::IsNewestVersion()
{
	TCHAR sNewestVersion[10] = "";
	DWORD nLen = 10;
	ReadURL(sNewestVersion, &nLen);
	if (strlen(sNewestVersion) == 0 || strcmp(sNewestVersion, "error") == 0) {
		MessageBox("The version information could not be fetched from www.ractive.ch", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}
	CString sOurFileName;
	GetModuleFileName(NULL, sOurFileName.GetBuffer(_MAX_PATH), _MAX_PATH);
	sOurFileName.ReleaseBuffer();

	DWORD dwVerHnd;
	DWORD dwVerInfoSize = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)sOurFileName, &dwVerHnd);

	if (dwVerInfoSize) {
		LPSTR lszVer = NULL;
		LPSTR lszVerName = NULL;
		UINT  cchVer = 0;


		LPSTR lpstrVffInfo = new char[dwVerInfoSize];

		if (GetFileVersionInfo((LPTSTR)(LPCTSTR)sOurFileName, 0L, dwVerInfoSize, lpstrVffInfo)) {
			BOOL bRet = VerQueryValue(lpstrVffInfo, TEXT("\\StringFileInfo\\040904b0\\FileVersion"), (void **)&lszVer, &cchVer);

			if (!bRet) {
				delete [] lpstrVffInfo;
				return -1;
			}
			else if (lstrcmpi(lszVer, sNewestVersion) == 0) {
				return 0;
			}
		}

		delete [] lpstrVffInfo;
		
		return 1;

	}
	
	return -1;
}

void CConfigDlg::OnNewVersion() 
{
	int nIsNewest = IsNewestVersion();
	if (nIsNewest == -1) {
		MessageBox("The version information could not be fetched!", "Error", MB_OK);
		return;	
	}
	if (nIsNewest == 0) {
		MessageBox("You already have the newest version", "Info", MB_OK| MB_ICONINFORMATION);	
		return;
	}

	if (MessageBox("A new version of ZipALot was found!\r\n\r\nDo you want to download it?", "New Version found", MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return;
	}

	ShellExecute(m_hWnd, "open", "http://www.ractive.ch/gpl/download/ZipALot.exe", NULL, NULL, SW_SHOWNORMAL);
}

void CConfigDlg::ReadURL(char * buf, DWORD * pdwBufLen)
{
	CWaitCursor wait;
	const int nReadBlockSize = 1024;

	CInternetSession session(NULL, 1, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_DONT_CACHE/* | INTERNET_FLAG_ASYNC*/);
	CHttpConnection * pServer;
	CHttpFile *pHttpFile;

	CString sServerName;
	CString sObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;
	CString sURL = "http://www.ractive.ch/gpl/zalver.php";
	AfxParseURL(sURL, dwServiceType, sServerName, sObject, nPort);

	pServer = session.GetHttpConnection(sServerName, nPort);

	DWORD dwHttpRequestFlags = 	INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_RELOAD;

	pHttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, sObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
	CString sHeaders(_T("Accept: text/*\r\nUser-Agent: MyIP Windows client\r\n"));

	pHttpFile->AddRequestHeaders(sHeaders);
	pHttpFile->SendRequest();

	if (*pdwBufLen == 0) {
		DWORD dwBufLen = sizeof(DWORD);
		pHttpFile->QueryInfo(HTTP_QUERY_FLAG_NUMBER  | HTTP_QUERY_CONTENT_LENGTH, pdwBufLen, &dwBufLen);
	}
	else {

		char sz[nReadBlockSize];
		memset(sz, 0, nReadBlockSize);

		int nPos = 0;


		unsigned int nCounter = 0;
		UINT nRead = 0;
		// Now the HTML code which is given by the CGI script is read in now.
		while (nRead = pHttpFile->Read(sz, nReadBlockSize)) {
			nCounter += nRead;
			
			if (nCounter >= *pdwBufLen) {
				// The buffer is too small
				return;
			}
			strcat(buf, sz);
		}
	}

	pHttpFile->Close();
	pServer->Close();
	delete pServer;
	delete pHttpFile;
	pServer = NULL;
	pHttpFile = NULL;
}
