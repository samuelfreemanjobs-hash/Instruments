# Stage the 14 NTS-1 mkII oscillators and, when the mkII is on USB, send them.
# The original .ntkdigunit files and the minilogue xd Sound Librarian do not load on mkII.
#
#   powershell -ExecutionPolicy Bypass -File tools\load-nts1-mkii.ps1
#
# Firmware on the unit must be 1.0.0 or newer (System / About in KORG KONTROL Editor).
# Add -CheckOnly to copy and list the files without sending.

param(
  [switch]$CheckOnly
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$Src = "C:\audiodev\nts1-mkii-build\src\mkii\oscillators"
$Ready = Join-Path $Root "dist\load-ready-mkii"
$Py = Join-Path $Root "tools\py-logue-tools\main.py"

$units = @(
  @{ File = "tr808_kick_phonk.nts1mkiiunit";            Slot = 0;  Sound = "Phonk 808 kick" }
  @{ File = "west_coast_moog_funk.nts1mkiiunit";        Slot = 1;  Sound = "P-Funk / West Coast" }
  @{ File = "juno_106_rnb_bass.nts1mkiiunit";           Slot = 2;  Sound = "Juno 80s R&B" }
  @{ File = "dilla_type_bass.nts1mkiiunit";             Slot = 3;  Sound = "Dilla hip-hop bass" }
  @{ File = "moog_sub_phatty_bass.nts1mkiiunit";        Slot = 4;  Sound = "Sub Phatty" }
  @{ File = "prophet5_funk_rnb_bass.nts1mkiiunit";      Slot = 5;  Sound = "Prophet funk / R&B" }
  @{ File = "dx7_lately_bass.nts1mkiiunit";             Slot = 6;  Sound = "DX7 Lately Bass" }
  @{ File = "sh101_babyface_bass.nts1mkiiunit";         Slot = 7;  Sound = "SH-101 pulse bass" }
  @{ File = "moog_voyager_se1_squelch.nts1mkiiunit";    Slot = 8;  Sound = "Voyager / SE squelch" }
  @{ File = "cardo_got_wings_gfunk.nts1mkiiunit";       Slot = 9;  Sound = "Cardo G-funk" }
  @{ File = "larry_june_bay_cruise_bass.nts1mkiiunit";  Slot = 10; Sound = "Larry June cruise" }
  @{ File = "khalifa_haze_stoner_bass.nts1mkiiunit";    Slot = 11; Sound = "Hazy stoner sub" }
  @{ File = "juicy_j_three6_memphis_bass.nts1mkiiunit"; Slot = 12; Sound = "Three 6 / Memphis trunk" }
  @{ File = "ensoniq_eps1_memphis_bass.nts1mkiiunit";   Slot = 13; Sound = "Ensoniq EPS Memphis" }
)

New-Item -ItemType Directory -Force -Path $Ready | Out-Null
$lines = @(
  "NTS-1 mkII — your 14 oscillators",
  "",
  "Load these .nts1mkiiunit files. Do not load the .ntkdigunit folder.",
  "Firmware must be 1.0.0 or newer.",
  "Only one oscillator plays at a time. Pick the user slot on the panel after the send.",
  "",
  "Official loader: KORG KONTROL Editor",
  "https://www.korg.com/us/support/download/software/0/933/1354/",
  "Open NTS-1 mkII, drag a file from this folder onto an oscillator slot, press SEND.",
  "",
  "Slot  File  Sound"
)

foreach ($u in $units) {
  $slug = [System.IO.Path]::GetFileNameWithoutExtension($u.File)
  $pkg = Join-Path $Src "$slug\$($u.File)"
  if (-not (Test-Path $pkg)) {
    throw "Missing mkII unit: $pkg"
  }
  $magic = [System.IO.File]::ReadAllBytes($pkg)[0..3]
  if ($magic[0] -ne 0x7F -or $magic[1] -ne 0x45 -or $magic[2] -ne 0x4C -or $magic[3] -ne 0x46) {
    throw "$pkg is not an ARM unit"
  }
  Copy-Item $pkg (Join-Path $Ready $u.File) -Force
  $lines += ("{0,4}  {1}  {2}" -f $u.Slot, $u.File, $u.Sound)
  Write-Host ("ready slot {0}: {1}" -f $u.Slot, $u.Sound)
}

$lines += ""
$lines += "Or, with the mkII plugged in:"
$lines += "  powershell -ExecutionPolicy Bypass -File tools\load-nts1-mkii.ps1"
Set-Content -Path (Join-Path $Ready "LOAD.txt") -Value ($lines -join "`r`n") -Encoding utf8

if ($CheckOnly -or -not (Test-Path $Py)) {
  if (-not $CheckOnly -and -not (Test-Path $Py)) {
    Write-Host ""
    Write-Host "Files are in $Ready"
    Write-Host "KORG KONTROL Editor is the loader. Drag a .nts1mkiiunit onto an oscillator slot and press SEND."
  } else {
    Write-Host "Checked 14 mkII oscillators. Files are in $Ready"
  }
  exit 0
}

Write-Host "py-logue-tools is present. Listing MIDI ports..."
& python $Py -l
if ($LASTEXITCODE -ne 0) { throw "Could not list MIDI ports" }
Write-Host "Plug in the NTS-1 mkII and pass the port number printed above if send is not automatic."
Write-Host "This script copies the files. Use KONTROL Editor SEND, or py-logue-tools install -s <slot>, to write them."
exit 0
