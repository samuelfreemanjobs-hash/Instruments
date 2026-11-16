---
name: plugin-licensing
description: Wires offline license gating that never blocks the audio thread. Use for activation, Keygen, Lemon Squeezy, Gumroad, or machine limits.
---

# Plugin licensing

Use `disklordz/drum-engine/include/LicenseManager.h`.

- Call `allowsProcessing()` from `processBlock`. It is an atomic load.
- Call `requestActivation()` from a button or from startup on a non-audio thread.
- Pass a `juce::File` under the user's application-data directory in the product. Tests use a temp file.
- Leave the endpoint empty until the user names the license server. Do not embed a URL with a secret.
- Offline grace is 14 days. See `licensing.mdc` for webhook signature and idempotency rules.
- Redact any key in logs to four plus four characters.

The drum exporter does not check a license. A commercial plugin does, behind its own CMake flag, after the product design says so.
