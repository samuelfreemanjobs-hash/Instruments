# MpcDxFm

DX7-style **4-op FM** for Melodee (Method 2 VA). 12 voices, 8 algorithms, 6 factory programs. Optional chip+ROM path documented in `docs/MPC_CHIP_EMULATION.md`.

## Melodee K1–K3

**Brightness** · **Feedback** · **Algorithm**

## Build

```bash
./tools/build-vst.sh MpcDxFm
MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh MpcDxFm
./scripts/vst/verify-mpc-sample-vst3.sh vst/MpcDxFm/build-aarch64/.../MpcDxFm.vst3
```