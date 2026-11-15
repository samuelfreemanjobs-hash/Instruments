# APC plan — ProphetMod16

**Complexity:** L3 (mod matrix + WebView + 16 voices + FX).

**Phases:** Dream ✅ → Plan ✅ → **Design next** (`/apc-design ProphetMod16`) → Impl at `ProphetMod16/` → Test (`pluginval` + `run_business.py --profile factory`) → Ship.

**Reuse:** `ProphetRev2Trap` engine idioms; do not duplicate trap presets or 8-voice limit.

**RT checklist (from JUCE8 rules):** `SmoothedValue` on cutoff/pitch/res; matrix at 16-sample control rate; `juce::Synthesiser` voice stealing; no alloc in `renderNextBlock`.