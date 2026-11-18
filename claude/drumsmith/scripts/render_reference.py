"""Generate deterministic, original, experimental PCM16 one shots (not production DSP)."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import random
import struct
import wave


def render(kind, rate, seed):
    rng = random.Random(seed)
    dur = 0.9 if kind == '808' else 0.5 if kind == 'FX' else 0.35 if kind != 'COWBELL' else 0.25
    samples = []
    phase = 0.0
    for i in range(int(dur * rate)):
        t = i / rate
        if kind in ('808', 'KICK'):
            base = 52 if kind == '808' else 70
            f = base + 95 * math.exp(-t / 0.018)
            phase += 2 * math.pi * f / rate
            tail = 0.48 if kind == '808' else 0.11
            env = (1-math.exp(-t/0.001)) * math.exp(-t/tail)
            value = math.tanh(1.7*math.sin(phase)*env) * 0.55
        elif kind == 'COWBELL':
            phase += 2 * math.pi * 540 / rate
            # Band-limited-ish reference sine partials, not a faithful 808 emulation.
            value = (math.sin(phase) + 0.47*math.sin(phase*800/540))*math.exp(-t/0.07)*0.36
        else:
            body = math.sin(2*math.pi*190*t)*math.exp(-t/0.03)
            noise = (rng.random()*2-1)*math.exp(-t/(0.06 if kind == 'HAT' else 0.12))
            if kind == 'CLAP':
                bursts = sum(math.exp(-max(0,t-offset)/0.016) * (t>=offset) for offset in (0,0.012,0.027))
                value = noise * bursts * 0.31
            elif kind == 'FX':
                value = (0.22*body + 0.35*noise)*math.sin(2*math.pi*(18*t + 110*t*t))
            else:
                value = (0.20*body + 0.42*noise)
        samples.append(max(-0.95, min(0.95, value)))
    return samples


def write_wav(path, samples, rate):
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), 'wb') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(struct.pack('<' + 'h'*len(samples), *(round(x*32767) for x in samples)))


def render_kit(kit):
    manifest_path = kit / 'manifest.json'
    manifest = json.loads(manifest_path.read_text(encoding='utf8'))
    # Validate all output paths before writing anything (protect against crafted manifests).
    from verify_kit import safe_relative
    for entry in manifest['samples']:
        if not safe_relative(entry.get('path')) or entry['path'].split('/')[0] != 'masters':
            raise ValueError('unsafe output path')
        path = (kit / entry['path']).resolve()
        if not path.is_relative_to(kit.resolve()) or path.exists():
            raise FileExistsError(f'unsafe or already existing output: {path}')
    for entry in manifest['samples']:
        if entry.get('recipe') != 'reference-v1':
            raise ValueError('only reference-v1 recipe supported')
        path = kit / entry['path']
        if path.exists():
            raise FileExistsError(f'refusing overwrite: {path}')
        write_wav(path, render(entry['kind'], entry['sample_rate'], entry['seed']), entry['sample_rate'])
        entry['sha256'] = hashlib.sha256(path.read_bytes()).hexdigest()
    manifest['status'] = 'REFERENCE_RENDERED_NOT_RELEASED'
    manifest_path.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf8')
    return len(manifest['samples'])


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--kit', type=Path, required=True)
    args = p.parse_args()
    print(f'Rendered {render_kit(args.kit)} experimental reference WAV files')
