#!/usr/bin/env python3
"""Validate portable JSON sample keygroup metadata (stdlib only)."""
import json, pathlib, sys, wave

def main(path):
    path=pathlib.Path(path)
    zones=json.loads(path.read_text(encoding='utf-8'))
    assert isinstance(zones,list) and zones, 'expected nonempty JSON array'
    for i,z in enumerate(zones):
        for field in ('file','root','low','high'):
            assert field in z, f'zone {i} missing {field}'
        assert 0<=z['low']<=z['root']<=z['high']<=127
        with wave.open(str(path.parent/z['file'])) as w:
            assert w.getnframes()>1 and w.getnchannels()>=1
        print('OK',z['file'])
    print(f'Validated {len(zones)} zones')
if __name__=='__main__':
    main(sys.argv[1] if len(sys.argv)>1 else 'keygroup.json')
