# MpcVoyager

**Monophonic** Voyager-inspired synth for Melodee: morph oscillators, **LPF24 ladder**, glide, dual envelopes.

## Melodee knobs

1. **Cutoff** · 2. **Resonance** · 3. **Filter envelope amount**

## Programs

**16** factory G-funk / West Coast patches (`FactoryPrograms.h`). Scroll via host program change.

To grow toward a **200-preset** library, add `plugins/MpcVoyager/data/voyager_bank.json` and a loader in a follow-up (avoid huge compile-time tables).

## Build

```bash
./tools/build-vst.sh MpcVoyager
MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh MpcVoyager
```

Drop `MpcVoyager.vst3` into `Melodee/Plugins/` on the microSD card.

## Note

**One** ladder filter in v1 (your sketch mentioned dual ladders — planned v2). Not affiliated with Moog Music Inc.