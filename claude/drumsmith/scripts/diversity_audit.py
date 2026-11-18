"""Conservative duplicate detection. Same hashes are hard failures; similarity is advisory."""
import argparse
import hashlib
import json
import struct
import wave
from collections import defaultdict
from pathlib import Path

def fingerprint(path, bins=64):
    with wave.open(str(path),'rb') as w:
        raw=w.readframes(w.getnframes()); n=w.getnframes()
    values=[x[0]/32768 for x in struct.iter_unpack('<h',raw)]
    if not values: return [0]*bins
    # Block RMS signature; shape only, normalized so gain variations won't conceal near-duplicates.
    vector=[]
    for i in range(bins):
        chunk=values[int(i*n/bins):int((i+1)*n/bins)]
        vector.append((sum(x*x for x in chunk)/len(chunk))**.5 if chunk else 0)
    mx=max(vector) or 1
    return [v/mx for v in vector]

def audit(kit):
    kit=Path(kit).resolve()
    data=json.loads((kit/'manifest.json').read_text(encoding='utf8'))
    groups=defaultdict(list); warnings=[]; signatures=[]
    for e in data['samples']:
        p=(kit/e['path']).resolve()
        if not p.is_relative_to(kit) or not p.is_file(): raise ValueError('missing or escaped sample path')
        digest=hashlib.sha256(p.read_bytes()).hexdigest()
        groups[digest].append(e['id'])
        signatures.append((e['id'],e['kind'],fingerprint(p)))
    duplicates=[g for g in groups.values() if len(g)>1]
    for i,(aid,kind,a) in enumerate(signatures):
        for bid,k2,b in signatures[i+1:]:
            if kind!=k2: continue
            diff=sum((x-y)**2 for x,y in zip(a,b))/len(a)
            if diff<.0004: warnings.append({'a':aid,'b':bid,'shape_mse':round(diff,7)})
    return {'status':'FAIL' if duplicates else 'PASS','exact_duplicate_groups':duplicates,'similar_envelope_pairs_advisory':warnings,'files_checked':len(signatures), 'limitations':'Envelope similarity cannot replace listening, spectral analysis or content review'}
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--kit',required=True);p.add_argument('--json');a=p.parse_args();r=audit(a.kit);print(json.dumps(r,indent=2));
    if a.json:Path(a.json).write_text(json.dumps(r,indent=2)+'\n',encoding='utf8')
    raise SystemExit(0 if r['status']=='PASS' else 1)
