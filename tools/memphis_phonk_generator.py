#!/usr/bin/env python3
"""Reference Memphis phonk MPC kit generator (parity target for C++ engine)."""
import os
import numpy as np
from scipy.io.wavfile import write
from scipy.signal import butter, lfilter

SAMPLE_RATE = 44100
OUTPUT_DIR = "Memphis_Phonk_MPC_Kit"
os.makedirs(OUTPUT_DIR, exist_ok=True)


def butter_filter(data, cutoff, btype="low", order=4):
    nyq = 0.5 * SAMPLE_RATE
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype=btype, analog=False)
    return lfilter(b, a, data)


def cassette_fx(audio, drive=2.5, bit_depth=12, hiss_level=0.004, lpf_cutoff=9500):
    saturated = np.tanh(audio * drive) / np.tanh(drive)
    steps = 2 ** (bit_depth - 1)
    crushed = np.round(saturated * steps) / steps
    hiss = np.random.normal(0, hiss_level, len(crushed)) * (np.abs(crushed) > 0.001)
    dirty = crushed + hiss
    warm = butter_filter(dirty, lpf_cutoff, btype="low", order=2)
    peak = np.max(np.abs(warm))
    if peak > 0:
        warm = (warm / peak) * 0.89
    return warm


def generate_kick(duration=0.55, start_freq=135.0, end_freq=44.0, punch=18.0):
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    freq_env = end_freq + (start_freq - end_freq) * np.exp(-t * punch)
    phase = 2 * np.pi * np.cumsum(freq_env) / SAMPLE_RATE
    body = np.sin(phase)
    click = np.random.normal(0, 0.3, len(t)) * np.exp(-t * 140)
    amp_env = np.exp(-t * 5.5)
    kick = (body + click) * amp_env
    return cassette_fx(kick, drive=3.2, bit_depth=12, lpf_cutoff=7500)


def generate_snare(duration=0.35, tone_freq=185.0):
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    pitch_env = tone_freq + 90 * np.exp(-t * 45)
    phase = 2 * np.pi * np.cumsum(pitch_env) / SAMPLE_RATE
    body = np.sin(phase) * np.exp(-t * 18)
    noise = np.random.normal(0, 1, len(t))
    noise = butter_filter(noise, 1100, btype="high", order=2)
    noise = butter_filter(noise, 6800, btype="low", order=2)
    snare_wires = noise * np.exp(-t * 12)
    snare = (body * 0.65) + (snare_wires * 0.85)
    return cassette_fx(snare, drive=2.8, bit_depth=12, lpf_cutoff=9000)


def generate_hihat(duration=0.09, open_hat=False):
    if open_hat:
        duration = 0.32
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    freqs = [205.3, 304.4, 369.6, 522.7, 540.0, 800.0]
    metal = sum(np.sign(np.sin(2 * np.pi * f * t)) for f in freqs) / len(freqs)
    noise = np.random.normal(0, 0.5, len(t))
    raw = butter_filter(metal + noise, 6500, btype="high", order=3)
    decay = 7.5 if open_hat else 48.0
    hat = raw * np.exp(-t * decay)
    return cassette_fx(hat, drive=2.0, bit_depth=12, lpf_cutoff=11000)


def generate_cowbell(duration=0.38, root_freq=540.0):
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    osc1 = np.sign(np.sin(2 * np.pi * root_freq * t))
    osc2 = np.sign(np.sin(2 * np.pi * (root_freq * 1.481) * t))
    raw = butter_filter(osc1 + osc2, 900, btype="high", order=2)
    raw = butter_filter(raw, 4200, btype="low", order=2)
    env = np.exp(-t * 9.5)
    return cassette_fx(raw * env, drive=2.6, bit_depth=12, lpf_cutoff=8000)


def generate_drum_loop(bpm=116, bars=2, swing=0.03):
    beat_dur = 60.0 / bpm
    step_dur = beat_dur / 4.0
    total_steps = bars * 16
    total_samples = int((total_steps * step_dur + 0.5) * SAMPLE_RATE)
    loop = np.zeros(total_samples)
    kick = generate_kick()
    snare = generate_snare()
    ch = generate_hihat(open_hat=False)
    oh = generate_hihat(open_hat=True)
    kick_steps = [0, 3, 7, 10, 14]
    snare_steps = [4, 12]
    oh_steps = [2, 10]
    for step in range(total_steps):
        bar_step = step % 16
        swing_offset = (step_dur * swing) if (step % 2 == 1) else 0.0
        pos = int((step * step_dur + swing_offset) * SAMPLE_RATE)
        if bar_step in oh_steps:
            loop[pos : pos + len(oh)] += oh * 0.55
        else:
            vel = 0.55 if step % 2 == 0 else 0.35
            loop[pos : pos + len(ch)] += ch * vel
        if bar_step in kick_steps:
            loop[pos : pos + len(kick)] += kick * 0.95
        if bar_step in snare_steps:
            loop[pos : pos + len(snare)] += snare * 0.90
    exact_len = int(total_steps * step_dur * SAMPLE_RATE)
    loop = loop[:exact_len]
    t_loop = np.arange(len(loop)) / SAMPLE_RATE
    wobble = np.sin(2 * np.pi * 1.8 * t_loop) * 18.0
    indices = np.clip(np.arange(len(loop)) + wobble, 0, len(loop) - 1)
    loop = np.interp(indices, np.arange(len(loop)), loop)
    return cassette_fx(loop, drive=2.4, bit_depth=12, hiss_level=0.005, lpf_cutoff=8800)


def save_wav(filename, audio):
    filepath = os.path.join(OUTPUT_DIR, filename)
    write(filepath, SAMPLE_RATE, np.int16(audio * 32767))


if __name__ == "__main__":
    for i in range(1, 5):
        save_wav(f"Phonk_Kick_0{i}.wav", generate_kick(start_freq=125 + i * 8, end_freq=42 + i * 2))
        save_wav(f"Phonk_Snare_0{i}.wav", generate_snare(tone_freq=175 + i * 12))
        save_wav(f"Phonk_ClosedHat_0{i}.wav", generate_hihat(open_hat=False))
        save_wav(f"Phonk_OpenHat_0{i}.wav", generate_hihat(open_hat=True))
        save_wav(f"Phonk_Cowbell_0{i}.wav", generate_cowbell(root_freq=520 + i * 15))
    save_wav("Memphis_Loop_112BPM.wav", generate_drum_loop(bpm=112, bars=2))
    save_wav("Memphis_Loop_118BPM.wav", generate_drum_loop(bpm=118, bars=2))
    save_wav("Memphis_Loop_124BPM.wav", generate_drum_loop(bpm=124, bars=2))
    print(f"Done! 23 dusty Memphis phonk WAVs saved to ./{OUTPUT_DIR}/")
