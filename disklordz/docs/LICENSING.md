# Licensing & Provenance — Architecture Requirements

Shipping audio products without a **license layer** is a business risk. This is enforced in software gates, not honor system.

## Plugin stack

### VST 3

- Use **VST 3 SDK 3.8+** (MIT license).  
- MIT permits commercial **binary** distribution when built against the MIT-licensed SDK ([Steinberg VST3 licensing FAQ](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Licensing.html)).  
- Include required notices in product `LICENSE.txt` / installer (per SDK guidance).  
- **Do not ship VST2.** Steinberg is not granting new VST2 licenses for new products.

### JUCE

- **JUCE Starter** (perpetual free tier with revenue ceiling—verify current terms at [juce.com/get-juce](https://juce.com/get-juce/)).  
- Track **gross revenue** attributable to JUCE-based products; escalate to Indie/Pro license before exceeding allowed limits.  
- Document decision in Airtable `Legal / Compliance` notes on Product Family.  
- **IP you own:** Sound DNA, Character Lab DSP, preset systems, UI design system, automation, brand—not the JUCE framework binary.

### Steinberg validator

- Use official **validator / test host** in CI where possible ([VST3 SDK documentation](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Index.html)) alongside pluginval.

## Sample & kit provenance

Every Sound DNA record must include `provenance`:

| Field | Required | Purpose |
|-------|----------|---------|
| `source` | yes | `original` \| `recorded` \| `licensed_library` \| `other` |
| `license` | yes | SPDX or internal enum |
| `owner` | yes | You or licensor |
| `date_created` | yes | ISO date |
| `processing` | yes | Chain of transforms |
| `derivative` | yes | Parent asset id if derived |
| `commercial_ok` | yes | boolean — **ship gate** |
| `attribution` | if needed | Credit text |
| `restrictions` | optional | Territories, max units, etc. |

### Ship gate (automated)

```text
IF any asset in compile set has commercial_ok == false
   OR license == unknown
   OR missing provenance block
THEN compiler exit code PROVENANCE_BLOCK
     PM + Creative Director notified
     DO NOT SHIP
```

**Strategy:** Favor **original generation** (Sound Factory + Character Lab) for defensible IP.

## Customer licensing (v1 — keep simple)

Phase 1–2:

- Payment → email with download link + personal use license PDF  
- No complex DRM; optional license key file for honor system  

Phase 3+:

- Plugin activation service only if revenue justifies it  
- Versioned `license_format_version` in compiler manifest  

## Product copy compliance

- Marketing describes **sonic character classes** (“12-bit style”, “sampler workflow”)—not counterfeit branding of hardware trademarks.  
- Content Factory agent runs **compliance checklist** before publish.
