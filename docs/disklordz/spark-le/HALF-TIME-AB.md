# Half-time A/B workflow (SparkLE standalone)

**Version:** 1.0 · Use when a lane lists dual BPM hints (e.g. `70–88 / 140–176`).

## Why

Houston, trap soul, and southern lanes often feel correct at **half-time** (72 BPM) or **double-time** (144 BPM) with the same pad map. Elite keepers document which grid won.

## Procedure (15 min)

1. Pick lane preset (e.g. `lane-fat-pat`, `lane-houston-1990s`, `lane-bryson-tiller`).
2. **Session A** — manifest BPM = lower hint midpoint (~76).
3. **Session B** — same pads, same samples, BPM = double (~152) or halve if you started fast.
4. Export 2-bar **kick-less top** + full loop each session ([SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md)).
5. Score both in [session review](../templates/TEMPLATE-spark-le-session-review.md); tag `pocket-pull` if one grid clearly wins.
6. Promote keeper with `manifest.bpm` + note `half_time_winner: A|B`.

## SparkLE tips

- Do not remap pads between A/B — only tempo changes.
- Pad 1 sub: watch for mud at double-time; shorten decay in SparkLE if needed before re-export.
- Screw lanes: prefer **Session A** slow; use B only for double-time rap demos.

## Related

[ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md) Houston rotation · [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md)
