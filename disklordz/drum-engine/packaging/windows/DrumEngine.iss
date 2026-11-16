; Inno Setup script for a later Windows installer. ISCC is not run on the Linux agent.
; Point Source at a VST3 you have already built. Do not hard-code a machine-specific sample library.

#define MyAppName "Disklordz Drum Engine"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "Disklordz"

[Setup]
AppId={{8F3E0A2C-6B14-4E5A-9C11-7D2B4E8A1C60}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\{#MyAppPublisher}\{#MyAppName}
OutputBaseFilename=DisklordzDrumEngine-{#MyAppVersion}-win64
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
Compression=lzma2
SolidCompression=yes
DisableProgramGroupPage=yes

[Files]
; Replace the source path when the VST3 artefact exists.
; Source: "..\..\build\DrumBatchExporter.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\DrumBatchExporter.exe"
