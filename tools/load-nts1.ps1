# Load the built Nu:Tekt NTS-1 oscillators onto a connected NTS-1.
# The minilogue xd Sound Librarian cannot send these files.
#
#   powershell -ExecutionPolicy Bypass -File tools\load-nts1.ps1
#
# Plug the NTS-1 in with USB before you run it. The script repacks each
# oscillator as a real zip, then sends them with Korg logue-cli.
# Add -CheckOnly to repack and validate without sending.

param(
  [switch]$CheckOnly
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$Cli = Join-Path $Root "tools\logue-cli\logue-cli-win64-0.07-2b\logue-cli.exe"
$Pack = Join-Path $Root "tools\logue\pack-unit.ps1"
$Ready = Join-Path $Root "dist\load-ready"
if (-not (Test-Path $Cli)) {
  throw "logue-cli is missing at $Cli"
}

$units = @(
  @{ Dir = "tr808-kick";      Project = "nts1_tr808_kick";      Slot = 0;  Sound = "Phonk 808 kick" }
  @{ Dir = "west-coast-moog"; Project = "nts1_west_coast_moog"; Slot = 1;  Sound = "P-Funk / West Coast" }
  @{ Dir = "juno-rnb";        Project = "nts1_juno_rnb";        Slot = 2;  Sound = "Juno 80s R&B" }
  @{ Dir = "dilla-bass";      Project = "nts1_dilla_bass";      Slot = 3;  Sound = "Dilla hip-hop bass" }
  @{ Dir = "sub-phatty";      Project = "nts1_sub_phatty";      Slot = 4;  Sound = "Sub Phatty" }
  @{ Dir = "prophet-funk";    Project = "nts1_prophet_funk";    Slot = 5;  Sound = "Prophet funk / R&B" }
  @{ Dir = "dx7-lately";      Project = "nts1_dx7_lately";      Slot = 6;  Sound = "DX7 Lately Bass" }
  @{ Dir = "sh101-babyface";  Project = "nts1_sh101_babyface";  Slot = 7;  Sound = "SH-101 pulse bass" }
  @{ Dir = "moog-voyager-se"; Project = "nts1_voyager_se";      Slot = 8;  Sound = "Voyager / SE squelch" }
  @{ Dir = "cardo-gfunk";     Project = "nts1_cardo_gfunk";     Slot = 9;  Sound = "Cardo G-funk" }
  @{ Dir = "larry-june";      Project = "nts1_larry_june";      Slot = 10; Sound = "Larry June cruise" }
  @{ Dir = "khalifa-haze";    Project = "nts1_khalifa_haze";    Slot = 11; Sound = "Hazy stoner sub" }
  @{ Dir = "memphis-juicy";   Project = "nts1_memphis_juicy";   Slot = 12; Sound = "Three 6 / Memphis trunk" }
  @{ Dir = "ensoniq-eps1";    Project = "nts1_ensoniq_eps1";    Slot = 13; Sound = "Ensoniq EPS Memphis" }
)

New-Item -ItemType Directory -Force -Path $Ready | Out-Null
$lines = @()
$lines += "Nu:Tekt NTS-1 digital kit — your oscillators"
$lines += ""
$lines += "These are .ntkdigunit files for the original NTS-1, not the mkII."
$lines += "The minilogue xd Sound Librarian will not load them."
$lines += "Only one oscillator plays at a time. Pick it on the panel after the send."
$lines += ""
$lines += "Slot  File  Sound"

foreach ($u in $units) {
  $src = Join-Path $Root "src\oscillators\$($u.Dir)"
  $build = Join-Path $src "build"
  if (-not (Test-Path (Join-Path $build "$($u.Project).bin"))) {
    throw "Missing build payload for $($u.Project). Build that oscillator first."
  }
  & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $Pack `
    -Project $u.Project -BuildDir $build -Manifest (Join-Path $src "manifest.json") -OutDir $src
  $pkg = Join-Path $src "$($u.Project).ntkdigunit"
  $magic = [System.IO.File]::ReadAllBytes($pkg)[0..1]
  if ($magic[0] -ne 0x50 -or $magic[1] -ne 0x4B) {
    throw "$pkg is not a zip archive"
  }
  & $Cli check -u $pkg
  if ($LASTEXITCODE -ne 0) {
    throw "logue-cli rejected $pkg"
  }
  Copy-Item $pkg (Join-Path $Ready "$($u.Project).ntkdigunit") -Force
  Copy-Item $pkg (Join-Path $Root "dist\$($u.Project).ntkdigunit") -Force
  $lines += ("{0,4}  {1}  {2}" -f $u.Slot, "$($u.Project).ntkdigunit", $u.Sound)
  Write-Host ("ready slot {0}: {1}" -f $u.Slot, $u.Sound)
}

$lines += ""
$lines += "On the NTS-1: USB connected, power on."
$lines += "Then from this repo run:"
$lines += "  powershell -ExecutionPolicy Bypass -File tools\load-nts1.ps1"
$lines += "Turn the oscillator TYPE control to the user slot you want."
$how = Join-Path $Ready "LOAD.txt"
Set-Content -Path $how -Value ($lines -join "`r`n") -Encoding utf8

if ($CheckOnly) {
  Write-Host "Checked 14 oscillators. Files are in $Ready"
  exit 0
}

$probe = & $Cli probe -l 2>&1 | Out-String
Write-Host $probe
if ($probe -notmatch "NTS-1|nts-1|Nu:Tekt|nutekt") {
  Write-Host ""
  Write-Host "NTS-1 is not on USB yet. Plug it in, wait until Windows sees it, then run this script again."
  Write-Host "Load folder: $Ready"
  exit 2
}

foreach ($u in $units) {
  $pkg = Join-Path $Ready "$($u.Project).ntkdigunit"
  Write-Host ("sending slot {0} {1}" -f $u.Slot, $u.Sound)
  & $Cli load -u $pkg -s $u.Slot
  if ($LASTEXITCODE -ne 0) {
    throw "Send failed for $($u.Project) (slot $($u.Slot))"
  }
}

Write-Host "Sent 14 oscillators. On the NTS-1, choose the user oscillator slot, then play."
