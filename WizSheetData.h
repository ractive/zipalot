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

#ifndef __wizdata_h     /* don't include more than once */
#define __wizdata_h

#include "unzipinfo.h"

class tagWizData {
public:
	tagWizData() {
		m_psCurSource = &m_sSource;
		m_psCurTarget = &m_sExtractTarget;
		m_nNextPage = -1;
		m_Error = EX_OK;
	};
	CString GetExtractDir() { return m_sExtractTarget;};
	void NoExtractDir() { m_sExtractTarget.Empty();};
	CString * m_psCurSource;
	CString * m_psCurTarget;
	CString m_sInstallationDir;
	CString m_sDefaultInstallDir;
	CString m_sExtractFilter;
	errorCode m_Error;
	int m_nNextPage;
	archiveType m_Type;
private:
	CString m_sSource;
	CString m_sExtractTarget;
};

#endif __wizdata_h