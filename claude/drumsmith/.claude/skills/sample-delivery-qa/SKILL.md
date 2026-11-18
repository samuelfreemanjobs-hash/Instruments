---
name: sample-delivery-qa
description: Verify kit manifest, WAV integrity, technical metrics, provenance and approval gates prior to shipping
---
# QA procedure
1. Validate `manifest.json` schema, file IDs and safe relative paths, and inventory matches exactly.
2. Run `python scripts/verify_kit.py --kit ./my-kit --manifest ./my-kit/manifest.json --json ./my-kit/qa-report.json`.
3. Inspect per-file sample rate, PCM sample width, channels, duration, measured sample peak, RMS and clipping; confirm requested target key separately using an appropriate pitched time window.
4. If `ffmpeg` installed, run `--ffmpeg` for optional EBU R128 loudness and true-peak observations. Never substitute peak sample amplitude for true peak.
5. Log missing provenance, pitch uncertainty, export compatibility and human listen signoff as blockers when required.
6. Seek explicit release approval before packaging/publishing. QA scripts may indicate a technical PASS without conferring commercial-release approval.
