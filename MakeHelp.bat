@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by ZIPALOT.HPJ. >"hlp\ZipALot.hm"
echo. >>"hlp\ZipALot.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\ZipALot.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\ZipALot.hm"
echo. >>"hlp\ZipALot.hm"
echo // Prompts (IDP_*) >>"hlp\ZipALot.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\ZipALot.hm"
echo. >>"hlp\ZipALot.hm"
echo // Resources (IDR_*) >>"hlp\ZipALot.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\ZipALot.hm"
echo. >>"hlp\ZipALot.hm"
echo // Dialogs (IDD_*) >>"hlp\ZipALot.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\ZipALot.hm"
echo. >>"hlp\ZipALot.hm"
echo // Frame Controls (IDW_*) >>"hlp\ZipALot.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\ZipALot.hm"
REM -- Make help for Project ZIPALOT


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\ZipALot.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\ZipALot.hlp" goto :Error
if not exist "hlp\ZipALot.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\ZipALot.hlp" Debug
if exist Debug\nul copy "hlp\ZipALot.cnt" Debug
if exist Release\nul copy "hlp\ZipALot.hlp" Release
if exist Release\nul copy "hlp\ZipALot.cnt" Release
echo.
goto :done

:Error
echo hlp\ZipALot.hpj(1) : error: Problem encountered creating help file

:done
echo.
