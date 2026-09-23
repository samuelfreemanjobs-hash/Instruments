# logue oscillator ideas backlog (keep & revisit)

**Owner lane:** NTS-1 mkII custom oscillators · **Agent:** [Korg Logue Agent](logue-mkii-oscillator-agent.md) (logue SDK mkII custom oscillator agent).

This file is the **living idea list** alongside the locked [collection roadmap](logue-oscillator-collection-roadmap.md). The roadmap is phase order and spec; **this backlog** holds brainstorms, genre targets, and items waiting for PM approval.

**Do not delete rows** — set status to `parked`, `approved`, or `shipped` and link the slug when implemented.

---

## Status legend

| Status | Meaning |
|--------|---------|
| `idea` | Captured; not scheduled |
| `planned` | Approved for production; brief pending |
| `in-progress` | Agent branch / PR open |
| `shipped` | `PORT_COMPLETE` + eval on branch/main |
| `parked` | Keep on list; defer |

---

## PM-approved future production (planned)

| Slug (proposed) | Display | Genre / reference | DSP sketch | Params (≤10) | Status |
|-----------------|---------|-------------------|------------|--------------|--------|
| `plug_nether_pluck` | Plug / Nether Pluck | UK **Plug** / **Nether** — short pluck, sub weight, sparse | Sine+saw pluck; fast amp; optional 1-pole damp; sub octave | **Pluck**, **Decay**, **Tone**, **Sub**, **Detune**, **Glide**, **Drive**, **Level** | `shipped` |
| `drill_slide_808_tail` | Drill 808 Slide | **Drill** 808 — portamento **slide**, long **tail**, sub sustain | Port from kick808 pitch envelope + slide on note change; tail release | **Slide**, **Tail**, **Tune**, **Punch**, **Decay**, **Sub**, **Drive**, **Level** | `shipped` |

See **Production briefs** below for agent-ready detail.

---

## Weekly intake queue (agent-generated)

New rows land here after each **weekly ideas run** (GitHub issue + optional Korg Logue Agent PR). Format:

```text
| slug | one-line hook | status | source (date) |
```

<!-- WEEKLY_IDEAS_ANCHOR: do not remove; automation appends below -->

| slug | one-line hook | status | source |
|------|---------------|--------|--------|
| `broken_cassette_trap_fx` | Trap glitch cassette (modfx) | `shipped` | 2026-09-23 |
| `cardo_got_wings_spring_revfx` | Cardo drum spring rev | `shipped` | 2026-09-23 |
| `parallel_comp_fx` | Parallel compression modfx | `shipped` | 2026-09-23 |

---

## External reference repos (not our slugs)

Third-party projects for **study only** — do not implement as collection units without explicit port WO. See [logue-external-reference-repos.md](logue-external-reference-repos.md).

| Repo | Use |
|------|-----|
| [junologue-chorus](https://github.com/peterall/junologue-chorus) | Juno chorus reference |
| [cloudlogue-reverb](https://github.com/peterall/cloudlogue-reverb) | Clouds reverb reference |
| [Waves2](https://github.com/boochow/Waves2) | Wavetable / mkII WT ideas |
| [logue-fx](https://github.com/dukesrg/logue-fx) | FX header patterns |
| [tremoxd](https://github.com/ttaschke/tremoxd) | Tremolo modfx reference |
| [logue-osc](https://github.com/dukesrg/logue-osc) | WT osc + web inject reference |

---

## Team-submitted ideas (manual)

Add rows when Slack / Airtable / issue comments propose sounds:

| slug | hook | status | submitted by |
|------|------|--------|--------------|
| | | | |

---

## Production briefs

### `plug_nether_pluck`

**Goal:** One-shot and short-loop **pluck** for Plug/Nether-style production — not a full bass rig; monophonic with fast retrigger.

**Sonic targets:** Tight front; ~80–200 ms body; controlled sub under ~80 Hz; slightly dark unless **Tone** opened.

**Engine (v1):**

- Exciter: short noise burst or sharp sine attack into damped resonator (2-pole or single LP).
- Osc: sine + optional thin saw; **Sub** adds −1 oct.
- **Glide** minimal (0–30 ms) for note slides at most; not drill-long portamento.

**MIDI:** Normal pitch; velocity → level + brightness.

**Out of scope v1:** Full chord poly; chorus (use host FX).

**Acceptance:** Static eval; on hardware, 16th-note pluck pattern at 140 BPM without clicks; sub audible with **Sub** > 50%.

---

### `drill_slide_808_tail`

**Goal:** **Drill** sub/bass — 808-ish pitch drop with **slide** between held notes and a **long tail** when notes overlap or release slowly.

**Sonic targets:** Punchy attack; pitch sweep down; slide glides pitch toward new note over 20–120 ms; **Tail** extends release/decay for 808 “ring” under slides.

**Engine (v1):**

- Reuse patterns from `tr808_kick_phonk` / `tr808_drumkit_4voice` kick oscillator (sine + sweep).
- **Slide:** on `noteOn` with previous note active, ramp target frequency over slide time (not TB-303 accent chain).
- **Tail:** separate release coefficient; optional noise bleed at very low level.

**MIDI:** Monophonic legato preferred; last-note priority.

**Relationship:** Keep `tr808_drumkit_4voice` for kit drums; this unit is **808 bass line** only.

**Acceptance:** Static eval; slide audible between C1→D1 at moderate **Slide**; tail audible after note off at high **Tail**.

---

## Related

- [logue-oscillator-collection-roadmap.md](logue-oscillator-collection-roadmap.md)
- [logue-agent-pm-automation.md](logue-agent-pm-automation.md) — weekly automation + team touchpoints
- [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md)
