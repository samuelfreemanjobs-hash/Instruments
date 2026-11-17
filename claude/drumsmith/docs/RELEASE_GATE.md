# DRUMSMITH v1.1 — release gate SOP

1. Run W1 spec from INTAKE.md and inspect the manifest hit inventory; the Tail Light Static draft contains 64 one-shots.
2. Render W2 experimental originals; reference synthesis is NOT commercial mastering.
3. Run W3 QA with `python scripts/verify_kit.py --kit PATH --manifest PATH/manifest.json --json PATH/qa-report.json`. Optional `--ffmpeg` adds EBU R128 measurements; absence does not imply measurements were made.
4. Assign pitched targets only to stable, genuinely pitched material. The dependency-free autocorrelation + target-centered spectral check is a screening aid. Its PASS is not a certified pitch measurement; INCONCLUSIVE creates a warning and must be independently reviewed before labeling a note.
5. Manually audition each asset in intended context. The actual reviewer records reviewer name, YYYY-MM-DD date, and link/path to a review log in `approvals.listening`, and sets `human_listening_approved` true. Do not fabricate this record.
6. Independently authorize release with reviewer, date and evidence in `approvals.release`, and set `release_approved` true. An AI agent cannot self-authorize.
7. Declare device targets requiring evidence in `required_device_profiles` and mark `device_profiles[target]` as TESTED only after running an actual hardware/DAW compatibility test. Generic PCM compatibility is not evidence of an MPC500 or Maschine+ import test.
8. Release only when report `technical_status == PASS` AND `release_ready == true`, provenance, legal/license, product copy and channel assets have been reviewed. Keep signed manifest and report together.

## Limitations
- The verifier currently supports mono/stereo PCM16 WAV for measurements; pitch check requires mono PCM16.
- SHA-256 verifies that WAV bytes match the manifest, but a manifest can be re-written. Human release evidence needs trustworthy external storage/signatures in a real commerce pipeline.
- FFmpeg loudness on short drum one-shots can be misleading. Sample peak is not true peak. Never call the audio 'mastered' based solely on these checks.
- CALIPER remains a read-only agent reviewing supplied artifacts; it cannot actually listen to audio or prove device support.
- `--skip-pitch` is a debug/reference bypass. Do not use it for pitched commercial labeling.
