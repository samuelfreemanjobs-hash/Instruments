# Template Effect (VST boilerplate)

**Do not ship from this folder.** Copy via:

```bash
./tools/new-vst.sh MyFx --type effect
```

Program chain: input → drive (tanh) → tone (shared `DspBiquad` LP) → dry/wet mix.
