#if !defined(AFX_CONFIGDLG_H__53AC4555_7206_4015_8FD3_3A2D71C39D85__INCLUDED_)
#define AFX_CONFIGDLG_H__53AC4555_7206_4015_8FD3_3A2D71C39D85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bHistoryDeleted;
// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIG };
	CString	m_sInstallDir;
	CString	m_sStartSource;
	CString	m_sStartTarget;
	CString	m_sUnzipped;
	BOOL	m_bTargetLocked;
	BOOL	m_bCloseWhenDone;
	BOOL	m_bFilterSettings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSourcePicker();
	afx_msg void OnTargetPicker();
	afx_msg void OnInstallPicker();
	afx_msg void OnDeleteHistory();
	afx_msg void OnNewVersion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadURL(char * buf, DWORD * pdwBufLen);
	int IsNewestVersion();	// return value:
							// -1 error
							// 0 no new version
							// 1 newer versino available
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__53AC4555_7206_4015_8FD3_3A2D71C39D85__INCLUDED_)
