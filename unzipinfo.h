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

#ifndef __unzip_h     /* don't include more than once */
#define __unzip_h

#define WM_EXTRACT_RANGE		(WM_USER + 1)
#define WM_EXTRACT_STEPIT		(WM_USER + 2)
#define WM_EXTRACT_CURFILE		(WM_USER + 3)
#define WM_EXTRACT_CURZIPFILE	(WM_USER + 4)
#define WM_EXTRACT_ERROR		(WM_USER + 5)
#define WM_EXTRACT_SETSOURCE	(WM_USER + 6)
#define WM_EXTRACT_SETTARGET	(WM_USER + 7)


typedef enum{EX_OK, EX_UNKNOWN, EX_EXISTS, EX_ARCHIVE, EX_MEM, EX_NO_ARCHIVES, EX_CANCELLED, EX_NO_FILES, EX_DISK, EX_EOF, EX_UNSUP, EX_BADPWD, EX_CRC, EX_OPEN, EX_CLOSE, EX_WRITE, EX_READ, EX_CREATE, EX_BUF} errorCode;

typedef enum{ACE, ZIP, RAR, BZ2, CAB, NO_TYPE} archiveType;

struct tagUnzipInfo {
	LPCTSTR sTarget;
	LPCTSTR sSource;
	LPCTSTR sFilter;
	LPCTSTR sExFilter;
	LPCTSTR sDontExFilter;
	BOOL bOnlyNewer;
	BOOL bRecreateDir;
	BOOL bExtract;
	BOOL bOverwrite;
	BOOL bCaseInsensitive;
	BOOL bOwnDir;
	archiveType type;
};

#endif __unzip_h