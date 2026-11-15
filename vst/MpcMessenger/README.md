# MpcMessenger

Moog **Messenger**-style monophonic synth for **Melodee** on Akai MPC Sample: dual morph **VCOs** with **wavefold**, **LPF24 ladder** + **Res Bass** low-end compensation, sub-octave square, white noise, **audio-rate FM** (Osc2 → Osc1), dual ADSR (filter + amp).

## Melodee K1–K3

**Cutoff** · **Resonance** · **Res Bass**

## Programs

8 factory patches (trap sub, fold lead, FM metal, …). Host program change in Melodee.

## Build

```bash
./tools/build-vst.sh MpcMessenger
MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh MpcMessenger
```