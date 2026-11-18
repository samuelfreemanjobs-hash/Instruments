"""Dependency-free WAV QA with optional ffmpeg EBU R128 metering. Exit 0 only on technical PASS."""
import argparse
import hashlib
import json
import math
from pathlib import Path, PurePosixPath
import re
import shutil
import struct
import subprocess
import sys
import wave

from dsp_math import dbfs, rms
from pitch_check import inspect_pitch


def safe_relative(raw):
    if not isinstance(raw, str) or not raw or '\\' in raw or raw.startswith('/'):
        return False
    p = PurePosixPath(raw)
    return not any(part in ('.', '..') for part in raw.split('/')) and len(p.parts) > 1 and p.suffix.lower() == '.wav'


def ffmpeg_ebur128(path):
    if not shutil.which('ffmpeg'):
        return {'status': 'UNAVAILABLE', 'reason': 'ffmpeg not on PATH'}
    cmd = ['ffmpeg', '-hide_banner', '-nostdin', '-i', str(path), '-filter_complex',
           'ebur128=peak=true', '-f', 'null', '-']
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=35)
    except (OSError, subprocess.TimeoutExpired) as exc:
        return {'status': 'ERROR', 'reason': str(exc)}
    output = result.stderr
    # Read final summary only; per-window lines may contain multiple transient values.
    summary = output.rsplit('Summary:', 1)[-1] if 'Summary:' in output else ''
    integrated = re.search(r'Integrated loudness:\s*I:\s*(-?[\d.]+) LUFS', summary)
    truepeak = re.search(r'True peak:\s*Peak:\s*(-?[\d.]+) dBFS', summary)
    if result.returncode != 0 or not integrated or not truepeak:
        return {'status': 'ERROR', 'reason': 'ffmpeg did not return complete EBU R128 summary'}
    return {'status': 'MEASURED_FFMPEG_EBUR128', 'integrated_lufs': float(integrated.group(1)),
            'true_peak_dbtp': float(truepeak.group(1)), 'note': 'Short percussive clips may not have meaningful integrated LUFS'}


def measure(path):
    with wave.open(str(path), 'rb') as w:
        channels, width, rate = w.getnchannels(), w.getsampwidth(), w.getframerate()
        frames, comp = w.getnframes(), w.getcomptype()
        if comp != 'NONE' or width != 2:
            raise ValueError('only uncompressed PCM16 WAV supported by this meter')
        if frames <= 0 or channels <= 0 or rate <= 0:
            raise ValueError('invalid WAV dimensions')
        raw = w.readframes(frames)
    if len(raw) != frames*channels*width:
        raise ValueError('truncated sample data')
    count = frames * channels
    peak, energy, fullscale, nonzero = 0, 0, 0, 0
    for (v,) in struct.iter_unpack('<h', raw):
        mag = abs(v)
        peak = max(peak, mag)
        energy += (v / 32768.0)**2
        fullscale += (v in (-32768, 32767))
        nonzero += (v != 0)
    return {'sample_rate': rate, 'channels': channels, 'bit_depth': width*8,
            'frames': frames, 'duration_sec': round(frames/rate, 6),
            'sample_peak_dbfs': round(dbfs(peak/32768), 4),
            'rms_dbfs': round(dbfs(math.sqrt(energy/count)), 4),
            'clipped_fraction': fullscale/count, 'nonzero_fraction': nonzero/count}


def verify(kit, manifest_path, use_ffmpeg=False, check_pitch=True):
    report = {'status': 'FAIL', 'technical_status': 'FAIL', 'errors': [], 'warnings': [], 'files': [],
              'release_ready': False, 'qa_scope': 'technical PCM WAV checks only'}
    try:
        kit = kit.resolve(strict=True)
        manifest = json.loads(manifest_path.read_text(encoding='utf8'))
        if manifest.get('schema_version') != 1 or not isinstance(manifest.get('samples'), list) or not manifest['samples']:
            raise ValueError('invalid or empty manifest')
    except (OSError, ValueError, KeyError) as exc:
        report['errors'].append(f'Cannot load kit/manifest: {exc}')
        return report
    expected = set()
    seen_ids = set()
    for entry in manifest['samples']:
        if not isinstance(entry, dict):
            report['errors'].append('invalid sample entry'); continue
        rel = entry.get('path')
        sample_id = entry.get('id')
        if not safe_relative(rel) or rel in expected or not sample_id or sample_id in seen_ids:
            report['errors'].append(f'bad/duplicate path or ID: {rel}'); continue
        expected.add(rel)
        seen_ids.add(sample_id)
        path = (kit / rel).resolve()
        if not path.is_relative_to(kit) or not path.is_file():
            report['errors'].append(f'missing or outside kit: {rel}'); continue
        row = {'id': sample_id, 'path': rel, 'errors': []}
        try:
            m = measure(path)
            row.update(m)
            if not entry.get('provenance'):
                row['errors'].append('missing provenance')
            if entry.get('recipe') == 'parametric-v2':
                from recipe_engine import validate_recipe, recipe_hash
                try:
                    validate_recipe(entry.get('recipe_params'))
                    if entry.get('recipe_sha256') != recipe_hash(entry['recipe_params']):
                        row['errors'].append('recipe provenance hash mismatch')
                except (ValueError, TypeError, KeyError):
                    row['errors'].append('invalid recipe parameters')
            for field in ('sample_rate', 'channels', 'bit_depth'):
                if field not in entry or m[field] != entry[field]:
                    row['errors'].append(f'{field} mismatch')
            if not (entry.get('duration_min_sec', 0) <= m['duration_sec'] <= entry.get('duration_max_sec', math.inf)):
                row['errors'].append('duration out of bounds')
            if m['sample_peak_dbfs'] > entry.get('peak_ceiling_dbfs', -0.1):
                row['errors'].append('sample peak exceeds ceiling')
            if m['clipped_fraction'] > 0:
                row['errors'].append('PCM fullscale sample detected')
            if m['nonzero_fraction'] < 0.02:
                row['errors'].append('mostly silent/empty waveform')
            actual_hash = hashlib.sha256(path.read_bytes()).hexdigest()
            if not entry.get('sha256') or entry['sha256'] != actual_hash:
                row['errors'].append('missing or mismatched SHA256')
            row['sha256'] = actual_hash
            if check_pitch and entry.get('pitch_target_hz') is not None:
                row['pitch_check'] = inspect_pitch(path, entry['pitch_target_hz'])
                if row['pitch_check']['status'] == 'FAIL':
                    row['errors'].append('pitch target mismatch')
                elif row['pitch_check']['status'] == 'INCONCLUSIVE':
                    report['warnings'].append(f'{rel}: pitch check inconclusive')
            if use_ffmpeg:
                row['ebur128'] = ffmpeg_ebur128(path)
                if row['ebur128']['status'] != 'MEASURED_FFMPEG_EBUR128':
                    report['warnings'].append(f'{rel}: optional ffmpeg measurement unavailable')
        except (OSError, ValueError, EOFError, struct.error, wave.Error) as exc:
            row['errors'].append(f'Invalid WAV: {exc}')
        for err in row['errors']:
            report['errors'].append(f'{rel}: {err}')
        report['files'].append(row)
    actual = {str(p.relative_to(kit).as_posix()) for p in kit.rglob('*.wav') if p.is_file()}
    for rel in sorted(actual-expected):
        report['errors'].append(f'unlisted WAV: {rel}')
    approvals = manifest.get('approvals') or {}
    def signed(kind):
        approval = approvals.get(kind)
        return (isinstance(approval,dict) and isinstance(approval.get('reviewer'),str)
                and bool(approval['reviewer'].strip()) and isinstance(approval.get('date'),str)
                and bool(re.fullmatch(r'\d{4}-\d{2}-\d{2}', approval['date']))
                and isinstance(approval.get('evidence'),str) and bool(approval['evidence'].strip()))
    if not manifest.get('human_listening_approved') or not signed('listening'):
        report['warnings'].append('HUMAN GATE: listening not approved')
    if not manifest.get('release_approved') or not signed('release'):
        report['warnings'].append('HUMAN GATE: release not approved')
    if any(v == 'UNVERIFIED' for v in manifest.get('device_profiles', {}).values()):
        report['warnings'].append('device compatibility unverified')
    required_profiles = manifest.get('required_device_profiles', [])
    if not isinstance(required_profiles, list) or not all(isinstance(x,str) and x for x in required_profiles):
        report['errors'].append('invalid required_device_profiles')
        required_profiles = []
    for target in required_profiles:
        if manifest.get('device_profiles',{}).get(target) != 'TESTED':
            report['warnings'].append(f'DEVICE GATE: {target} lacks TESTED proof')
    report['technical_status'] = 'FAIL' if report['errors'] else 'PASS'
    report['status'] = report['technical_status']
    report['release_ready'] = bool(not report['errors'] and manifest.get('human_listening_approved') and signed('listening')
                                   and manifest.get('release_approved') and signed('release')
                                   and all(manifest.get('device_profiles',{}).get(x) == 'TESTED' for x in required_profiles))
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--kit', type=Path, required=True)
    p.add_argument('--manifest', type=Path, required=True)
    p.add_argument('--json', type=Path)
    p.add_argument('--ffmpeg', action='store_true')
    p.add_argument('--skip-pitch', action='store_true', help='Reference only; skip pitch gate')
    args = p.parse_args()
    result = verify(args.kit, args.manifest, args.ffmpeg, not args.skip_pitch)
    encoded = json.dumps(result, indent=2, allow_nan=False) + '\n'
    if args.json:
        args.json.write_text(encoded, encoding='utf8')
    print(encoded)
    sys.exit(0 if result['technical_status'] == 'PASS' else 1)
