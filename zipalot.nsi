Name "ZipALot"
LicenseText "Please read and agree to this license before continuing."
LicenseData license.txt
ComponentText "This will install ZipALot on your system. Select which options you want set up."
DirText "Select a directory to install Spinner Plus in."
UninstallText "This will uninstall ZipALot. Hit Next to uninstall, or Cancel to cancel."
UninstallExeName uninst-zipalot.exe
OutFile ZipALot.exe
InstType Typical

InstallDir "$PROGRAMFILES\ZipALot"
SetOverwrite on

Section "ZipALot (required)"
	SetOutPath $INSTDIR
	File Release\ZipALot.exe
	File Release\unzip32.dll
	File Release\unrar.dll
	File Release\unacev2.dll		
	File ReadMe.txt
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZipALot" "DisplayName" "ZipALot (remove only)"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZipAlot" "UninstallString" '"$INSTDIR\uninst-zipalot.exe"'
	WriteRegStr HKEY_CURRENT_USER "Software\@ractive Software" "ZipALot" "ZipALot"
SectionEnd

Section "View Readme.txt"
	SectionIn 1
	Exec '"$WINDIR\notepad.exe" "$INSTDIR\readme.txt"'
SectionEnd

Section "Integrate into shell (explorer)"
	SectionIn 1
	WriteRegStr HKEY_CLASSES_ROOT "Directory\Shell\ZipALot" "" "Extract archives with ZipALot"
	WriteRegStr HKEY_CLASSES_ROOT "Directory\Shell\ZipALot\command" "" '"$INSTDIR\ZipALot.exe" "%1"'
	WriteRegStr HKEY_CLASSES_ROOT "Drive\Shell\ZipALot" "" "Extract all archives in this folder..."
	WriteRegStr HKEY_CLASSES_ROOT "Drive\Shell\ZipALot\command" "" '"$INSTDIR\ZipALot.exe" "%1"'
SectionEnd

Section "Add Icon to Start Menu"
	SectionIn 1
	CreateShortCut "$SMPROGRAMS\ZipALot.lnk" "$INSTDIR\ZipALot.exe" "" "" 0
SectionEnd

Section "Add Icon to Desktop"
	CreateShortCut "$DESKTOP\ZipALot.lnk" "$INSTDIR\ZipALot.exe" "" "" 0
SectionEnd

Section "Add Icon to Quicklauch Bar"
	CreateShortCut "$QUICKLAUNCH\ZipALot.lnk" "$INSTDIR\ZipALot.exe" "" "" 0
SectionEnd

Section "Launch ZipALot after installation"
	Exec '"$INSTDIR\ZipALot.exe"'
SectionEnd

Section Uninstall
	DeleteRegKey HKEY_CURRENT_USER "Software\@ractive Software\ZipALot"
	DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZipALot"
	DeleteRegKey HKEY_CLASSES_ROOT "Directory\Shell\ZipALot"

	;DeleteRegValue HKEY_CLASSES_ROOT "Directory\Shell\ZipALot" ""
	;DeleteRegValue HKEY_CLASSES_ROOT "Directory\Shell\ZipALot\command" ""
	;DeleteRegValue HKEY_CLASSES_ROOT "Drive\Shell\ZipALot" ""
	;DeleteRegValue HKEY_CLASSES_ROOT "Drive\Shell\ZipALot\command" ""
	;DeleteRegKey HKEY_CLASSES_ROOT "Directory\Shell\ZipALot\command"		

	Delete "$DESKTOP\ZipALot.lnk"
	Delete "$SMPROGRAMS\ZipALot.lnk"
	Delete "$QUICKLAUNCH\ZipALot.lnk"
	Delete $INSTDIR\*.*
	RMDir $INSTDIR
	
	Delete "$SMPROGRAMS\ZipALot.lnk"
	Delete "$DESKTOP\ZipALot.lnk"
	Delete "$QUICKLAUNCH\ZipALot.lnk"
SectionEnd