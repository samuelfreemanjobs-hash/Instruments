# NTS-1 mkII oscillator boilerplate

**Ship target:** mkII-only (`.nts1mkiiunit`).

## New unit

```bash
chmod +x tools/new-mkii-oscillator.sh
./tools/new-mkii-oscillator.sh my_slug "Display Name"
```

Then implement DSP in `src/mkii/oscillators/my_slug/osc.h` and add `/* PORT_COMPLETE */` at top when done.

## Build & eval

```bash
./tools/mkii-automate.sh bootstrap   # once: LOGUE_SDK + ARM gcc
./tools/osc-eval-mkii.sh my_slug --static-only
./tools/build-mkii.sh my_slug
```

## Parameters

Hardware knobs: **0–1023**. Use `param_10bit_to_f32(value)` in `setParameter()`.

## Golden reference

`src/mkii/oscillators/tr808_kick_phonk/osc.h` — full port with `noteOn` / envelope.

## Agent workflow

[/logue-mkii-* commands](logue-mkii-workflow.md) · [collection roadmap](logue-oscillator-collection-roadmap.md)
