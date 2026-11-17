"""v1.3 genre kit planner. Safe writes; v1.2 manifest/engine compatible."""
import argparse
import json
from pathlib import Path
from scaffold_kit import scaffold
from genre_profiles import genre_recipe, PROFILES
from recipe_engine import recipe_hash

def build(out, genre, sku='DKL', count=64, plan=None):
    if genre not in PROFILES: raise ValueError('unknown genre')
    if count not in range(1,129): raise ValueError('count must be 1..128')
    if plan not in (None, 'tail-light-static', 'converter-night'): raise ValueError('unknown plan')
    manifest = scaffold(out, sku, count, plan)
    variants = {}
    for entry in manifest['samples']:
        kind = entry['kind']
        n = variants.get(kind, 0)
        variants[kind] = n + 1
        recipe = genre_recipe(kind,n,genre)
        entry.update(recipe='parametric-v2', recipe_params=recipe,
                     recipe_sha256=recipe_hash(recipe),
                     pitch_target_hz=recipe['pitch_hz'] if kind=='808' else None,
                     duration_min_sec=recipe['duration_sec']-.02,
                     duration_max_sec=recipe['duration_sec']+.02)
    manifest.update(status='RECIPE_DRAFT', genre=genre,
                    genre_description=PROFILES[genre]['description'],
                    generator='drumsmith-genre-v1.3',
                    production_note='Reference-grade offline original synthesis; listening approval required')
    (Path(out)/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf8')
    return manifest

if __name__=='__main__':
    p=argparse.ArgumentParser(description='Create original genre recipe kit (no rendering)')
    p.add_argument('--out', required=True);p.add_argument('--genre', choices=sorted(PROFILES), required=True)
    p.add_argument('--sku',default='DKL');p.add_argument('--count',type=int,default=64)
    p.add_argument('--plan', choices=['tail-light-static', 'converter-night'])
    a=p.parse_args();m=build(a.out,a.genre,a.sku,a.count,a.plan)
    print('Planned',len(m['samples']),'samples for',a.genre)
