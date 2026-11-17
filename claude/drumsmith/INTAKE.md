# INTAKE.md — Kit Intake Questionnaire
> Answer before a new kit starts. Answers populate the kit spec (W1). Missing required answers BLOCK the run.

## Minimum (required)
1. **Style target:** (e.g. drift phonk, Memphis phonk, lo-fi boom bap) ______
2. **BPM range:** ______ (loops and glide math depend on it)
3. **Sample rate and bit depth:** default 24-bit / 44.1 kHz; change only if a device needs it ______
4. **Target devices:** FL Studio, MPC 500, MPC Sample, Maschine+, MV-1, Liven Lofi12, other ______

## Full (T3 / ship-bound kits)
5. **Kit name and one-line promise:** ______
6. **How are the sounds made?** [ ] synthesized from scratch  [ ] sliced from loops  [ ] recorded  [ ] mixed
   (This fills docs/provenance/<kit>.md. "Original" is only claimed for synthesized or recorded-by-Sam sounds.)
7. **Hit-list size target:** total hits ______ (see kit-product-spec for category presets)
8. **Pitched content:** 808s and cowbells tuned to keys? [ ] yes (key list ______) [ ] no
9. **Loops included?** [ ] no  [ ] yes: BPMs ______ bars ______
10. **Sales channel(s):** Beatstars / Gumroad / Etsy / own store ______
11. **Price (Sam decides, HITL):** ______
12. **Cover art status:** ______
13. **Who confirms by ear before ship:** ______

## Hard halt triggers decided up front
| Condition | What triggers it | What DRUMSMITH does |
|-----------|------------------|---------------------|
| Peak over -1.0 dBFS | verify_kit FAIL | halt, list files, fix upstream |
| UNVERIFIED device in a "ready" claim | export plan | halt, ask Sam to confirm profile |
| "Original" with no provenance file | spec or listing copy | halt, request record |

## Sign-off
- [ ] Sam reviewed answers  Date: ______
- [ ] scripts/self_test.py exits 0
