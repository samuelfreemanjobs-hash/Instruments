# VST Factory — commercial readiness & security

## Hard gates before `approve_release`

Defined in `testing/certification-v1.json` and evaluated by `vst_factory/certification.py`:

1. Linux (or CI) build succeeded.
2. VST3 artefact present under `vst/<Name>/build`.
3. Factory QA profile passed (`pluginval` / business runner as configured).
4. Factory-audio check passed or explicitly skip-ok.

## Human gates

| Gate | When | Blocks |
|------|------|--------|
| `approve_architecture` | After intake / dream | Scaffold |
| `approve_gui` | After design verified | Impl completion acceptance |
| `approve_release` | After certify passed | Ship |
| `approve_dsp_promotion` | DSP research | Promoting unverified algorithms |

## Security

- Never commit Slack webhooks, license keys, or `.env` secrets.
- `VST_FACTORY_SLACK_WEBHOOK_URL` optional; unset → notify no-op.
- Do not copy `.vst3` into system plugin folders without human confirmation.
- Public HTTP surface binds to loopback by default (`127.0.0.1:8790`).

## Release checklist

1. Certify passed + `approve_release` approved.
2. `POST /api/projects/{id}/ship` → zip under `release/<Name>/`.
3. Windows DAW smoke per `testing/daw-matrix.example.json`.
4. INSTALL.txt reviewed; version bumped in package manifest.
