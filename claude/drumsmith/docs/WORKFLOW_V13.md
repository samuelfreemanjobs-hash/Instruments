# DRUMSMITH v1.3 — Production SOP

## Stage 1 — Intake / original creative direction

Select one of: `drift-phonk`, `memphis-tape`, `detroit-trap`, `southern-trap`, `cloud-rap`. Capture intended audience, exclusions, SKU, count, licensing/provenance and intended hardware outputs in `INTAKE.md`. Do not label generated content as a recreation of trademarked hardware or a producer's proprietary kit.

## Stage 2 — Plan and render

`python scripts/factory.py --out <NEW_FOLDER> --genre <GENRE> --sku DKL --count 64`

Under the hood: scaffold manifest, assign genre modifiers to original parametric v2 recipes, verify recipe hashes, preflight destinations, render PCM16 mono 44.1 kHz, hash emitted WAVs. Each recipe is reproducible from recipe parameters and seed; **Python stdlib math/rendering changes can affect floating-point quantization across platforms**, so always preserve source version and WAV hashes.

## Stage 3 — Machine QA

Run manifest/PCM check, 808 pitch estimator, clipping/peak, hashes, and exact duplicate detector. Reports are `qa-report.json` and `diversity-report.json`; the factory exits nonzero on a failed technical/diversity gate. Advisory similar-envelope flags require human review. An optional `--ffmpeg` switch exists on `verify_kit.py` for measured true-peak/LUFS; do not claim the internal sample-peak checker is certified true peak.

## Stage 4 — Local listening review

`python scripts/kit_browser.py --kit <FOLDER>` and visit `http://127.0.0.1:8765`. Browser is read-only and does not transmit WAVs outside loopback. Choose best hits, reject or modify bad ones **in a new versioned kit**, not by silently replacing hashed assets. Record dated listening approval and evidence in manifest according to `docs/RELEASE_GATE.md`.

## Stage 5 — FORGE implementation handoff

Read `forge-handoff.json` as a specification **only**. Integrate tested C++/JUCE engines separately; enforce real-time callback restrictions, denormal handling, parameter and multi-voice tests. This Python kit is not a VST3 plugin.

## Stage 6 — Device export and commercial release

Hardware-specific bit depth, filename lengths, velocity layout and storage structures remain UNVERIFIED until hardware tests are recorded. Do not claim compatibility or mark release ready based solely on technical pass. Archive manifest, QA, hashes, provenance and approvals.

## Security notes

Only run kit folders you trust. The preview server listens on `127.0.0.1` and serves manifest-listed `.wav` files with canonical-path containment. It has no write route. Close the browser server when finished. The factory refuses existing output directories and does not auto-approve release.
