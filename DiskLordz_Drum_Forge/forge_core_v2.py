#!/usr/bin/env python3
"""DiskLordz DRUM FORGE 4 synthesis core — deterministic, synthesis-only trap/phonk drums.

Dependencies: numpy, scipy, soundfile
Examples:
 python disklordz_drum_forge.py --preset all --variations 3 --out DiskLordz_Kit
 python disklordz_drum_forge.py --preset memphis --variations 8 --sr 48000
"""
from __future__ import annotations
import argparse, csv, json, math, re
from pathlib import Path
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt, resample_poly, lfilter

NOTES = ('C','C#','D','D#','E','F','F#','G','G#','A','A#','B')

def _voice(overrides):
    voice = {
        '808_decay': 1.0, '808_drive': 3.2, 'knock': 1.0,
        'snare_air': 0.8, 'hat_bright': 0.85, 'lofi': 0.04, 'verb': 0.1,
        '808_sag': 4.5, '808_translate': 0.28,
        'snare_tune': 1.0, 'snare_snap': 1.0,
        'hat_metal': 0.32, 'hat_decay': 1.0,
        'clap_color': 1.0, 'clap_decay': 1.0,
        'cowbell_tone': 1.0, 'perc_tone': 1.0,
    }
    voice.update(overrides)
    return voice

PRESETS = {
    'atl_trap': _voice({'808_decay': 1.18, '808_drive': 3.4, 'knock': 0.92, 'snare_air': 0.84, 'hat_bright': 0.9, 'lofi': 0.02, 'verb': 0.10, '808_sag': 4.2, '808_translate': 0.30, 'snare_tune': 1.0, 'snare_snap': 1.02, 'hat_metal': 0.34, 'clap_color': 1.05}),
    'memphis': _voice({'808_decay': 0.8, '808_drive': 5.4, 'knock': 1.05, 'snare_air': 0.58, 'hat_bright': 0.5, 'lofi': 0.22, 'verb': 0.05, '808_sag': 6.5, '808_translate': 0.22, 'snare_tune': 0.94, 'snare_snap': 0.78, 'hat_metal': 0.16, 'hat_decay': 0.85, 'clap_color': 0.72, 'cowbell_tone': 0.97}),
    'drift_phonk': _voice({'808_decay': 0.96, '808_drive': 6.2, 'knock': 1.22, 'snare_air': 1.12, 'hat_bright': 1.18, 'lofi': 0.08, 'verb': 0.09, '808_sag': 4.0, '808_translate': 0.36, 'snare_tune': 1.04, 'snare_snap': 1.22, 'hat_metal': 0.55, 'clap_color': 1.25}),
    'space_trap': _voice({'808_decay': 1.75, '808_drive': 2.5, 'knock': 0.62, 'snare_air': 0.7, 'hat_bright': 0.7, 'lofi': 0.0, 'verb': 0.24, '808_sag': 3.2, '808_translate': 0.18, 'snare_tune': 0.98, 'snare_snap': 0.84, 'hat_metal': 0.4, 'hat_decay': 1.15, 'clap_color': 0.9}),
    'southern_trap': _voice({'808_decay': 1.32, '808_drive': 4.4, 'knock': 1.16, 'snare_air': 0.8, 'hat_bright': 0.94, 'lofi': 0.05, 'verb': 0.11, '808_sag': 5.5, '808_translate': 0.32, 'snare_tune': 0.97, 'snare_snap': 1.05, 'hat_metal': 0.28, 'clap_color': 1.0}),
    'cloud_trap': _voice({'808_decay': 1.62, '808_drive': 2.3, 'knock': 0.52, 'snare_air': 0.55, 'hat_bright': 0.64, 'lofi': 0.02, 'verb': 0.26, '808_sag': 2.8, '808_translate': 0.16, 'snare_tune': 1.06, 'snare_snap': 0.7, 'hat_metal': 0.42, 'hat_decay': 1.2, 'clap_color': 0.8}),
    'raw_underground': _voice({'808_decay': 0.88, '808_drive': 5.8, 'knock': 1.08, 'snare_air': 0.68, 'hat_bright': 0.58, 'lofi': 0.28, 'verb': 0.04, '808_sag': 7.0, '808_translate': 0.24, 'snare_tune': 0.92, 'snare_snap': 1.12, 'hat_metal': 0.14, 'clap_color': 0.66, 'cowbell_tone': 0.95}),
}
FAMILIES = ('808','kick','snare','clap','closed_hat','open_hat','cowbell','perc')

def hz_to_note(hz):
    midi = int(round(69 + 12 * np.log2(max(1e-10, float(hz))/440)))
    return f'{NOTES[midi%12]}{midi//12-1}'

def note_to_hz(midi): return 440 * 2**((midi-69)/12)

def envelope(t, attack=0.001, decay=0.2, curve=1.0):
    return (1-np.exp(-t/max(attack,1e-5))) * np.exp(-(t/max(decay,1e-4))**curve)

def filter_signal(x, cutoff, kind, sr, order=2):
    nyq = sr/2
    cutoff = float(np.clip(cutoff, 20, nyq*0.92))
    sos = butter(order, cutoff, btype=kind, fs=sr, output='sos')
    return sosfilt(sos, x, axis=0)

def osc(freq, sr, phase=0):
    """Frequency in Hz, scalar or vector. Integrated phase avoids glides that click."""
    return np.sin(phase + 2*np.pi*np.cumsum(np.asarray(freq))/sr)

def fade(x, sr, fadein_ms=0.1, fadeout_ms=8):
    x = np.array(x, dtype=np.float64, copy=True)
    n = len(x)
    ni = min(n, int(sr*fadein_ms/1000))
    no = min(n, int(sr*fadeout_ms/1000))
    if ni: x[:ni] *= np.linspace(0,1,ni)[:,None] if x.ndim==2 else np.linspace(0,1,ni)
    if no: x[-no:] *= np.linspace(1,0,no)[:,None] if x.ndim==2 else np.linspace(1,0,no)
    return x

def saturate_oversampled(x, drive=3, mix=1, factor=4):
    """Oversample nonlinear stage to reduce aliasing (not a perfect analog model)."""
    up = resample_poly(x, factor, 1, axis=0)
    y = np.tanh(drive*up)/max(np.tanh(drive), 1e-8)
    y = resample_poly(y, 1, factor, axis=0)[:len(x)]
    if len(y)<len(x): y = np.pad(y, (0,len(x)-len(y)))
    return (1-mix)*x + mix*y

def sub_translation(x, sr, amount=0.22, cutoff=115):
    """Harmonic enhancement from isolated low end; high-pass added harmonics."""
    sub = filter_signal(x, cutoff, 'lowpass', sr, 4)
    harmonic = saturate_oversampled(sub, 3.6, 1)
    harmonic = filter_signal(harmonic, cutoff*1.05, 'highpass', sr, 3)
    return x + amount * harmonic

def comb(x, delay, feedback, damping=0.28):
    """Lowpass feedback comb. One zero + denominator path with fixed coefficients."""
    # Feedback filter: y[n] = x[n] + f*(1-d)*y[n-D] + f*d*y[n-D-1]
    a = np.zeros(delay+2)
    a[0]=1
    a[delay] = -feedback*(1-damping)
    a[delay+1] = -feedback*damping
    return lfilter([1.], a, x)

def allpass(x, delay, gain=0.55):
    # H(z) = (-g + z^-D) / (1 - g*z^-D)
    b = np.zeros(delay+1); a=np.zeros(delay+1)
    b[0]=-gain; b[delay]=1
    a[0]=1; a[delay]=-gain
    return lfilter(b, a, x)

def plate_reverb(x, sr, decay=0.2, wet=0.16):
    """Short damped Schroeder plate. Stereo delays, no sampled impulse."""
    mono = np.asarray(x, dtype=np.float64)
    if mono.ndim == 2:
        mono = mono.mean(axis=1)
    tail = int(sr * min(max(decay, 0.05), 0.4))
    padded = np.pad(mono, (0, tail))
    wet_channels = []
    delay_sets = ((11.7, 16.3, 22.1, 27.9), (12.4, 15.8, 21.4, 29.2))
    for delays in delay_sets:
        acc = np.zeros_like(padded)
        for d_ms in delays:
            delay = max(1, int(sr * d_ms / 1000))
            feedback = min(0.72, 10 ** (-3 * delay / (sr * max(decay, 0.05))))
            acc += comb(padded, delay, feedback, damping=0.55)
        acc /= 4
        for d_ms in (3.1, 1.3):
            acc = allpass(acc, max(1, int(sr * d_ms / 1000)), 0.45)
        wet_channels.append(acc)
    dry = np.column_stack((padded, padded))
    wet_mix = np.column_stack(wet_channels)
    return (1 - wet) * dry + wet * wet_mix

def room_reverb(x, sr, decay=0.36, wet=0.14):
    """Schroeder parallel comb + series allpass, stereo independent delay lines."""
    x=np.asarray(x)
    source = x if x.ndim==2 else np.column_stack([x,x])
    tail = int(sr*(decay+0.13))
    padded = np.pad(source,((0,tail),(0,0)))
    out=np.zeros_like(padded)
    for ch in range(2):
        comb_out=np.zeros(len(padded))
        for d_ms in ([29.7,37.1,41.1,43.7] if ch==0 else [30.7,35.9,40.3,44.9]):
            delay=max(1,int(sr*d_ms/1000))
            # -60 dB decay target: feedback^repetitions ~ 0.001
            feedback=min(0.90, 10**(-3*delay/(sr*max(decay,0.08))))
            comb_out += comb(padded[:,ch],delay,feedback)/4
        for d_ms in (5.0,1.7): comb_out=allpass(comb_out,max(1,int(sr*d_ms/1000)))
        out[:,ch]=comb_out
    out = (1-wet)*padded + wet*out
    return out if x.ndim==2 else out.mean(axis=1)

def quantize_character(x, sr, rng, amount):
    if amount<=0: return x
    dull = filter_signal(x, 12000 - 8500*amount, 'lowpass', sr, 2)
    noise = filter_signal(rng.standard_normal(np.shape(x)), 200, 'highpass', sr) * 0.0018*amount
    return (1-0.55*amount)*x + 0.55*amount*dull + noise

def gen_808(sr,rng,p,midi):
    root=note_to_hz(midi)
    decay=p['808_decay']
    length=max(1.15, min(decay*2.35, 4.0))
    t=np.arange(int(sr*length))/sr
    knock=rng.uniform(125,170)*p['knock']*np.exp(-t/rng.uniform(.009,.015))
    sag=p.get('808_sag', 4.5)*np.exp(-t/.08)
    pitch=root + knock + sag
    amp=envelope(t,.0005,decay,1.06)
    fund=osc(pitch,sr)*amp
    sub=filter_signal(fund,110,'lowpass',sr,4)
    mid=filter_signal(fund,150,'highpass',sr,3)
    octave=filter_signal(osc(pitch*2,sr)*amp,180,'highpass',sr,2)
    mid=saturate_oversampled(mid + 0.16*octave, max(1.4, p['808_drive']), mix=.82)
    click=filter_signal(rng.standard_normal(len(t)),1600,'highpass',sr,2)*np.exp(-t/.002)*0.05*p['knock']
    x=sub + 0.7*mid + click
    x=sub_translation(x,sr,amount=p.get('808_translate', 0.28), cutoff=95)
    return x,root

def gen_kick(sr,rng,p):
    t=np.arange(int(sr*.46))/sr
    f=50+rng.uniform(115,175)*p['knock']*np.exp(-t/.012)
    body=osc(f,sr)*envelope(t,.0003,.15,.85)
    click=filter_signal(rng.standard_normal(len(t)),2000,'highpass',sr,2)*np.exp(-t/.0018)*.15*p['knock']
    return saturate_oversampled(body+click, 1.7+0.35*p['knock'], .42), None

def gen_snare(sr,rng,p):
    tune=p.get('snare_tune', 1.0)
    snap=p.get('snare_snap', 1.0)
    t=np.arange(int(sr*.4))/sr
    pitch=(190*tune)+(130*tune)*np.exp(-t/.01)
    body=osc(pitch,sr)*np.exp(-t/.04)*.7
    noise=rng.standard_normal(len(t))
    wires=filter_signal(noise,1700,'highpass',sr,3)
    wires=filter_signal(wires,7200,'lowpass',sr,2)*np.exp(-t/.065)*0.5*snap*p['snare_air']
    crack=filter_signal(noise,3000,'highpass',sr,2)*np.exp(-t/.011)*.32*p['knock']
    wet=min(0.26, 0.06+p['verb'])
    return plate_reverb(body+wires+crack, sr, decay=0.16+0.12*p['verb'], wet=wet), None

def gen_clap(sr,rng,p):
    color=p.get('clap_color', 1.0)
    tail=p.get('clap_decay', 1.0)
    t=np.arange(int(sr*.4))/sr
    n=filter_signal(rng.standard_normal(len(t)),1000,'highpass',sr,2)
    n=filter_signal(n, min(sr*0.42, 2200+3800*color), 'lowpass', sr, 2)
    bursts=np.zeros_like(t)
    starts=(0.0, 0.011+float(rng.uniform(0,0.003)), 0.024+float(rng.uniform(0,0.004)), 0.039)
    for i,start in enumerate(starts):
        tau=0.016 if i<3 else 0.05*tail
        bursts += np.exp(-np.maximum(t-start,0)/tau)*(t>=start)
    wet=min(0.2, 0.04+p['verb']*0.45)
    return plate_reverb(n*bursts*0.62, sr, decay=0.1+0.06*p['verb'], wet=wet), None

def gen_hat(sr,rng,p,is_open=False):
    decay_scale=p.get('hat_decay', 1.0)
    duration=(0.7 if is_open else 0.15)*decay_scale
    t=np.arange(int(sr*duration))/sr
    bright=p['hat_bright']
    cutoff=float(np.clip(rng.uniform(5600,7800)*bright, 2500, min(12000, sr*0.33)))
    noise=filter_signal(rng.standard_normal(len(t)), cutoff, 'highpass', sr, 3)
    metal=np.zeros(len(t))
    base=min(2800*bright, sr*0.22)
    for i,ratio in enumerate((1.0, 1.63, 2.27, 3.11, 4.07)):
        freq=min(base*ratio, sr*0.46)
        metal += (0.52**i)*np.sin(2*np.pi*freq*t)
    decay=(0.24 if is_open else 0.03)*decay_scale
    x=(noise*0.62 + metal*p.get('hat_metal', 0.3))*envelope(t,.00015,decay,1.12)
    return x,None

def gen_cowbell(sr,rng,p):
    tone=p.get('cowbell_tone', 1.0)
    t=np.arange(int(sr*.42))/sr
    f1=(540+rng.uniform(-5,5))*tone
    f2=(812+rng.uniform(-6,6))*tone
    raw=np.sin(2*np.pi*f1*t)+0.62*np.sin(2*np.pi*f2*t)+0.18*np.sin(2*np.pi*f1*2*t)
    x=filter_signal(filter_signal(raw,420,'highpass',sr),5000,'lowpass',sr)
    x*=envelope(t,.0004,.11,.9)
    return saturate_oversampled(x,1.7,.32),None

def gen_perc(sr,rng,p):
    tone=p.get('perc_tone', 1.0)
    t=np.arange(int(sr*.32))/sr
    f=rng.uniform(280,520)*tone*np.exp(-t/.04)+rng.uniform(150,230)
    body=osc(f,sr)*envelope(t,.0002,.06)
    noise=filter_signal(rng.standard_normal(len(t)),2400,'highpass',sr)*np.exp(-t/.01)*.2*p.get('snare_snap', 1.0)
    return body*.8+noise,None

GENERATORS={'808':gen_808,'kick':gen_kick,'snare':gen_snare,'clap':gen_clap,
            'closed_hat':lambda sr,rng,p:gen_hat(sr,rng,p,False),
            'open_hat':lambda sr,rng,p:gen_hat(sr,rng,p,True),
            'cowbell':gen_cowbell,'perc':gen_perc}

def estimate_808_pitch(x,sr,expected_hz):
    """Stable-tail normalized autocorrelation plus agreement/confidence check."""
    s=int(sr*.24); e=min(len(x),s+int(sr*.34))
    v=np.asarray(x[s:e],dtype=float)
    if len(v)<2048 or np.max(np.abs(v))<1e-5: return None,0
    v=filter_signal(v,200,'lowpass',sr,3)
    v-=np.mean(v)
    # FFT-based linear autocorrelation avoids O(N^2) np.correlate.
    n=1 << (2*len(v)-1).bit_length()
    a=np.fft.irfft(abs(np.fft.rfft(v,n))**2,n)[:len(v)]
    lo=max(1,int(sr/(expected_hz*1.13)))
    hi=min(len(a),int(sr/(expected_hz*.87))+1)
    if hi<=lo:return None,0
    lag=lo+int(np.argmax(a[lo:hi])); confidence=float(a[lag]/max(a[0],1e-12))
    return sr/lag,confidence

def true_peak_db(x, factor=4):
    y=resample_poly(x,factor,1,axis=0)
    return float(20*np.log10(max(np.max(np.abs(y)),1e-12)))

def master(x,sr,ceiling_db=-1.0):
    """DC removal and fades, then static gain to the 4x true-peak estimate.

    Gain can rise or fall. Makeup is capped at +24 dB. This is not a lookahead limiter.
    """
    x=np.asarray(x,dtype=np.float64)
    x=x-np.mean(x,axis=0)
    x=fade(x,sr)
    peak=np.max(np.abs(x))
    if peak>8: x=x*(8/peak)
    measured=10**(true_peak_db(x)/20)
    if measured<1e-8: return x
    ceiling=10**(ceiling_db/20)
    scale=min(ceiling/measured, 10**(24/20))
    return x*scale

def main():
    ap=argparse.ArgumentParser(description='DiskLordz Drum Forge: original synthesis-only commercial-ready kit foundation')
    ap.add_argument('--preset',choices=(*PRESETS,'all'),default='all')
    ap.add_argument('--variations',type=int,default=3)
    ap.add_argument('--sr',type=int,choices=(44100,48000,96000),default=44100)
    ap.add_argument('--seed',type=int,default=808660)
    ap.add_argument('--out',default='DiskLordz_Drum_Forge')
    ap.add_argument('--ceiling',type=float,default=-1.0,help='true-peak ceiling in dBTP, default -1')
    args=ap.parse_args()
    if not 1<=args.variations<=100: ap.error('--variations must be between 1 and 100')
    if not -12<=args.ceiling<=-0.1: ap.error('--ceiling must be between -12 and -0.1 dBTP')
    root=Path(args.out)
    presets=list(PRESETS) if args.preset=='all' else [args.preset]
    rows=[]
    for pi,preset in enumerate(presets):
        p=PRESETS[preset]
        for fi,family in enumerate(FAMILIES):
            for i in range(args.variations):
                seed=args.seed+pi*100000+fi*1000+i*17
                rng=np.random.default_rng(seed)
                midi=int(rng.choice([24,26,28,29,31,33,35,36,38,40,41,43]))
                x,root_hz=(gen_808(args.sr,rng,p,midi) if family=='808' else GENERATORS[family](args.sr,rng,p))
                if x.ndim==1 and family not in ('808','kick'):
                    x=quantize_character(x,args.sr,rng,p['lofi'])
                # 808 root tags come from the oscillator, independently verify tail where possible.
                note=hz_to_note(root_hz) if root_hz else ''
                detected,confidence=estimate_808_pitch(x,args.sr,root_hz) if root_hz else (None,0)
                valid=detected is not None and abs(12*np.log2(detected/root_hz))<.45 and confidence>=.5
                x=master(x,args.sr,args.ceiling)
                suffix=f'_{note}' if note else ''
                name=f'DiskLordz_{preset}_{family}{suffix}_V{i+1:03d}.wav'
                target=root/preset/family/name
                target.parent.mkdir(parents=True,exist_ok=True)
                sf.write(target,x,args.sr,subtype='PCM_24')
                tp=true_peak_db(x)
                if tp>args.ceiling+.03: raise RuntimeError(f'True peak ceiling failed: {target}: {tp:.2f}')
                rows.append({'file':str(target.relative_to(root)),'preset':preset,'family':family,
                  'seed':seed,'sample_rate':args.sr,'channels':(1 if x.ndim==1 else 2),
                  'frames':len(x),'root_note':note,'root_hz':round(root_hz,3) if root_hz else '',
                  'detected_hz':round(detected,3) if detected else '',
                  'pitch_confidence':round(confidence,3),'pitch_verified':valid if root_hz else '',
                  'sample_peak_dbfs':round(20*np.log10(max(np.max(np.abs(x)),1e-12)),2),
                  'true_peak_dbtp':round(tp,2)})
                print(f'[{len(rows):03}] {target} | TP {tp:.2f} dBTP')
    with (root/'manifest.csv').open('w',newline='',encoding='utf-8') as f:
        writer=csv.DictWriter(f,fieldnames=rows[0].keys());writer.writeheader();writer.writerows(rows)
    with (root/'manifest.json').open('w',encoding='utf-8') as f:json.dump({'engine':'DiskLordz Drum Forge v2',
        'parameters':vars(args),'files':rows},f,indent=2)
    (root/'README.txt').write_text('DiskLordz Drum Forge v2 — original, procedural, synthesis-only samples.\n'
        '24-bit PCM WAV. 808s are root-tagged from known oscillator tuning; independent analysis in manifest.\n'
        'True-peak constrained gain with 4x interpolation, not a dynamic lookahead limiter.\n'
        'Test in your DAW and audition at different loudness levels before commercial release.\n',encoding='utf-8')
    print(f'Created {len(rows)} samples in {root.resolve()}')

if __name__=='__main__':main()
