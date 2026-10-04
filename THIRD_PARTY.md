# Third-party code in ZipALot

ZipALot itself is licensed under the GNU General Public License v2 (see
`LICENSE`). It drives three unpacker DLLs through their public C interfaces; the
DLLs are **not** part of this repository, only the header files that describe
their APIs. Licence statements below are summarised from the file headers, which
remain authoritative.

## Unpacker DLL interfaces (headers included, DLLs not included)

| Component | Files | Origin | Terms |
|---|---|---|---|
| Info-ZIP UnZip 5.4 DLL (`unzip32.dll`) | `structs.h`, `unzver.h`, return codes in `Extract.h` | Info-ZIP (www.info-zip.org) | Info-ZIP licence (BSD-like): free use and redistribution with notice; `unzver.h` states `UNZ_DLL_VERSION "5.4"` and `COMPANY_NAME "Info-ZIP"`. |
| RARLAB `unrar.dll` | `UNRAR_.H` | Alexander Roshal, RARLAB (www.rarlab.com) | The `unrar.dll` API header carries no notice in this copy. RARLAB's unrar licence allows free use of the DLL in other software but forbids using it to re-create the RAR compression algorithm. |
| ACE `UnAceV2.dll` | `UNACE_.H`, `STRUCS.H`, `CALLBACK.H`, `UNACEFNC.H` | ACE Compression Software (Marcel Lemke) | Header notice, quoted: "The use of this Dll within another program than the ACE-Archiver itself has to be agreet upon contract. Any use of this Dll without such a contract or any violation of such a contract is seen as an attack against the copyright on the archiver ACE. This copyright is holded by ACE Compression Software, the create of the ACE archiver." In 2001 ACE made the DLL available free of charge for use in other unpackers; the DLL itself is nevertheless not redistributed here. See the README for the 2019 security advisory. |
| bzip2 / libbzip2 1.0 | `bzlib.h` (used by the stub `Bunzip2.cpp`) | Julian Seward | BSD-style licence quoted in the header: redistribution permitted with notice, no misrepresentation of origin, altered versions marked, no endorsement. |

## Included source code by other authors

| Component | Files | Author | Licence (as stated in the files) |
|---|---|---|---|
| Regular expressions (`CRegExp`) | `RegExp.h/.cpp` | Henry Spencer (University of Toronto, 1986/1993/1995); C++/TCHAR wrapper by an unnamed CodeGuru contributor | "Permission is granted to anyone to use this software for any purpose on any computer system, and to redistribute it in any way, subject to the following restrictions: 1. The author is not responsible for the consequences of use of this software, no matter how awful, even if they arise from defects in it. 2. The origin of this software must not be misrepresented, either by explicit claim or by omission. 3. Altered versions must be plainly marked as such, and must not be misrepresented (by explicit claim or omission) as being the original software. 4. This notice must not be removed or altered." |
| CHyperLink | `HyperLink.h/.cpp` | Chris Maunder, 1997-1998 | "Feel free to use and distribute. May not be sold for profit." |
| CRegBinding | `RegBinding.h/.cpp` | Joerg Dentler, 1998 | Permissive four-clause notice (same wording as Henry Spencer's regex licence above). |
| Pete's AVI collection | `res/extract.avi`, `help-src/dateien.avi` | "Pete's AVI files", a freely distributed collection of Windows animation clips | Distributed free for use in applications. |
| MFC AppWizard help skeleton | `hlp/AfxDlg.rtf`, `MakeHelp.bat` | Microsoft Visual C++ 6.0 AppWizard | Microsoft sample code generated for the project. |

## Build tools (not included)

Microsoft Visual C++ 6.0 with MFC, NSIS (Nullsoft Scriptable Install System,
`zipalot.nsi`), Microsoft Help Workshop (`hlp/ZipALot.hpj`) and the "Windows
Help Designer" used for the project in `help-src/`.
