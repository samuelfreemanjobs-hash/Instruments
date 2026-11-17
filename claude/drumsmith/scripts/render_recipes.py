"""Safe all-or-nothing preflight for offline recipe renders (does not overwrite)."""
import argparse
import hashlib
import json
from pathlib import Path
from recipe_engine import render_recipe, validate_recipe, recipe_hash
from render_reference import write_wav
from verify_kit import safe_relative

def render_kit(kit):
    kit=Path(kit).resolve(strict=True)
    mp=kit/'manifest.json'
    manifest=json.loads(mp.read_text(encoding='utf8'))
    samples=manifest.get('samples')
    if not isinstance(samples,list) or not samples: raise ValueError('empty manifest')
    seen=set()
    for e in samples:
        rel=e.get('path')
        if not safe_relative(rel) or not rel.startswith('masters/') or rel in seen: raise ValueError('unsafe or duplicate output path')
        seen.add(rel)
        dest=(kit/rel).resolve()
        if not dest.is_relative_to(kit) or dest.exists(): raise FileExistsError('existing/unsafe target '+str(dest))
        if e.get('recipe')!='parametric-v2': raise ValueError('recipe not v2')
        validate_recipe(e.get('recipe_params'))
        if recipe_hash(e['recipe_params'])!=e.get('recipe_sha256'): raise ValueError('recipe hash mismatch')
        if e.get('sample_rate') not in (44100,48000): raise ValueError('unsupported sample rate')
        if e.get('channels')!=1 or e.get('bit_depth')!=16: raise ValueError('unsupported output format')
    # Precompute all audio before writing; no partial output due to bad recipes.
    rendered=[render_recipe(e['recipe_params'],e['sample_rate'],e['seed']) for e in samples]
    for e,audio in zip(samples,rendered):
        dest=kit/e['path'];write_wav(dest,audio,e['sample_rate']);e['sha256']=hashlib.sha256(dest.read_bytes()).hexdigest()
    manifest['status']='RECIPE_RENDERED_NOT_RELEASED'
    mp.write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf8')
    return len(samples)
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--kit',required=True);a=p.parse_args();print('Rendered',render_kit(a.kit),'recipe WAVs')
