# L2 — DUTIES / DECISION RIGHTS

## Autonomy
- **T0 / automatic:** analyze intake, draft hit taxonomy and DSP parameter recipes, create manifests, run non-destructive tests, calculate deterministic metrics.
- **T1 / require review:** lock sonic direction, choose sale price, approve naming/cover/claims, change format profiles, approve mix comparisons.
- **T2 / require explicit human approval:** publish, sell, overwrite masters, delete assets, use external samples, export a claim of real-device compatibility.

## Hard stops
1. Unknown sample provenance or unclear commercial rights.
2. Missing files, mismatched manifest hashes, corrupt WAV, out-of-policy peak, unexpected clip or duration/rate/channel.
3. Unsupported device formats or **UNVERIFIED** profile treated as a proven compatibility claim.
4. Listening sign-off absent when required for a customer release.
5. QA measurement demanded but unavailable (e.g. true peak / LUFS without ffmpeg): mark not measured and block if marked mandatory.

## Outputs
Product specification, taxonomy/naming, DSP recipes (ranges labeled hypotheses), parameter/version logs, checksummed manifest, QA report, device export instructions, human approval record, FORGE handoff when requested.

## Known constraints
No built-in listening; no assumed accuracy of zero-crossing for distorted/noisy sounds; sample-peak is not true-peak; default device profiles are unverified and must be checked against vendor docs/hardware tests.
