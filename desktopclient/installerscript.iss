#define MyAppName "KeyWizard"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "Nour Haj Ramadan"
#define MyAppExeName "KeyWizard.exe"

#define ArduinoCliUrl "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip"

[Setup]

AppId={{B2C5D8F4-5F4F-4D8D-ABCD-123456789ABC}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}

DefaultDirName={userappdata}\Programs\{#MyAppName}
DefaultGroupName={#MyAppName}

PrivilegesRequired=lowest

OutputDir=Output
OutputBaseFilename=KeyWizard_{#MyAppVersion}_Installer

Compression=lzma2
SolidCompression=yes

WizardStyle=modern

UninstallDisplayIcon={app}\{#MyAppExeName}

SetupIconFile="C:\Users\moham\eclipse-workspace_2025\build-KeyWizard-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\release\kwicon.ico"

DisableProgramGroupPage=yes

[Languages]

Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]

Name: "desktopicon"; Description: "Create a desktop shortcut"; Flags: unchecked

[Files]

Source: "C:\Users\moham\eclipse-workspace_2025\build-KeyWizard-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]

Name: "{group}\KeyWizard"; Filename: "{app}\{#MyAppExeName}"

Name: "{autodesktop}\KeyWizard"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

Name: "{group}\Uninstall KeyWizard"; Filename: "{uninstallexe}"

[Run]

Filename: "{app}\{#MyAppExeName}"; Description: "Launch KeyWizard"; Flags: nowait postinstall skipifsilent

[Code]

function DownloadArduinoCLI(): Boolean;
var
  ZipPath: string;
  ExtractPath: string;
  ArduinoPath: string;
  ResultCode: Integer;
  PowerShellCommand: string;
begin
  Result := False;

  WizardForm.StatusLabel.Caption := 'Downloading Arduino CLI...';

  ZipPath := ExpandConstant('{tmp}\arduino-cli.zip');
  ExtractPath := ExpandConstant('{tmp}\arduino-cli-extracted');
  ArduinoPath := ExpandConstant('{app}\arduino-cli.exe');

  DeleteFile(ZipPath);
  DelTree(ExtractPath, True, True, True);

  PowerShellCommand :=
    '-NoProfile -ExecutionPolicy Bypass -Command ' +
    '"$ProgressPreference = ''SilentlyContinue''; ' +
    'Invoke-WebRequest -Uri ''' + '{#ArduinoCliUrl}' + ''' ' +
    '-OutFile ''' + ZipPath + '''; ' +
    'Expand-Archive -Path ''' + ZipPath + ''' ' +
    '-DestinationPath ''' + ExtractPath + ''' -Force"';

  if not Exec(
    'powershell.exe',
    PowerShellCommand,
    '',
    SW_HIDE,
    ewWaitUntilTerminated,
    ResultCode
  ) then
  begin
    MsgBox(
      'Could not start PowerShell to download Arduino CLI.',
      mbError,
      MB_OK
    );
    Exit;
  end;

  if ResultCode <> 0 then
  begin
    MsgBox(
      'Arduino CLI could not be downloaded.' + #13#10 +
      'Please check your internet connection and try again.',
      mbError,
      MB_OK
    );
    Exit;
  end;

  WizardForm.StatusLabel.Caption := 'Installing Arduino CLI...';


  if FileExists(AddBackslash(ExtractPath) + 'arduino-cli.exe') then
  begin
    if FileCopy(
      AddBackslash(ExtractPath) + 'arduino-cli.exe',
      ArduinoPath,
      False
    ) then
    begin
      Result := True;
      Exit;
    end;
  end;


  MsgBox(
    'Arduino CLI was downloaded, but arduino-cli.exe could not be found inside the archive.',
    mbError,
    MB_OK
  );
end;


function PrepareToInstall(var NeedsRestart: Boolean): string;
begin
  Result := '';

  if not FileExists(
    ExpandConstant('{app}\arduino-cli.exe')
  ) then
  begin
    if not DownloadArduinoCLI() then
    begin
      Result :=
        'Arduino CLI could not be installed.' + #13#10 +
        'Installation cannot continue.';
      Exit;
    end;
  end;
end;