# -*- mode: python ; coding: utf-8 -*-
"""PyInstaller spec for Phonk Drum Studio desktop app."""

import os
import platform
from pathlib import Path

USE_UPX = os.environ.get("GITHUB_ACTIONS") != "true"

block_cipher = None

ROOT = Path(SPECPATH)
REPO = ROOT.parent.parent
SF = REPO / "disklordz" / "sound-factory"
STUB = REPO / "disklordz" / "website" / "public" / "samples"

datas = []
binaries = []

if (SF / "kit_from_samples.py").is_file():
    datas.append((str(SF / "kit_from_samples.py"), "sound-factory"))

try:
    import customtkinter

    assets = Path(customtkinter.__file__).resolve().parent / "assets"
    if assets.is_dir():
        datas.append((str(assets), "customtkinter/assets"))
except ImportError:
    pass

if STUB.is_dir():
    datas.append((str(STUB), "factory-stubs/samples"))

hiddenimports = [
    "torch",
    "torchaudio",
    "soundfile",
    "sounddevice",
    "customtkinter",
    "numpy",
    "_soundfile_data",
    "phonk_drum_studio",
    "phonk_drum_studio.app",
    "phonk_drum_studio.model",
    "phonk_drum_studio.dataset",
    "phonk_drum_studio.trainer",
    "phonk_drum_studio.paths",
]

a = Analysis(
    [str(ROOT / "run_desktop.py")],
    pathex=[str(ROOT)],
    binaries=binaries,
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name="PhonkDrumStudio",
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=USE_UPX,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=platform.system() == "Darwin",
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
