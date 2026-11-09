# Operating modes

| Mode | UI | Autonomy |
|------|-----|----------|
| `beginner` | Hides raw C++; dashboard + presets | Approvals on every major gate |
| `collaborative` | Diffs, approvals banner, module picks | Default; agents run until gates |
| `engineering` | Full Monaco/code studio | Same gates; release still requires certify |

Encode on each `factory-spec/v2` as `autonomy_mode`. HTTP: `GET /api/projects/{id}/approvals`.

Gates: `approve_architecture`, `approve_dsp_promotion`, `approve_gui`, `approve_release`.
