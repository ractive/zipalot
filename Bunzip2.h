// Bunzip2.h: interface for the CBunzip2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUNZIP2_H__7B9FCE0F_6210_4E44_AEF7_7C5D37955643__INCLUDED_)
#define AFX_BUNZIP2_H__7B9FCE0F_6210_4E44_AEF7_7C5D37955643__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Extract.h"
#include "bzlib.h"

class CBunzip2 : public CExtract  
{
public:
	CBunzip2(CUnzipDlg * pUnzipDlg);
	virtual ~CBunzip2();

};

#endif // !defined(AFX_BUNZIP2_H__7B9FCE0F_6210_4E44_AEF7_7C5D37955643__INCLUDED_)
