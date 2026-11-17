# MpcSubPhatty

Sub Phatty–inspired **mono** bass/lead: morph oscs, square **sub**, **Multidrive** pre/post, **LPF12/24**.

## Melodee K1–K3

**Cutoff** · **Resonance** · **Multidrive**

## Programs

16 factory patches (808, Reese growl, Acid Phatty, …). Host program change in Melodee.

200-preset bank: add `plugins/MpcSubPhatty/data/` JSON loader (same pattern as MpcVoyager).

## Build

```bash
./tools/build-vst.sh MpcSubPhatty
MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh MpcSubPhatty
```