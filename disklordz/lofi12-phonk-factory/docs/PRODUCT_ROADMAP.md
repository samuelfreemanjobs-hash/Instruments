# Lofi-12 phonk factory — product roadmap

## What you asked for vs scope

| Idea | Verdict | Status |
|------|---------|--------|
| **FX controlled by loop generator** | Core | Done: `phonk_loop_fx.py`, CLI `--filter/--reverb/--tape/--drive`, sequencer sliders + API |
| **Play loop & beat on it** | Core | Done: backing WAV upload + `/api/render_loop`, Web Audio step preview |
| **Edit step sequencer** | Core | Done: 6 tracks, slot/velocity edit, step inspector, groove import |
| **More tracks** | 6 UI lanes (4 = Lofi hardware + 2 extra MIDI) | Done in UI; native Lofi stays 4 |
| **Cloud / ambient one-shot generator** | **Separate product lane** | Phase 2 — not mixed into phonk engine yet (see below) |
| **Full FM plugin / external DAW** | Not needed | In-repo FM + Web Audio sufficient |

## Phase 2 (optional — not too much if scoped)

1. **`cloud_one_shot_factory.py`** — soft kicks, vinyl noise, rim, pad-stab textures (no phonk 808s). Different prompts/BPM defaults.
2. **Per-step FX locks** — filter/reverb automation on steps (export as CC lanes).
3. **Native Lofi SysEx** — push patterns/samples without manual sample.
4. **Reference pack tuning** — optional *your* WAV stats, not Splice cloning.

## Phase 3

- DAW-less **performance mode** (pad triggers + loop stem mute)
- Disklordz kit → auto bank + auto pattern

## How to use today

```bash
python3 disklordz/lofi12-phonk-factory/sequencer/serve.py

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --prompt "juicy j dirty memphis" --filter 0.45 --reverb 0.35 --tape 0.3 \
  --out ~/Music/Lofi12/Loops
```

Open sequencer → **Generate backing loop** → **Play** → edit grid → MIDI to Lofi when ready.
