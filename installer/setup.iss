; SPDX-License-Identifier: GPL-2.0-or-later
#ifndef AppVersion
  #define AppVersion "0.3.2"
#endif
#ifndef ProjectURL
  #define ProjectURL "https://github.com/1728643928/obs-gpu-encoder"
#endif
#ifndef RequiredPrivileges
  #define RequiredPrivileges "admin"
#endif
[Setup]
AppId=GPUEncoderBridge-{code:InstallationId}
AppName=GPU Encoder Bridge for OBS
AppVersion={#AppVersion}
AppPublisher=GPU Encoder Bridge contributors
AppPublisherURL={#ProjectURL}
AppSupportURL={#ProjectURL}/issues
DefaultDirName={autopf}\obs-studio
AppendDefaultDirName=no
UsePreviousAppDir=no
UsePreviousLanguage=no
ShowLanguageDialog=yes
DisableDirPage=no
DisableWelcomePage=no
DisableProgramGroupPage=yes
PrivilegesRequired={#RequiredPrivileges}
PrivilegesRequiredOverridesAllowed=commandline
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
OutputDir=..\release
OutputBaseFilename=GPU-Encoder-Bridge-{#AppVersion}-windows-x64-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes
UninstallDisplayName=GPU Encoder Bridge for OBS ({app})
UninstallFilesDir={app}\data\obs-plugins\obs-gpu-encoder\uninstall
UninstallDisplayIcon={app}\bin\64bit\obs64.exe
CloseApplications=no
RestartApplications=no
DirExistsWarning=no
[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "zh"; MessagesFile: "languages\ChineseSimplified.isl"
Name: "tw"; MessagesFile: "languages\ChineseTraditional.isl"
Name: "ja"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "ko"; MessagesFile: "compiler:Languages\Korean.isl"
#include "messages.iss"
[Files]
Source: "..\dist\obs-plugins\64bit\obs-gpu-encoder.dll"; DestDir: "{app}\obs-plugins\64bit"; Flags: ignoreversion
Source: "..\dist\obs-plugins\64bit\obs-gpu-encoder-worker.exe"; DestDir: "{app}\obs-plugins\64bit"; Flags: ignoreversion
Source: "..\dist\data\obs-plugins\obs-gpu-encoder\locale\*.ini"; DestDir: "{app}\data\obs-plugins\obs-gpu-encoder\locale"; Flags: ignoreversion
Source: "..\LICENSE"; DestDir: "{app}\data\obs-plugins\obs-gpu-encoder"; Flags: ignoreversion
Source: "..\THIRD-PARTY.md"; DestDir: "{app}\data\obs-plugins\obs-gpu-encoder"; Flags: ignoreversion
Source: "..\third-party\*"; DestDir: "{app}\data\obs-plugins\obs-gpu-encoder\third-party"; Flags: ignoreversion recursesubdirs createallsubdirs
[Code]
var
  DiscoveryPage: TWizardPage;
  Results: TNewListBox;
  ScanButton, StopButton: TNewButton;
  ScanStatus, ScanHelp: TNewStaticText;
  Pending: TStringList;
  SearchRec: TFindRec;
  Searching, Scanning, HaveFindHandle, WizardReady: Boolean;
  CurrentDir: String;
  ScanTimer: UINT_PTR;
  Scanned: Integer;
function SetTimer(hWnd: HWND; nIDEvent: UINT_PTR; uElapse: UINT; lpTimerFunc: LongWord): UINT_PTR;
  external 'SetTimer@user32.dll stdcall';
function KillTimer(hWnd: HWND; nIDEvent: UINT_PTR): Boolean;
  external 'KillTimer@user32.dll stdcall';
function DriveType(lpRootPathName: String): UINT;
  external 'GetDriveTypeW@kernel32.dll stdcall';
function PathAttributes(lpFileName: String): Cardinal;
  external 'GetFileAttributesW@kernel32.dll stdcall';
function OpenFileForWrite(lpFileName: String; Access, Share: Cardinal; Security: LongWord; Disposition, Flags: Cardinal; Template: LongWord): LongWord;
  external 'CreateFileW@kernel32.dll stdcall';
function CloseFileHandle(Handle: LongWord): Boolean;
  external 'CloseHandle@kernel32.dll stdcall';
function FileIsAvailable(const FileName: String): Boolean;
var Handle: LongWord;
begin
  Result := True;
  if not FileExists(FileName) then Exit;
  Handle := OpenFileForWrite(FileName, $40000000, 0, 0, 3, 0, 0);
  Result := Handle <> $FFFFFFFF;
  if not Result then Log('Target file unavailable: ' + FileName + ' error=' + IntToStr(DLLGetLastError));
  if Result then CloseFileHandle(Handle);
end;
function IsOBSRoot(const Root: String): Boolean;
begin
  Result := FileExists(AddBackslash(Root) + 'bin\64bit\obs64.exe') and
    FileExists(AddBackslash(Root) + 'bin\64bit\obs.dll');
end;
function ValidOBS(const Root: String): Boolean;
var MS, LS: Cardinal;
begin
  Result := False;
  if not IsOBSRoot(Root) then Exit;
  if not GetVersionNumbers(AddBackslash(Root) + 'bin\64bit\obs64.exe', MS, LS) then Exit;
  Result := ((MS shr 16) = 33) and
    FileExists(AddBackslash(Root) + 'bin\64bit\obs-frontend-api.dll') and
    FileExists(AddBackslash(Root) + 'bin\64bit\avcodec-62.dll') and
    FileExists(AddBackslash(Root) + 'bin\64bit\avutil-60.dll');
end;
function InstallationId(Param: String): String;
var Root: String;
begin
  if WizardReady then Root := WizardDirValue
  else Root := ExpandConstant('{param:DIR|{autopf}\obs-studio}');
  Result := Copy(GetSHA256OfString(Lowercase(RemoveBackslashUnlessRoot(Root))), 1, 24);
end;
procedure AddCandidate(Root: String);
var I: Integer;
begin
  Root := RemoveBackslashUnlessRoot(Root);
  if not IsOBSRoot(Root) then Exit;
  for I := 0 to Results.Items.Count - 1 do
    if CompareText(Results.Items[I], Root) = 0 then Exit;
  Results.Items.Add(Root);
  if Results.ItemIndex < 0 then Results.ItemIndex := 0;
end;
procedure StopScan(Sender: TObject);
begin
  Scanning := False;
  if ScanTimer <> 0 then begin KillTimer(0, ScanTimer); ScanTimer := 0; end;
  if HaveFindHandle then begin FindClose(SearchRec); HaveFindHandle := False; end;
  Searching := False;
  Pending.Clear;
  ScanButton.Enabled := True;
  StopButton.Enabled := False;
  WizardForm.NextButton.Enabled := True;
  WizardForm.BackButton.Enabled := True;
  ScanStatus.Caption := FmtMessage(CustomMessage('ScanDone'), [IntToStr(Scanned), IntToStr(Results.Items.Count)]);
end;
procedure ScanTick(Wnd: HWND; Msg: UINT; TimerID: UINT_PTR; Time: DWORD);
var I, Last: Integer; Child: String;
begin
  if not Scanning then Exit;
  { At most 200 directory entries per tick; no recursive calls or message pumping. }
  for I := 1 to 200 do begin
    if not Searching then begin
      Last := Pending.Count - 1;
      if Last < 0 then begin StopScan(nil); Exit; end;
      CurrentDir := Pending[Last];
      Pending.Delete(Last);
      Scanned := Scanned + 1;
      AddCandidate(CurrentDir);
      Searching := FindFirst(AddBackslash(CurrentDir) + '*', SearchRec);
      HaveFindHandle := Searching;
    end else Searching := FindNext(SearchRec);
    if Searching then begin
      if ((SearchRec.Attributes and FILE_ATTRIBUTE_DIRECTORY) <> 0) and
         ((SearchRec.Attributes and $400) = 0) and
         (SearchRec.Name <> '.') and (SearchRec.Name <> '..') then begin
        Child := AddBackslash(CurrentDir) + SearchRec.Name;
        Pending.Add(Child);
      end;
    end else begin
      { FindClose is safe after a successful FindFirst, even at end of enumeration. }
      if HaveFindHandle then begin FindClose(SearchRec); HaveFindHandle := False; end;
    end;
  end;
  ScanStatus.Caption := CustomMessage('Scanning') + ' ' + IntToStr(Scanned) + #13#10 + CurrentDir;
end;
procedure StartScan(Sender: TObject);
var C: Integer; Root: String;
begin
  Pending.Clear;
  for C := 65 to 90 do begin
    Root := Chr(C) + ':\';
    if DriveType(Root) = 3 then Pending.Add(Root);
  end;
  Scanned := 0;
  Scanning := True;
  Searching := False;
  ScanButton.Enabled := False;
  StopButton.Enabled := True;
  WizardForm.NextButton.Enabled := False;
  WizardForm.BackButton.Enabled := False;
  ScanStatus.Caption := CustomMessage('Scanning');
  ScanTimer := SetTimer(0, 0, 30, CreateCallback(@ScanTick));
  if ScanTimer = 0 then StopScan(nil);
end;
procedure InitializeWizard;
var Root: String; C: Integer;
begin
  WizardReady := True;
  Pending := TStringList.Create;
  DiscoveryPage := CreateCustomPage(wpWelcome, CustomMessage('FindTitle'), CustomMessage('FindDescription'));
  ScanHelp := TNewStaticText.Create(DiscoveryPage);
  ScanHelp.Parent := DiscoveryPage.Surface;
  ScanHelp.SetBounds(0, 0, DiscoveryPage.SurfaceWidth, ScaleY(55));
  ScanHelp.AutoSize := False;
  ScanHelp.WordWrap := True;
  ScanHelp.Caption := CustomMessage('ScanHelp');
  Results := TNewListBox.Create(DiscoveryPage);
  Results.Parent := DiscoveryPage.Surface;
  Results.SetBounds(0, ScaleY(62), DiscoveryPage.SurfaceWidth, ScaleY(100));
  ScanButton := TNewButton.Create(DiscoveryPage);
  ScanButton.Parent := DiscoveryPage.Surface;
  ScanButton.SetBounds(0, ScaleY(172), ScaleX(240), ScaleY(28));
  ScanButton.Caption := CustomMessage('ScanAll');
  ScanButton.OnClick := @StartScan;
  StopButton := TNewButton.Create(DiscoveryPage);
  StopButton.Parent := DiscoveryPage.Surface;
  StopButton.SetBounds(ScaleX(250), ScaleY(172), ScaleX(150), ScaleY(28));
  StopButton.Caption := CustomMessage('StopScan');
  StopButton.Enabled := False;
  StopButton.OnClick := @StopScan;
  ScanStatus := TNewStaticText.Create(DiscoveryPage);
  ScanStatus.Parent := DiscoveryPage.Surface;
  ScanStatus.SetBounds(0, ScaleY(208), DiscoveryPage.SurfaceWidth, ScaleY(60));
  ScanStatus.AutoSize := False;
  ScanStatus.WordWrap := True;
  ScanStatus.Caption := CustomMessage('ScanIdle');
  AddCandidate(ExpandConstant('{param:DIR|}'));
  AddCandidate(ExpandConstant('{autopf}\obs-studio'));
  AddCandidate(ExpandConstant('{localappdata}\Programs\obs-studio'));
  if RegQueryStringValue(HKLM64, 'SOFTWARE\OBS Studio', '', Root) then AddCandidate(Root);
  if RegQueryStringValue(HKCU, 'SOFTWARE\OBS Studio', '', Root) then AddCandidate(Root);
  for C := 65 to 90 do begin
    Root := Chr(C) + ':\';
    if DriveType(Root) = 3 then begin
      AddCandidate(Root + 'obs-studio');
      AddCandidate(Root + 'OBS Studio');
      AddCandidate(Root + 'Program Files\obs-studio');
      AddCandidate(Root + 'SteamLibrary\steamapps\common\OBS Studio');
    end;
  end;
end;
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if CurPageID = DiscoveryPage.ID then begin
    Result := not Scanning;
    if (Results.ItemIndex >= 0) and Result and
      (ExpandConstant('{param:DIR|}') = '') then
      WizardForm.DirEdit.Text := Results.Items[Results.ItemIndex];
  end;
  if (CurPageID = wpSelectDir) and not ValidOBS(WizardDirValue) then begin
    MsgBox(CustomMessage('InvalidOBS'), mbError, MB_OK);
    Result := False;
  end;
end;
function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  Result := '';
  if not ValidOBS(WizardDirValue) then Result := CustomMessage('InvalidOBS')
  else if not FileIsAvailable(AddBackslash(WizardDirValue) + 'bin\64bit\obs.dll') or
    not FileIsAvailable(AddBackslash(WizardDirValue) + 'obs-plugins\64bit\obs-gpu-encoder.dll') or
    not FileIsAvailable(AddBackslash(WizardDirValue) + 'obs-plugins\64bit\obs-gpu-encoder-worker.exe') then
    Result := CustomMessage('CloseOBS');
end;
procedure CancelButtonClick(CurPageID: Integer; var Cancel, Confirm: Boolean);
begin
  if Scanning then StopScan(nil);
end;
procedure DeinitializeSetup;
begin
  if ScanTimer <> 0 then KillTimer(0, ScanTimer);
  if HaveFindHandle then FindClose(SearchRec);
end;

function InitializeUninstall: Boolean;
begin
  Result := FileIsAvailable(ExpandConstant('{app}\obs-plugins\64bit\obs-gpu-encoder.dll')) and
    FileIsAvailable(ExpandConstant('{app}\obs-plugins\64bit\obs-gpu-encoder-worker.exe'));
  if not Result then MsgBox(CustomMessage('CloseOBS'), mbError, MB_OK);
end;
