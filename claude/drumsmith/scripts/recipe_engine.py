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
    # cycle + variant salts keep large kits unique (native 808/kick ignore seed).
    n = variant % 16
    cycle = variant // 16
    # RATE-style 808 beds: long buffer + long amp decay (Memphis crawl / phonk)
    dur_808 = 1.45 + (n % 5) * 0.16 + cycle * 0.08 + (variant % 3) * 0.04
    decay_808 = 0.95 + (n % 5) * 0.14 + cycle * 0.08 + (variant % 4) * 0.03
    # Spread 808 fundamentals across ~36–58 Hz for RATE-style beds
    pitch_808 = 58.0 - (variant % 22) * 1.0 - cycle * 0.35
    kick_pitch = 55.0 + (variant % 18) * 2.5 + cycle * 1.5
    cow_pitch = 480.0 + (variant % 12) * 14.0 + cycle * 8.0
    # Drum oneshots: longer than truncated ~0.22s clips so decay can finish
    dur_kick = 0.42 + (n % 5) * 0.07 + cycle * 0.03 + (variant % 5) * 0.012
    decay_kick = 0.16 + (n % 6) * 0.035 + cycle * 0.015 + (variant % 8) * 0.004
    dur_snare = 0.40 + (n % 5) * 0.055 + cycle * 0.025 + (variant % 5) * 0.01
    decay_snare = 0.14 + (n % 6) * 0.028 + cycle * 0.012 + (variant % 8) * 0.003
    dur_clap = 0.32 + (n % 4) * 0.05 + cycle * 0.02 + (variant % 5) * 0.01
    decay_clap = 0.10 + (n % 5) * 0.022 + cycle * 0.01 + (variant % 7) * 0.003
    dur_hat = 0.28 + (n % 5) * 0.05 + cycle * 0.02 + (variant % 5) * 0.008
    decay_hat = 0.08 + (n % 6) * 0.02 + cycle * 0.01 + (variant % 8) * 0.002
    # FX families widen duration/tone/noise so large kits stay SHA-unique and usefully distinct.
    fx_family = variant % 5  # 0 crash · 1 alias · 2 reverse/sweep · 3 stab · 4 texture
    fx_dur = (0.55 + (n % 5) * 0.11 + cycle * 0.04 + (variant % 11) * 0.012
              if fx_family == 0 else
              0.28 + (n % 4) * 0.07 + cycle * 0.025 + (variant % 9) * 0.01
              if fx_family == 1 else
              0.70 + (n % 6) * 0.12 + cycle * 0.05 + (variant % 7) * 0.015
              if fx_family == 2 else
              0.18 + (n % 5) * 0.05 + cycle * 0.02 + (variant % 8) * 0.008
              if fx_family == 3 else
              0.90 + (n % 6) * 0.14 + cycle * 0.055 + (variant % 10) * 0.02)
    fx_decay = (0.12 + (n % 6) * 0.03 + cycle * 0.015 + (variant % 7) * 0.004
                if fx_family in (0, 2) else
                0.04 + (n % 5) * 0.012 + cycle * 0.008 + (variant % 9) * 0.002
                if fx_family in (1, 3) else
                0.22 + (n % 6) * 0.04 + cycle * 0.02 + (variant % 8) * 0.005)
    fx_noise = min(1.0, max(0.0, 0.15 + fx_family * 0.12 + (n % 7) * 0.08 + cycle * 0.03 + (variant % 6) * 0.015))
    fx_tone = min(1.0, max(0.0, 0.08 + fx_family * 0.1 + (n % 7) * 0.07 + cycle * 0.035 + (variant % 9) * 0.012))
    if kind == '808':
        duration_sec = min(2.8, dur_808)
        decay_sec = min(1.9, decay_808)
    elif kind == 'KICK':
        duration_sec, decay_sec = min(1.2, dur_kick), min(0.55, decay_kick)
    elif kind == 'SNARE':
        duration_sec, decay_sec = min(1.0, dur_snare), min(0.45, decay_snare)
    elif kind == 'CLAP':
        duration_sec, decay_sec = min(0.85, dur_clap), min(0.35, decay_clap)
    elif kind == 'HAT':
        duration_sec, decay_sec = min(0.75, dur_hat), min(0.28, decay_hat)
    elif kind == 'COWBELL':
        duration_sec = 0.28 + (n % 4) * 0.045 + cycle * 0.02 + (variant % 5) * 0.01
        decay_sec = 0.065 + (n % 6) * 0.016 + cycle * 0.008 + (variant % 8) * 0.002
    elif kind == 'FX':
        duration_sec, decay_sec = min(2.9, fx_dur), min(1.6, fx_decay)
    else:  # PERC
        duration_sec = 0.28 + (n % 5) * 0.05 + cycle * 0.02 + (variant % 5) * 0.008
        decay_sec = 0.08 + (n % 6) * 0.02 + cycle * 0.01 + (variant % 8) * 0.002
    return {'engine':'parametric-v2', 'kind':kind, 'variant':variant,
            'duration_sec': round(duration_sec, 4),
            'decay_sec': round(decay_sec, 4),
            'drive': round(min(7.5, 1.05+(n%6)*0.19+cycle*0.11+(variant%5)*0.03 + (fx_family*0.08 if kind=='FX' else 0)),4),
            'tone': round(fx_tone if kind=='FX' else min(1.0, max(0.0, 0.12+(n%7)*0.08+cycle*0.04+(variant%9)*0.015)),4),
            'pitch_hz': (max(32.0, min(70.0, pitch_808)) if kind=='808' else max(40.0, min(120.0, kick_pitch)) if kind=='KICK' else max(350.0, min(900.0, cow_pitch)) if kind=='COWBELL' else None),
            'noise_color':round(fx_noise if kind=='FX' else min(1.0, max(0.0, 0.2+(n%7)*0.095+cycle*0.035+(variant%6)*0.02)),4)}

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
            elif kind=='FX':
                fam = int(r.get('variant', 0)) % 5
                dur = max(0.08, float(r['duration_sec']))
                u = min(1.0, max(0.0, t / dur))
                if fam == 0:
                    burst = (0.55 + 0.45 * math.sin(2 * math.pi * (180 + tone * 420) * t)) * (1.0 + 0.35 * math.exp(-t / (0.04 + tone * 0.08)))
                elif fam == 1:
                    step_hz = 18.0 + tone * 40.0
                    step = math.floor(t * step_hz) / step_hz
                    burst = 0.7 + 0.6 * math.sin(2 * math.pi * (90 + tone * 500) * step)
                elif fam == 2:
                    burst = math.sin(2 * math.pi * (8 * t + 90 * t * t + tone * 40 * t))
                    rev = u ** (0.55 + tone * 0.35)
                    x = math.tanh(drive * (hp * r['noise_color'] + 0.28 * burst) * rev) * env * 0.52
                    samples.append(max(-0.97, min(0.97, x)))
                    continue
                elif fam == 3:
                    click = math.exp(-t / (0.008 + tone * 0.01))
                    body = math.sin(2 * math.pi * (55 + tone * 90) * t) * math.exp(-t / (0.05 + decay * 0.2))
                    burst = click * 1.4 + body
                else:
                    burst = 0.35 + 0.25 * math.sin(2 * math.pi * (12 + tone * 28) * t) + 0.2 * math.sin(2 * math.pi * (0.7 + r['noise_color']) * t)
            x=math.tanh(drive*(hp*r['noise_color'] + .22*math.sin(2*math.pi*(120+tone*150)*t))*burst)*env*.49
        samples.append(max(-0.97,min(.97,x)))
    return samples

def recipe_hash(recipe):
    return hashlib.sha256(json.dumps(recipe,sort_keys=True,separators=(',',':')).encode()).hexdigest()
