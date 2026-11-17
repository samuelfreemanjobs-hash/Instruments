"""Create a minimal kit manifest and stable, safe filenames; no audio is rendered."""
import argparse
import json
from pathlib import Path


TAIL_LIGHT_STATIC_PLAN = [("808",8),("KICK",8),("SNARE",8),("CLAP",6),
                          ("HAT",12),("COWBELL",8),("PERC",8),("FX",6)]

# Converter Night (CNV) — 512-hit SKU (drums @ 3× guide + full useful FX set).
# RATE/EDGE/ROMP hero weight; FX covers crash/alias/reverse/stab/texture + loop slots.
CONVERTER_NIGHT_PLAN = [
    ("808", 60),   # pitched subs (RATE) — hero
    ("KICK", 48),  # BIT12, ANLG, ROMP, SIMM, 707-punch
    ("SNARE", 54), # ROMP, RX, CAVE, BIT12
    ("CLAP", 24),  # CAVE / metallic
    ("HAT", 66),   # 42 closed + 24 open (EDGE / ANLG / ROMP)
    ("COWBELL", 15),
    ("PERC", 15),  # Latin capped (with cowbell = 30)
    ("FX", 230),   # crash40 + alias48 + reverse40 + stab40 + tex26 + loop slots36
]

PLANS = {
    'tail-light-static': TAIL_LIGHT_STATIC_PLAN,
    'converter-night': CONVERTER_NIGHT_PLAN,
}


def scaffold(out, sku, count, plan=None):
    if not sku.isascii() or not sku.isalnum() or not 2 <= len(sku) <= 16:
        raise ValueError('sku must be 2-16 ASCII alphanumerics')
    if plan is not None and plan not in PLANS:
        raise ValueError('unknown plan')
    if plan is not None:
        count = sum(n for _, n in PLANS[plan])
    if not 1 <= count <= 512:
        raise ValueError('count must be 1-512')
    out = Path(out)
    out.mkdir(parents=True, exist_ok=True)
    (out / 'masters').mkdir(exist_ok=True)
    if (out / 'manifest.json').exists():
        raise FileExistsError('manifest already exists; refusing overwrite')
    types = ['808', 'KICK', 'SNARE', 'CLAP', 'HAT', 'COWBELL', 'PERC', 'FX']
    entries = []
    planned_types = (
        [kind for kind, n in PLANS[plan] for _ in range(n)]
        if plan is not None
        else [types[i % len(types)] for i in range(count)]
    )
    for i, kind in enumerate(planned_types):
        filename = f'{sku}_{kind}_ORIGINAL_NA_{i+1:03d}.wav'
        entries.append({'id': f'{sku}-{i+1:03d}', 'path': f'masters/{filename}',
                        'kind': kind, 'recipe': 'reference-v1', 'seed': 1000+i,
                        'sample_rate': 44100, 'channels': 1, 'bit_depth': 16,
                        'duration_min_sec': 0.05, 'duration_max_sec': 2.0,
                        'peak_ceiling_dbfs': -0.1, 'provenance': 'original_synthesis',
                        'sha256': None, 'pitch_target_hz': 52.0 if kind == '808' else None})
    manifest = {'schema_version': 1, 'sku': sku, 'status': 'DRAFT',
                'device_profiles': {'generic_wav_pcm16': 'INTERNAL', 'hardware_targets': 'UNVERIFIED'},
                'human_listening_approved': False, 'release_approved': False, 'approvals': {'listening': None, 'release': None},
                'required_device_profiles': [],
                'samples': entries}
    (out / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf8')
    return manifest


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--out', required=True)
    p.add_argument('--sku', default='TLS')
    p.add_argument('--count', type=int, default=3)
    p.add_argument('--plan', choices=sorted(PLANS))
    args = p.parse_args()
    print(f"Scaffolded {len(scaffold(args.out, args.sku, args.count, args.plan)['samples'])} expected files; audio not rendered")
