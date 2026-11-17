"""Deterministic, recipe-driven original reference DSP. Not a real-time engine or finished master."""
import math
import random
import hashlib
import json

KINDS = ('808','KICK','SNARE','CLAP','HAT','COWBELL','PERC','FX')

def recipe_for(kind, variant=0):
    if kind not in KINDS or not isinstance(variant, int) or variant < 0:
        raise ValueError('unsupported kind/variant')
    # Every variant changes at least two audible parameters; deterministic by index.
    n = variant % 16
    return {'engine':'parametric-v2', 'kind':kind, 'variant':variant,
            'duration_sec': round((0.76 + (n%4)*0.09) if kind=='808' else (0.28+(n%4)*0.045) if kind=='COWBELL' else (0.42+(n%5)*0.075) if kind=='FX' else (0.22+(n%5)*0.065), 4),
            'decay_sec': round((0.31+(n%5)*0.045) if kind=='808' else (0.065+(n%6)*0.016),4),
            'drive': round(1.1+(n%6)*0.19,4),
            'tone': round(0.17+(n%7)*0.08,4),
            'pitch_hz':52.0 if kind=='808' else 62.0+(n%5)*9 if kind=='KICK' else 540.0 if kind=='COWBELL' else None,
            'noise_color':round(0.25+(n%7)*0.095,4)}

def validate_recipe(r):
    if not isinstance(r,dict) or r.get('engine')!='parametric-v2' or r.get('kind') not in KINDS: raise ValueError('invalid engine/kind')
    for name, lo, hi in [('duration_sec',.08,3.0),('decay_sec',.01,2.0),('drive',.1,8),('tone',0,1),('noise_color',0,1)]:
        x=r.get(name)
        if not isinstance(x,(float,int)) or isinstance(x,bool) or not math.isfinite(x) or not lo<=x<=hi: raise ValueError('invalid '+name)
    pitch=r.get('pitch_hz')
    if r['kind'] in ('808','KICK','COWBELL') and (not isinstance(pitch,(int,float)) or not math.isfinite(pitch) or not 25<=pitch<=2500): raise ValueError('invalid pitch')
    if r['kind'] not in ('808','KICK','COWBELL') and pitch is not None: raise ValueError('unexpected pitch')

def render_recipe(r, rate, seed):
    validate_recipe(r)
    if not isinstance(rate,int) or not 8000<=rate<=192000: raise ValueError('bad rate')
    rng=random.Random(seed)
    count=round(r['duration_sec']*rate)
    kind=r['kind']; decay=r['decay_sec']; tone=r['tone']; drive=r['drive']; base=r['pitch_hz'] or 0
    samples=[]; phase=0.0; filtered=0.0
    for i in range(count):
        t=i/rate
        env=math.exp(-t/decay)
        if kind in ('808','KICK'):
            glide= (95 if kind=='808' else 150) * math.exp(-t/(0.012+tone*0.013))
            phase+=2*math.pi*(base+glide)/rate
            body=math.sin(phase) + (0.10+tone*0.12)*math.sin(2*phase)
            x=math.tanh(drive*body)*(1-math.exp(-t/0.0015))*env*0.64
        elif kind=='COWBELL':
            phase+=2*math.pi*base/rate
            x=(math.sin(phase)+(.30+.3*tone)*math.sin(phase*(1.48+tone*.05)))*env*.42
        else:
            raw=2*rng.random()-1
            filtered = filtered*(.45+.5*tone)+raw*(.55-.5*tone)
            hp=raw-filtered
            burst=1.0
            if kind=='CLAP':
                burst=sum(math.exp(-max(0,t-o)/(.006+.014*tone)) for o in (0,.011,.024,.038) if t>=o)
            elif kind=='HAT': burst=math.exp(-t/(.025+.055*tone)) / max(env,1e-8)
            elif kind=='SNARE': burst=1.0+.35*math.sin(2*math.pi*150*t)
            elif kind=='PERC': burst=math.sin(2*math.pi*(230+tone*330)*t)*1.1
            elif kind=='FX': burst=math.sin(2*math.pi*(15*t+45*t*t))
            x=math.tanh(drive*(hp*r['noise_color'] + .22*math.sin(2*math.pi*(120+tone*150)*t))*burst)*env*.49
        samples.append(max(-0.97,min(.97,x)))
    return samples

def recipe_hash(recipe):
    return hashlib.sha256(json.dumps(recipe,sort_keys=True,separators=(',',':')).encode()).hexdigest()
