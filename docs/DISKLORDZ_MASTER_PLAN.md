# DiskLordz Audio — Business & Technical Master Plan

**Owner agent:** [`audio-business-copilot`](../disklordz/agents/profit/audio-business-copilot/agent.md)  
**Engineering agent (VST only):** Cursor Cloud + [`golden-wav-qa`](../disklordz/agents/profit/golden-wav-qa/agent.md) — not the business copilot.

Companion specs:

| Document | Purpose |
|----------|---------|
| [**DiskLordz_Audio_Master_Roadmap_for_Cursor.md**](DiskLordz_Audio_Master_Roadmap_for_Cursor.md) | Full business + technical brief (§18–20 = Cursor engineering rules) |
| [DPF_TECHNICAL_ARCHITECTURE.md](DPF_TECHNICAL_ARCHITECTURE.md) | Shared plugin framework (DPF) |
| [CHORD_ENGINE_PRD.md](CHORD_ENGINE_PRD.md) | Flagship product PRD + acceptance tests |
| [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) | Factory lanes vs HISE |
| [ROADMAP.md](ROADMAP.md) | Monorepo-wide progress block |

**Operator UI:** **Coach** PWA (one task at a time) — [disklordz/audio-command/ARCHITECTURE.md](../disklordz/audio-command/ARCHITECTURE.md) · outreach [OUTREACH_SETUP.md](../disklordz/audio-command/docs/OUTREACH_SETUP.md)

---

## Operating model

| Decision | Direction |
|----------|-----------|
| Company | DiskLordz Audio |
| Market | Trap, phonk, underground hip-hop |
| Initial platform | Windows 10/11, 64-bit |
| Future platform | macOS |
| Development | Shared C++ plugin framework (DPF) |
| First product | CHORD ENGINE |
| Initial budget | Near $0 |
| Revenue model | Paid perpetual licenses |
| Distribution | Direct online sales |

**Principle:** Build the foundation once; sell individual products repeatedly.

---

## Product divisions

### DISKLORDZ MIDI

Composition, creativity, performance — CHORD ENGINE (flagship), ARP MACHINE, TRAP CHORDS, PHONK SEQUENCER.

### DISKLORDZ FX

808 SATURATOR, PHONK TAPE, DRUM CRUSH, MEMPHIS GRIT, DRIFT DESTROYER, SAMPLE DAMAGE.

### DISKLORDZ SOUND

MIDI progression libraries, preset expansions, producer toolkits, sound design collections.

---

## Milestone 001 (current)

**DiskLordz Audio Plugin Foundation** — before full CHORD ENGINE scope:

1. JUCE project + Windows VST3 compiles
2. DiskLordz interface shell + shared parameter system
3. Preset save/load
4. MIDI event generation verified in REAPER (or equivalent)
5. Automated build + test procedures

Track completion in the [audio-command PWA](../disklordz/audio-command/pwa/index.html) or `data/progress.json`.

---

## Year 1 roadmap (summary)

| Phase | Focus | Deliverable |
|-------|--------|-------------|
| M1 | Company foundation, plugin skeleton | Compiling foundation |
| M2–3 | CHORD ENGINE alpha | Functional alpha |
| M4 | Beta | Public beta candidate |
| M5 | Launch prep | Release candidate |
| M6 | Launch | First commercial product |
| M7–8 | ARP MACHINE | Second plugin |
| M9–10 | 808 SATURATOR | First audio FX |
| M11–12 | MEMPHIS GRIT | Fourth product milestone |

Detailed month goals live in `disklordz/audio-command/data/milestones-year1.json`.

---

## Business targets (internal)

| Metric | Target |
|--------|--------|
| First commercial product | 1 |
| Initial beta testers | 15–25 |
| First 90-day sales | 50 |
| Customer rating | 4.5/5 |
| Critical launch bugs | 0 |
| Products in year one | 3–4 |
| Email subscribers | 250 |
| Refund rate | Under 5% |

---

## Pricing hypotheses (validate in beta)

| Product | Launch | Regular |
|---------|--------|---------|
| CHORD ENGINE | $29 | $49 |
| ARP MACHINE | $19 | $39 |
| 808 SATURATOR | $19 | $39 |
| MEMPHIS GRIT | $19 | $39 |
| Complete MIDI bundle | $49 | $79 |
| Complete producer bundle | $99 | $149 |

---

## Agent fleet responsibilities (Audio track)

The business copilot **coordinates**; specialists **execute**:

| Agent | Role on this plan |
|-------|-------------------|
| **audio-business-copilot** | Sprint checklist, weekly review, doc truth, route work |
| **pm-agent** | Fleet registry + governance CI |
| **ship-velocity** | WO → issue → PR for non-VST store/commerce |
| **golden-wav-qa** | Plugin CI after C++ touches |
| **license-compliance** | JUCE tier, VST SDK, third-party NOTICES |
| **competitive-intel** | Chord/MIDI competitor positioning |
| **release-notes** | Customer-facing changelog at launch |
| **marketing-glue** | Organic channel stubs |
| **social-clip-factory** | Demo clip batching |
| **pricing-experiment** | Beta willingness-to-pay |
| **analytics-interpreter** | Post-launch MRR/sales (when Stripe wired) |

Run a fleet briefing: `python3 disklordz/audio-command/scripts/fleet_briefing.py`

---

## Weekly development review

Every week the owner runs:

```bash
python3 disklordz/audio-command/scripts/weekly_review.py --write
```

Output: `disklordz/audio-command/reviews/YYYY-Www.md` (gitignored locally if preferred; template committed).

Review covers: completed checklist items, build status, open bugs, next milestone, which fleet agents to invoke.

---

## Licensing watchlist

- **JUCE Starter:** free until ~$20k product revenue — plan Indie tier before threshold.
- **VST3 SDK:** MIT (3.8+) — maintain `LICENSES/` notices.
- **DPF logic** stays JUCE-agnostic in `DPF-MIDI` / `DPF-DSP` per [DPF_TECHNICAL_ARCHITECTURE.md](DPF_TECHNICAL_ARCHITECTURE.md).

---

## Long-term vision

Recognizable DiskLordz sound and UI; competitive advantage via genre intelligence, affordable pricing, and original presets — not copied compositions.
