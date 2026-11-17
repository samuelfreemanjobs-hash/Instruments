"""Native C++ WAV batch rendering for v1.3 manifests; technical QA required afterward.

This explicit adapter does not imply bit-identical Python and C++ synthesis.
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path
from build_genre_kit import build
from recipe_engine import validate_recipe, recipe_hash
from verify_kit import safe_relative, verify
from diversity_audit import audit

KINDS={'808':0,'KICK':1,'SNARE':2,'CLAP':3,'HAT':4,'COWBELL':5,'PERC':6,'FX':7}

def native_run(out, renderer, genre='drift-phonk', count=64, sku='DKL', plan=None):
    out=Path(out).resolve()
    renderer=Path(renderer).resolve(strict=True)
    if not renderer.is_file():raise ValueError('renderer executable not found')
    if out.exists():raise FileExistsError('kit destination already exists')
    manifest=build(out,genre,sku,count,plan)
    samples=manifest['samples']
    seen=set()
    # Preflight entire manifest before writing sample WAVs.
    for e in samples:
        rel=e['path'];r=e['recipe_params']
        if not safe_relative(rel) or not rel.startswith('masters/') or rel in seen:raise ValueError('unsafe/duplicate output path')
        seen.add(rel)
        if (out/rel).resolve().is_relative_to(out) is False:raise ValueError('path outside output')
        validate_recipe(r)
        if recipe_hash(r)!=e['recipe_sha256']:raise ValueError('recipe hash mismatch')
    for e in samples:
        r=e['recipe_params'];dst=out/e['path'];dst.parent.mkdir(parents=True,exist_ok=True)
        cmd=[str(renderer),str(dst),str(KINDS[e['kind']]),str(r['pitch_hz'] or 52),str(e['seed']),
             str(r['decay_sec']),str(r['drive']),str(r['tone']),str(r['noise_color']),str(r['duration_sec']),str(e['sample_rate'])]
        subprocess.run(cmd,check=True,capture_output=True,text=True,timeout=35)
        e['sha256']=hashlib.sha256(dst.read_bytes()).hexdigest()
    manifest['status']='NATIVE_RENDERED_NOT_RELEASED'
    manifest['generator']='drumsmith-cxx-v2-native'
    (out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    qa=verify(out,out/'manifest.json')
    diversity=audit(out)
    (out/'qa-report.json').write_text(json.dumps(qa,indent=2)+'\n')
    (out/'diversity-report.json').write_text(json.dumps(diversity,indent=2)+'\n')
    return {'kit':str(out),'samples':len(samples),'technical_qa':qa['technical_status'],
            'errors':qa['errors'],'diversity':diversity['status'],'release_ready':False}
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--renderer',required=True)
    p.add_argument('--genre',required=True);p.add_argument('--count',type=int,default=64)
    p.add_argument('--sku',default='DKL');p.add_argument('--plan',choices=['tail-light-static'])
    a=p.parse_args();result=native_run(a.out,a.renderer,a.genre,a.count,a.sku,a.plan)
    print(json.dumps(result,indent=2));sys.exit(0 if result['technical_qa']=='PASS' and result['diversity']=='PASS' else 1)
