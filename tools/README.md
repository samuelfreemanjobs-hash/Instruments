# Disklordz offline factory

Batch-render drum WAVs and store **sample DNA** in SQLite. Real-time playing belongs in the **JUCE** path—see [docs/DISKLORDZ_BLUEPRINT.md](../docs/DISKLORDZ_BLUEPRINT.md).

## Setup

```bash
cd tools
python3 -m venv .venv
source .venv/bin/activate   # Windows: .venv\Scripts\activate
pip install -r requirements.txt
```

## Render a starter kit

```bash
python render_kit.py --kit-name MEMPHIS_RAW_V1
```

Outputs:

- WAVs → `tools/output/samples/`
- Catalog → `tools/data/disklordz.db`

### Options

| Flag | Purpose |
|------|---------|
| `--mode grid808` | Render N random 808 variants (`--count`) |
| `--refine-grit` | Low-RMS grit loop (tape + Memphis crunch) |
| `--seed 42` | Reproducible noise |
| `--slack-webhook URL` | Or set `DISKLORDZ_SLACK_WEBHOOK` |

## Reference profiling

Drop favorite kicks into `tools/references/` (create the folder), then:

```bash
python profile_references.py references/
```

Re-render kits; `render_kit.py` prints a **match score** vs mean reference crest/ZCR when references exist.

## Layout

```
tools/
├── schema.sql
├── render_kit.py
├── profile_references.py
├── disklordz/
│   ├── synthesis.py   # DSP (mirror in JUCE later)
│   ├── audit.py       # RMS, crest, ZCR, sub peak
│   ├── db.py
│   └── references.py
├── data/              # gitignored
└── output/            # gitignored
```
