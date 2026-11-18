"""Generate read-only JUCE/FORGE implementation handoff; never claim plugin compiled."""
import argparse
import json
from pathlib import Path
from recipe_engine import recipe_hash

def export(kit,out):
    kit=Path(kit)
    m=json.loads((kit/'manifest.json').read_text(encoding='utf8'))
    specs=[]
    for e in m['samples']:
        if e.get('recipe')!='parametric-v2' or recipe_hash(e['recipe_params'])!=e.get('recipe_sha256'):
            raise ValueError('invalid recipe provenance')
        specs.append({'id':e['id'],'kind':e['kind'],'seed':e['seed'],'sample_rate':e['sample_rate'],'parameters':e['recipe_params'],'reference_sha256':e.get('sha256')})
    result={'contract':'drumsmith-forge-v1','source_kit':m['sku'],'implementation':'JUCE 8 / C++20','status':'SPECIFICATION_ONLY_NOT_COMPILED','acceptance':['offline-vs-C++ output comparison','no heap allocation on audio callback','denormal protections','parameter bounds','sample-rate changes','CPU budget profiling','multivoice isolation','DAW scan tests'],'voices':specs}
    out=Path(out);out.parent.mkdir(parents=True,exist_ok=True)
    if out.exists(): raise FileExistsError(out)
    out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf8')
    return len(specs)
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--kit',required=True);p.add_argument('--out',required=True);a=p.parse_args();print('Exported',export(a.kit,a.out),'voice specifications')
