# vst-factory-director — learnings (validated lessons)

Factory, plugin (APC/DPCS), design-system and JUCE work. Format and rules: [MEMORY.md](MEMORY.md).

### L-0001 · JUCE FileOutputStream appends to existing files
- **date:** 2026-11-09
- **area:** juce, testing, evidence, files
- **when:** writing renders, logs, golden files or any output that may exist from a previous run
- **lesson:** `juce::FileOutputStream` opens at the end of an existing file. Re-running a writer concatenates outputs; files still start with a valid header, so naive checks pass.
- **do:** `file.deleteFile()` before opening, or `stream.setPosition(0); stream.truncate();`. For PNGs assert exactly one IEND at EOF.
- **evidence:** PR #127 review: MeterTapEditor@1.0x.png held 5 concatenated PNGs; normal and over+bypass renders byte-identical
- **confidence:** executed
- **status:** active

### L-0002 · JUCE version is split: build against the pin
- **date:** 2026-11-09
- **area:** juce, cmake, ci, versions
- **when:** adding JUCE code to vst/factory-lib or a plugin; configuring FetchContent
- **lesson:** /JUCE_VERSION pins 7.0.12 (MyFirstPlugin, DiskLordz-Audio, MeterTap CI) while root CMake fetches 8.0.15. A cached build/_deps/juce-src (8.0.15) hid a JUCE-8-only API (`juce::FontOptions`) until CI built against 7.0.12. vst/JUCE is not a submodule despite build-vst.sh's message.
- **do:** Build against `/JUCE_VERSION` from a fresh FetchContent tree; guard JUCE-8 APIs with `#if JUCE_MAJOR_VERSION >= 8` in one helper; test both versions in CI.
- **evidence:** PR #126 tokens-and-render failure run 42571508857; fixed at 251f4fb8, ctest green on 7.0.12 and 8.0.15
- **confidence:** executed
- **status:** active

### L-0003 · Non-ASCII UI strings in JUCE need UTF-8 conversion
- **date:** 2026-11-09
- **area:** juce, text, ui
- **when:** any UI string containing ·, −, ∞, °, or other non-ASCII characters
- **lesson:** Passing a UTF-8 literal straight to juce::String decodes as Latin-1: "METERS · INPUT" rendered as "METERS Â· INPUT".
- **do:** `juce::String(juce::CharPointer_UTF8("…"))` or `juce::String::fromUTF8(...)`; add a render assertion for one such label.
- **evidence:** MeterTap editor render, PR #127 review
- **confidence:** executed
- **status:** active

### L-0004 · JUCE on Linux CI needs X11 dev packages and a virtual display
- **date:** 2026-11-09
- **area:** ci, linux, juce, pluginval
- **when:** writing a GitHub Actions job that builds any JUCE target or runs pluginval
- **lesson:** juceaide includes juce_gui_basics; without libxrandr-dev etc. configure fails with "X11/extensions/Xrandr.h: No such file". JUCE 8 also needs libfontconfig1-dev for freetype2 via pkg-config.
- **do:** Copy the apt list from .github/workflows/build.yml (libx11 libxrandr libxcursor libxinerama libxext libfontconfig1 libfreetype6 libasound2 libgl1-mesa libcurl4-openssl dev, xvfb) and run GUI/pluginval steps under `xvfb-run -a`.
- **evidence:** metertap.yml run 42570871955 failed, fixed run 42571324884 green; design-system 8.0.15 cell fixed at 56162c76
- **confidence:** executed
- **status:** active

### L-0005 · Tests passing is not evidence: inspect the artefacts
- **date:** 2026-11-09
- **area:** review, testing, evidence, ui
- **when:** an agent reports renders, packages or other artefacts as verified
- **lesson:** Render tests passed while the evidence was corrupt and the UI had 5 visible bugs (whole peak bar in hot colour, mojibake, empty RMS readouts, overlapping dot, wrong value format).
- **do:** Open the artefacts yourself; hash-compare files that should differ; check file structure (one PNG per file). Require each fix to ship with a test that fails on the old code.
- **evidence:** PR #127 review 2026-11-09 00:47
- **confidence:** executed
- **status:** active

### L-0006 · APC gate path for the parameter spec
- **date:** 2026-11-08
- **area:** apc, plugins, docs
- **when:** creating plugin Dream docs by hand
- **lesson:** tools/apc-gate.sh requires plugins/<Name>/.ideas/parameter-spec.md; a spec one level up fails the plan gate.
- **do:** Use `bin/apc template <Name> dream` (writes correct paths); run `bash tools/apc-gate.sh <Name> plan`.
- **evidence:** MeterTap plan gate failed until the spec moved (PR #122)
- **confidence:** executed
- **status:** promoted:templates/apc/README.md

### L-0007 · Check the factory import graph before running it
- **date:** 2026-11-08
- **area:** factory, python, git
- **when:** starting disklordz/vst-factory on a branch, or after merging factory changes
- **lesson:** Modules referenced by main.py/orchestrator.py (certification, portfolio) had never been committed on any branch; uvicorn fails at import.
- **do:** `python -c "import vst_factory.main"` and `pytest -q` first; `git log --all -- <path>` to see if a missing file ever existed.
- **evidence:** ModuleNotFoundError vst_factory.certification; rebuilt in PR #121
- **confidence:** executed
- **status:** promoted:docs/disklordz/sops/SOP-VF-07-factory-operations.md

### L-0008 · Standard header width sets the minimum window width
- **date:** 2026-11-08
- **area:** design-system, ui, layout
- **when:** choosing a plugin window size
- **lesson:** Glyph + product name + brand + preset browser (≥180 px) + version + bypass need ≈470 px; a 300 px meter design could not fit the standard header.
- **do:** Start layouts at ≥480 px wide when using HeaderBar (SOP-VF-02 step 5).
- **evidence:** MeterTap design-spec v1 (480 × 440)
- **confidence:** inspected
- **status:** promoted:docs/disklordz/sops/SOP-VF-02-ui-design.md

### L-0009 · Check the brand guide before choosing palettes
- **date:** 2026-11-08
- **area:** design-system, brand
- **when:** asking the operator to pick colours or building a theme
- **lesson:** docs/DISKLORDZ_BRAND_IDENTITY.md already fixed black/purple/lime/gold; the operator's new pick (steel cyan) conflicted. Resolved as brand kept + cyan reserved for meters.
- **do:** Read the brand guide first and present conflicts as a choice; encode the outcome as theme usageRestrictions so the checker enforces it.
- **evidence:** design system v0.1 decision, themes/disklordz/THEME.md
- **confidence:** executed
- **status:** active

### L-0010 · Catch spec contradictions in Dream, not Plan
- **date:** 2026-11-08
- **area:** apc, dsp, specs
- **when:** writing or reviewing a creative brief with measurable acceptance checks
- **lesson:** MeterTap's brief expected a −12 dBFS-peak sine to read −12 dB RMS (true value −15.01) and capped output at 0 dBFS while allowing +12 dB trim.
- **do:** Compute every acceptance number from the brief's own definitions during Dream; list contradictions as open issues with defaults (SOP-VF-01 step 6).
- **evidence:** MeterTap architecture.md §8 issues A and B, resolved by operator
- **confidence:** executed
- **status:** active

### L-0011 · Split agents by owned paths; fixes go to the owner
- **date:** 2026-11-09
- **area:** cloud-agents, orchestration
- **when:** running several agents in parallel, or when a consumer finds a bug in a shared component
- **lesson:** Disjoint OWN ONLY lists let 3 agents work in parallel without conflicts. The LevelMeter colour bug was found via MeterTap; fixing it in MeterTap's branch would have forked the shared component.
- **do:** Give every agent OWN ONLY / DO NOT edit lists; send shared-component fixes to the component's owner and have consumers merge the owner's branch (SOP-VF-11).
- **evidence:** PRs #124, #125, #126 built in parallel; LevelMeter fix routed to the design-system agent
- **confidence:** executed
- **status:** active

### L-0012 · Assert rendered UI text, not just pixels
- **date:** 2026-11-09
- **area:** ui, testing, juce, text, review
- **when:** a component draws values in fixed-width boxes (readouts, headers, labels) and tests only sample colours
- **lesson:** Render tests passed while RMS readouts showed '...' and a header name was clipped: colour-pixel assertions cannot see truncation or clipping.
- **do:** Assert the drawn string equals the expected text (no ellipsis) and that text bounds sit inside the component, for every theme at 1x and 2x; size boxes from the longest value (e.g. −96.0, +12.0).
- **evidence:** PR #127 review 2 @1bf4cce4: disklordz RMS '...', studio-light RMS '-1...', 'MeterTap' descender clipped
- **confidence:** inspected
- **status:** active

### L-0013 · Readout unit column starves the value when the unit is empty
- **date:** 2026-11-09
- **area:** ui, juce, design-system, readout, layout
- **when:** a ValueReadout-style component reserves space for a unit or OVER label
- **lesson:** Reserving the unit column even when the unit text is empty shrank RMS values to '...' in an 80 px well.
- **do:** Allocate unit space only when unit/OVER text is non-empty; assert valueTextFitsWithoutEllipsis() and the exact string (e.g. −10.0) per theme at 1x and 2x.
- **evidence:** PR #127 @68c7d6d4 EditorRenderTests (134 checks), over-bypass PNGs show full RMS
- **confidence:** executed
- **status:** active

### L-0014 · Header layout must use the theme productName lineHeight
- **date:** 2026-11-09
- **area:** ui, design-system, header, themes, layout
- **when:** laying out HeaderBar text rows for any theme
- **lesson:** Hard-coded row heights (22 px) clipped studio-light's productName (lineHeight 24) at the bottom; other themes looked fine.
- **do:** Allocate productName.lineHeight first, shrink the brand line into the remainder, never clip the name; assert product-name bounds inside the header for every theme.
- **evidence:** PR #127 @68c7d6d4 assertProductNameInsideHeader; studio-light @1.0x PNG
- **confidence:** executed
- **status:** active

### L-0015 · Golden baselines lock in whatever bugs exist when captured
- **date:** 2026-11-09
- **area:** testing, visual-regression, goldens, review, sequencing
- **when:** creating or updating visual/audio golden files while UI or DSP fixes are still open in other PRs
- **lesson:** First factory-gui goldens captured an uppercased preset name and a placeholder glyph that an open PR (#127) already fixed; merged as-is, the suite would protect the bugs and fail the fix.
- **do:** Review a sample of new goldens by eye before merging; merge pending fixes first, then re-baseline only the affected goldens with a REVIEW.md listing each change; never re-baseline from an unmerged branch.
- **evidence:** PR #131 HeaderBar-longest@1x goldens (INIT, square glyph) vs PR #127 renders (Init, floppy+crown)
- **confidence:** inspected
- **status:** active

### L-0016 · JUCE 8 removed Font::getStringWidthFloat
- **date:** 2026-11-09
- **area:** juce, juce8, text, api, compat
- **when:** measuring text width in factory-lib gui or plugin editors
- **lesson:** Code using juce::Font::getStringWidthFloat compiled on JUCE 7.0.12 but failed the 8.0.15 CI cell; JUCE 8 removed it.
- **do:** Measure text only through the shared helper fds::gui::stringWidth (version-guarded) instead of calling Font APIs directly; keep the 7.0.12 + 8.0.15 matrix until the repo pin moves.
- **evidence:** PR #127 68c7d6d4 design-system 8.0.15 failure; fixed in 749c83e0
- **confidence:** executed
- **status:** active

### L-0017 · Factory pytest rewrites tracked liaison-status.json
- **date:** 2026-11-09
- **area:** factory, pytest, git, tests
- **when:** running disklordz/vst-factory tests locally or in a repo-wide health script
- **lesson:** The factory test suite writes disklordz/vst-factory/data/liaison-status.json, leaving the working tree dirty after a green run.
- **do:** Short term: git checkout -- disklordz/vst-factory/data/liaison-status.json after the suite (scripts/repo_health.sh does). Proper fix: tests write to a tmp path via env override, never to tracked data.
- **evidence:** PR #130 repo_health.sh local run; git status clean only after checkout
- **confidence:** executed
- **status:** active

### L-0018 · Goldens must be authored on the CI runner, never on a Cloud/dev VM
- **date:** 2026-11-09
- **area:** ci, visual-regression, goldens, tolerances
- **when:** factory-gui visual goldens rendered on a Cloud VM fail strict compare on ubuntu-latest
- **lesson:** Loosening channel-tol / max-diff-ratio to absorb FreeType host AA (e.g. 24 / 8–10%) lets small real regressions (LED-on colour, focus ring) pass. The mismatch is environment authorship, not tolerance.
- **do:** Author and commit goldens only via design-system.yml workflow_dispatch rebaseline=true on ubuntu-latest JUCE 8.0.15; keep channel-tol 8 and max-diff-ratio 0.002; never push PNGs from a Cloud or laptop render.
- **evidence:** PR #131: CI AA fail on Cloud-authored goldens; orchestrator rejected tol 24/0.08; LED-on recolour fails at 0.2% but would pass at 8%
- **confidence:** executed
- **status:** active

### L-0019 · Cloud ghs_ cannot workflow_dispatch; use sentinel file for CI rebaseline
- **date:** 2026-11-09
- **area:** ci, github-actions, visual-regression, permissions
- **when:** triggering design-system rebaseline from a Cloud agent
- **lesson:** gh workflow run returns HTTP 403 Resource not accessible by integration for ghs_ tokens. Also GITHUB_TOKEN bot pushes do not re-trigger workflows (and may land as action_required).
- **do:** Commit tests/visual/.rebaseline-on-ci to request CI --update; after bot golden commit, push an agent empty/follow-up commit to run the verifying strict compare.
- **evidence:** PR #131: dispatch 403; sentinel run 42649831533 authored c459ade7; verifying empty commit run 42653393326 success
- **confidence:** executed
- **status:** active
