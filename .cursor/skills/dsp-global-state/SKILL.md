---
name: dsp-global-state
description: Defines global context, sample rate lifecycle, program/bank index, and cross-target parameter schema (params.json, manifest alignment). Use when adding prepare/init, multi-program support, or shared metadata between VST and logue.
---

# DSP Global & Schema subagent

## Read first

`docs/dsp-architecture-specification.md` — **§6 Global layer**

## Scope

- `vst/<Plugin>/params.json` (create or update)
- `prepareToPlay` / `releaseResources` — sample rate, block size stored for Dsp
- `GlobalContext` struct in `Source/Dsp/GlobalContext.h` (recommended L1+)
- Align `manifest.json` with same logical parameter order for hardware ports
- mkII `header.c` param count and defaults from presets

## params.json schema

Follow example in spec §6.3. Required fields per parameter:

- `id`, `name`, `type` (`float` | `int` | `enum`)
- `min`, `max`, `default`
- optional: `smoothingMs`, `unit`, `targets.vst`, `targets.logue`

## Lifecycle checklist

| Event | Action |
|-------|--------|
| Constructor | Default APVTS; reset program index |
| prepareToPlay | Fill `GlobalContext`; resize delay lines |
| releaseResources | Release large buffers (not on audio thread) |
| setStateInformation | Restore program + params |

## Multi-program (optional)

- `getNumPrograms` > 1 → preset snapshots per index
- Store factory presets in `params.json` `presets` array

## Deliverables

- `params.json` committed for tier ≥ L1
- Table in README: logical ID ↔ APVTS ID ↔ logue index
- Global struct read-only on audio thread after prepare

## Do not

- Change DSP algorithms (program chain skill)
- Build UI layouts (surface chain skill)
