"""Build recipe manifests without overwriting prior work."""
import argparse
import json
from pathlib import Path
from scaffold_kit import scaffold
from recipe_engine import recipe_for, recipe_hash

def build(out, sku='TLS', plan='tail-light-static', count=64):
    manifest=scaffold(out,sku,count,plan)
    variants={}
    for e in manifest['samples']:
        kind=e['kind']; n=variants.get(kind,0); variants[kind]=n+1
        recipe=recipe_for(kind,n)
        e['recipe']='parametric-v2'; e['recipe_params']=recipe
        e['recipe_sha256']=recipe_hash(recipe)
        e['pitch_target_hz']=recipe['pitch_hz'] if kind=='808' else None
        e['duration_max_sec']=recipe['duration_sec']+.02
        e['duration_min_sec']=recipe['duration_sec']-.02
    manifest['generator']='drumsmith-recipe-engine-v2'
    manifest['status']='RECIPE_DRAFT'
    (Path(out)/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf8')
    return manifest

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--sku',default='TLS');p.add_argument('--count',type=int,default=64);p.add_argument('--plan',choices=['tail-light-static'],default='tail-light-static')
    a=p.parse_args(); print('Created',len(build(a.out,a.sku,a.plan,a.count)['samples']),'recipe entries')
