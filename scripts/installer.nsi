Name "theBall"
OutFile "${OUTPUT}"
InstallDir $PROGRAMFILES\theBall
SetCompressor /SOLID lzma
Icon "..\artworks\icon.ico"
XPStyle on

Page license
Page directory
Page instfiles

LicenseData "release\LICENCE.txt"

UninstPage uninstConfirm
UninstPage instfiles

Section "Install"

	SetOutPath $INSTDIR

	; Files
	File /r /x "*.lnk" /x "*.bat" /x "*.psd" /x "*.blend" /x "*.blend1" /x "*.flp" /x "profiles.xml" "..\release\*"

	; Shortcut
	CreateDirectory "$SMPROGRAMS\theBall"
	CreateShortCut "$SMPROGRAMS\theBall\theBall.lnk" "$INSTDIR\theball.exe"
	CreateShortCut "$SMPROGRAMS\theBall\LisezMoi.lnk" "$INSTDIR\README.txt"
	CreateShortCut "$SMPROGRAMS\theBall\Manuel du jeu.lnk" "$INSTDIR\MANUAL.txt"
	CreateShortCut "$SMPROGRAMS\theBall\Editeur de carte.lnk" "$INSTDIR\EDITOR.txt"
	CreateShortCut "$SMPROGRAMS\theBall\Note de version.lnk" "$INSTDIR\CHANGES.txt"
	CreateShortCut "$SMPROGRAMS\theBall\D�installer.lnk" "$INSTDIR\uninstall.exe"
	CreateShortCut "$DESKTOP\theBall.lnk" "$INSTDIR\theball.exe"

	; Uninstaller reg
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\theBall" "DisplayName" "theBall (remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\theBall" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteUninstaller "uninstall.exe"

SectionEnd

Section "Uninstall"

	; Files
	RMDir /r "$INSTDIR"

	; Shortcut
	RMDir /r "$SMPROGRAMS\theBall"
	Delete "$DESKTOP\theBall.lnk"

	; Uninstaller reg
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\theBall"

SectionEnd
