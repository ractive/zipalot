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

// DirectoryPicker.h: interface for the CDirectoryPicker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTORYPICKER_H__9040CBB0_B95C_11D3_835F_0000B4A265C3__INCLUDED_)
#define AFX_DIRECTORYPICKER_H__9040CBB0_B95C_11D3_835F_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectoryPicker  
{
public:
	CDirectoryPicker(HWND hWnd, CString * sStartDir);
	virtual ~CDirectoryPicker();
private:
	static CString * m_sDirectory;
	HWND m_hWnd;
	void BrowseFolder();
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData);

};

#endif // !defined(AFX_DIRECTORYPICKER_H__9040CBB0_B95C_11D3_835F_0000B4A265C3__INCLUDED_)
