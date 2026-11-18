"""One command build, render, audit, QA, export. No release authorization."""
import argparse
import json
from pathlib import Path
from build_genre_kit import build
from render_recipes import render_kit
from verify_kit import verify
from diversity_audit import audit
from export_forge import export

def run(out,genre,sku='DKL',count=64,plan=None):
    out=Path(out)
    # Enforce brand new directory to avoid overwriting human work.
    if out.exists(): raise FileExistsError('destination exists: '+str(out))
    build(out,genre,sku,count,plan)
    render_kit(out)
    qa=verify(out,out/'manifest.json')
    diversity=audit(out)
    (out/'qa-report.json').write_text(json.dumps(qa,indent=2)+'\n',encoding='utf8')
    (out/'diversity-report.json').write_text(json.dumps(diversity,indent=2)+'\n',encoding='utf8')
    if qa['technical_status']!='PASS' or diversity['status']!='PASS':
        raise RuntimeError('QC failed; see reports. Kit remains blocked.')
    export(out,out/'forge-handoff.json')
    return {'kit':str(out),'genre':genre,'samples':len(qa['files']),
            'technical_qa':qa['technical_status'],'diversity':diversity['status'],
            'release_ready':qa['release_ready']}
if __name__=='__main__':
    p=argparse.ArgumentParser(description='Produce offline reference kit; never auto-approve release')
    p.add_argument('--out',required=True);p.add_argument('--genre',required=True)
    p.add_argument('--sku',default='DKL');p.add_argument('--count',type=int,default=64)
    p.add_argument('--plan',choices=['tail-light-static'])
    a=p.parse_args();print(json.dumps(run(a.out,a.genre,a.sku,a.count,a.plan),indent=2))
