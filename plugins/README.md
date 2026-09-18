# APC plugin workspaces

Each folder here is **one plugin project** (Noizefield APC **Dream → Ship** state).

```
plugins/MyPlugin/
  status.json           # phase + validation flags
  .ideas/
    creative-brief.md
    parameter-spec.md
    architecture.md     # after /apc-plan
    design-spec.md      # after /apc-design
    reference-analysis.md
  references/           # WAV/MP3 for dream phase (Part 2 walkthrough)
```

**Implementation code** lives in `vst/MyPlugin/` (created in `/apc-impl`).

Start: `/apc-dream MyPlugin` or `bin/apc init MyPlugin`
