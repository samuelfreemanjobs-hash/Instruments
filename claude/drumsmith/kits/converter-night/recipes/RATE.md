# Bank RATE — rate-drop subs

**Maps to:** `02/Pitched_Sub_Basses`, some cowbell slides  
**RE refs:** `tr808` lofi set `[executed]`; DR-660 technique text only `[inspected]` — **partial RE / technique-led** until DR-660 WAV

## Synthesis hypothesis

1. Body: sine/tri 808-style or kick body `[A]`
2. Pitch envelope: deep dive emulating playback slowdown (−12…−24 st feel) — long attack-free drop `[A]` / research `[inspected]`
3. Length: 0.8–2.5 s; choke via decay so notes don’t muddy `[A]`
4. Drive / soft sat on velocity `[A]`
5. Optional mild bit-depth / alias coloration (not ROM) `[A]`
6. Micro-fade + DC block on write `[S]` factory practice

## Params (draft)

| Param | Range | Tag |
|-------|-------|-----|
| pitch_hz start | 80–120 → 35–55 | A |
| decay_sec | 0.6–2.2 | A |
| drive | 1.2–1.8 | A |
| key | F minor set | A |

## RE notes

- Meter 808 lofi kicks/cowbells for peak/RMS/duration before locking A→C.  
- Do not ship `references/tr808/` PCM.
