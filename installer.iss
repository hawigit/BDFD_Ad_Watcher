#define MyAppName "BDFD Ad Watcher"
#define MyAppVersion "5.0"
#define MyAppPublisher "phivogit"
#define MyAppURL "https://github.com/phivogit/BDFD_Ad_Watcher"
#define MyAppExeName "appBDFDAdW.exe"

[Setup]
AppId={{C6A4F491-03D8-4C8E-98DF-BCBE970F3FA2}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=Release Exe
OutputBaseFilename=BDFD_Ad_Watcher_v5.0_Setup
SetupIconFile=Deploy Folder\BDFDADW_Icon.ico
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "Deploy Folder\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
