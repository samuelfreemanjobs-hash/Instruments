# MpcMessenger

Moog **Messenger**-style monophonic synth for **Melodee** on Akai MPC Sample: dual morph **VCOs** with **wavefold**, **LPF24 ladder** + **Res Bass** compensation, sub-octave square, white noise, **audio-rate FM** (Osc2 → Osc1), dual ADSR (filter + amp).

## Melodee K1–K3 (hardware macros)

**Cutoff** · **Resonance** · **Res Bass** (`resBassToggle` — values **> 0.5** enable low-end compensation under high resonance)

Eight factory programs via host program change (trap sub, fold lead, FM metal, …).

## Build (desktop x86_64 — iteration / pluginval)

```bash
git submodule update --init vst/JUCE
./tools/build-vst.sh MpcMessenger
```

## Build (aarch64 — Melodee microSD)

**Option A — native cross on Debian/Ubuntu** (install arm64 dev libs; see `docs/MPC_SAMPLE_VST3.md`):

```bash
MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh MpcMessenger
```

**Option B — Podman/Docker + dockcross** (no host multiarch):

```bash
./tools/build-vst-aarch64-podman.sh MpcMessenger
```

Verify the bundle before copying to the card:

```bash
./scripts/vst/verify-mpc-sample-vst3.sh \
  vst/MpcMessenger/build-aarch64/MpcMessenger_artefacts/Release/VST3/MpcMessenger.vst3
```

Expect: `MpcMessenger.vst3/Contents/aarch64-linux/MpcMessenger.so` (ARM ELF, not `x86_64-linux`).

## Install on MPC Sample

1. Power off, remove the Melodee microSD, mount on your computer.
2. Copy the **entire** `MpcMessenger.vst3` **folder** to `Melodee/Plugins/` on the card (not just the `.so`).
3. Eject safely, boot Melodee, pick **MpcMessenger** in the plugin browser.
4. Map **K1–K3** to cutoff / resonance / res bass; step factory programs from the host preset UI.

This layout matches how keremimo’s Melodee reference synths ship ARM64 Linux VST3 bundles — same path, custom Messenger-style DSP and macro order.