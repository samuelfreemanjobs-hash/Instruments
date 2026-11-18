# shared — learnings (validated lessons)

Format and rules: [MEMORY.md](MEMORY.md).

### L-0001 · Windows operator machine conventions
- **date:** 2026-11-08
- **area:** windows, powershell, git, build
- **when:** any agent working on the operator's Windows laptop (C:\dev\Instruments)
- **lesson:** PowerShell ignores bash syntax: `export X=..` and `\` line continuations fail. Other agents share C:\dev\Instruments; switching branches there breaks them. Building the root project takes 30+ minutes.
- **do:** `$env:X = 'value'` (lasts for that window only); run repo `.sh` with `bash ./path.sh` (Git Bash); `git worktree add C:\dev\Instruments-<task> -b cursor/<task> origin/<base>`; build one CMake target. See SOP-VF-12.
- **evidence:** 2026-11-08 factory session: user pasted bash exports into PowerShell; worktrees used for every task
- **confidence:** executed
- **status:** active

### L-0002 · Local gh token cannot push workflow files
- **date:** 2026-11-09
- **area:** github, ci, permissions
- **when:** changing anything under .github/workflows/ from the operator's Windows machine
- **lesson:** Push is rejected: "refusing to allow an OAuth App to create or update workflow ... without workflow scope". Cloud agents can push workflow files.
- **do:** Send the exact workflow change to the Cloud agent that owns the branch (Task resume), or ask the operator to refresh gh auth with the workflow scope. Do not retry the push.
- **evidence:** metertap.yml X11 fix rejected locally; pushed by Cloud agent, run 42571324884 green
- **confidence:** executed
- **status:** active

### L-0003 · Merging PRs requires explicit operator approval
- **date:** 2026-11-09
- **area:** git, github, approvals
- **when:** any agent wants to merge a PR (even after the operator said "continue autonomously")
- **lesson:** Auto-review blocks autonomous `gh pr merge` as a shared-state write. Broad autonomy is not merge approval.
- **do:** Get CI green, then ask with the exact PR list, targets and order. Merge with `gh pr merge N --merge --match-head-commit <sha>` so nothing unreviewed sneaks in.
- **evidence:** merge script blocked 2026-11-09 00:12; operator then approved "Merge 123-125"
- **confidence:** executed
- **status:** active

### L-0004 · Shell redirects corrupt binary files on Windows
- **date:** 2026-11-09
- **area:** windows, git, binary, evidence
- **when:** extracting a binary file (PNG, zip, wav) from git or a command with `>` in PowerShell or cmd
- **lesson:** Newline translation in the redirect produces an undecodable file, which looks like a broken artefact when it isn't.
- **do:** Write bytes from Python: `subprocess.run(["git","show",f"{ref}:{path}"],capture_output=True).stdout` then `Path(...).write_bytes(...)`.
- **evidence:** MeterTap renders unreadable via cmd redirect; valid PNG headers via Python
- **confidence:** executed
- **status:** active

### L-0005 · Never set placeholder secrets
- **date:** 2026-11-08
- **area:** secrets, slack, config
- **when:** a user pastes example config such as `.../services/...` or `<your real ... URL>`
- **lesson:** Setting the placeholder makes every integration call fail; leaving it unset makes integrations no-op cleanly (factory Slack returns a preview).
- **do:** Leave unset; tell the user how to set the real value in their own shell; never ask them to paste a secret into chat.
- **evidence:** VST Factory Slack setup; real webhook set by operator, test returned sent: True
- **confidence:** executed
- **status:** active

### L-0006 · Work that only exists on another machine is lost
- **date:** 2026-11-08
- **area:** cloud-agents, git, bundles
- **when:** a plan depends on a git bundle, local clone or file produced on a Cloud agent VM or another computer
- **lesson:** Fresh Cloud agents start from GitHub and cannot see another VM's files; an expired agent cannot be messaged from local tools.
- **do:** Agents push branches before finishing. If an artefact is missing, rebuild from the spec on a new branch rather than block (SOP-VF-11 step 8).
- **evidence:** metertap-m1-cf8c.bundle never found; missing factory modules rebuilt in PR #121
- **confidence:** executed
- **status:** active

### L-0007 · pluginval download flakes in CI
- **date:** 2026-11-09
- **area:** ci, pluginval, flaky
- **when:** cmake or linux-vst3 jobs fail in "Cache pluginval binary" / "Validate VST3 plugins" with HTTP 504
- **lesson:** Upstream download timeout, not a code failure; it recurred on 3 PRs in one night.
- **do:** `gh run rerun <run-id> --failed`. Candidate promotion: cache pluginval in CI.
- **evidence:** PR #122 and #124 runs 42569578862, 42571109192 (HTTP Error 504)
- **confidence:** executed
- **status:** active

### L-0008 · Stacked PRs after a base merges
- **date:** 2026-11-09
- **area:** git, github, prs
- **when:** a PR's base branch was just merged, or main changed under an open PR
- **lesson:** Dependents keep pointing at the old base; `mergeable` reads UNKNOWN for a few seconds after the base moves.
- **do:** `gh pr edit N --base <new-base>`; wait ~15 s and re-read `mergeable` before merging; never guess.
- **evidence:** #127 retargeted; #124 mergeable UNKNOWN right after #123 merged, MERGEABLE 15 s later
- **confidence:** executed
- **status:** active

### L-0009 · Path-filtered workflows leave contract checks unrun
- **date:** 2026-11-09
- **area:** ci, github-actions, workflows, coverage
- **when:** adding a check that should guard the whole repo
- **lesson:** Checks wired only into path-filtered workflows (e.g. design-system.yml) never ran for changes elsewhere; templates, doc links, memory and factory tests had no CI at all before repo-health.
- **do:** Put repo-wide contracts in .github/workflows/repo-health.yml (always-on, continue-on-error steps + summary that fails the job); mirror locally with bash scripts/repo_health.sh.
- **evidence:** PR #130 run 42580463051 (9 steps, 22 s)
- **confidence:** executed
- **status:** active

### L-0010 · DRUMSMITH lives under claude/drumsmith
- **date:** 2026-11-17
- **area:** drumsmith,agents
- **when:** Landing drum kit agent in Instruments monorepo
- **lesson:** Package path is claude/drumsmith with self_test gate; Tail Light Static intake at INTAKE.md + kits/tail-light-static/kit-spec.md
- **do:** cd claude/drumsmith && python3 scripts/self_test.py
- **evidence:** cursor/drumsmith-tail-light-static-58c6 self_test 19/19
- **confidence:** executed
- **status:** active

### L-0011 · CNV kit hit tables: re-sum role counts to locked SKU size
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** Implementing Converter Night W1 from a plan whose role rows summed to 98 while Total said 88
- **lesson:** Always re-sum commercial kit hit-mix tables before locking kit-spec; trim secondary roles to honor the SKU total.
- **do:** Verify sum(counts)==SKU_total in kit-spec; document any trim vs plan draft.
- **evidence:** kit-spec.md count check 12+12+4+10+6+8+6+12+6+4+4+4=88; references/INVENTORY.md 26 unique refs staged
- **confidence:** inspected
- **status:** active

### L-0012 · CNV W2: converter-night scaffold plan + native_factory memphis-tape
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** W2 render after W1 design
- **lesson:** Add a named PLANS entry in scaffold_kit for SKU hit mixes; native_factory out path must be empty; keep release_ready false after technical PASS.
- **do:** Use --plan converter-night --genre memphis-tape --sku CNV; pack storefront from kinds; commit notes not WAVs.
- **evidence:** native_factory log technical_qa PASS diversity PASS samples 88; CALIPER_NOTES BLOCKED/UNVERIFIED
- **confidence:** executed
- **status:** active

### L-0013 · DRUMSMITH factory ceiling raised to 512 for 384-hit SKUs
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** Operator chose 384 Converter Night after 128 max
- **lesson:** scaffold_kit/build_genre_kit count ceiling is 512; CNV plan sums must equal SKU; recipe variant+cycle salt needed for SHA uniqueness past 16
- **do:** Update CONVERTER_NIGHT_PLAN + recipe_engine salts; native_factory out must be empty; verify masters-only tree (listen-web/refs fail inventory)
- **evidence:** native_factory /tmp/CNV_W2_384 technical_qa PASS diversity PASS samples 384
- **confidence:** executed
- **status:** active

### L-0014 · CNV: DR-660 zip must be re-attached; uploads had DR-5/55 only
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** user claims DR-660 zip but Cloud uploads lack Boss_DR-660*
- **lesson:** Inventory /home/ubuntu/.cursor/projects/*/uploads for Boss_DR-660 before staging; DR-5/DR-55 are not substitutes. Document STAGED.md NOT STAGED and ask re-attach.
- **do:** Refuse to mark dr660 staged; keep RATE technique-led with TR-808+DR-5 proxies; list exact upload filenames in IMPROVEMENTS.md.
- **evidence:** uploads listing 2026-11-17: Boss_DR-5*, Boss_DR-55 only; find *660* empty of zips
- **confidence:** executed
- **status:** active

### L-0015 · CNV competitor: Loaded Samples MU V1=DR5 parallel banks; V2=SP1200+BPM loops
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** gap-fill research without competitor WAVs
- **lesson:** MU V1 sells DR5 story + original/tape/lofi parallel banks + instrument oneshots; V2 sells SP1200 + 23 BPM loops. Biggest CNV product gap is parallel process banks and optional instruments, not FX count.
- **do:** Write public-page analysis under kits/converter-night/docs/COMPETITOR_LOADED_SAMPLES.md; do not ship their PCM; prioritize RAW/TAPE/LOFI lanes and BPM filenames.
- **evidence:** loadedsamples.com product scrapes 2026-11-17: V1 220+86+39+82; V2 357+156+141+34+23
- **confidence:** inspected
- **status:** active

### L-0016 · Web Audio lookahead Stop must cancel sources
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** DR-660 Web Audio lab transport Stop
- **lesson:** Lookahead sequencers must skip expired nextNoteTime slots and stopAll tracked AudioScheduledSourceNodes on Stop; clearing setInterval alone leaves queued sources ringing.
- **do:** Export catchUpExpiredSteps; pass onStopAudio into DR660Sequencer; track sources in DrumEngine._active.
- **evidence:** PR #165 Codex P2 + commit 33e72745; node --test 10/10
- **confidence:** executed
- **status:** active

### L-0017 · Elite phonk kit: parallel RAW/TAPE/LOFI + BPM mini-kits
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** Competitive deep-research for commercial phonk kit
- **lesson:** Compete on parallel process banks and matched oneshot+BPM-loop mini-kits; own original converter-crawl wedge; never clone DR-5/SP authenticity claims.
- **do:** Use build_elite_phonk_kit.py; keep release_ready false until HITL; price band $20-34 drums-only.
- **evidence:** RESEARCH_ELITE_PHONK_KIT.md; Firecrawl MU V1/Loopmasters Drift/LANDR 2026-11-17
- **confidence:** inspected
- **status:** active

### L-0018 · Black Box kits must ship CNV-style process banks + guide
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** User feedback that BB96 example lacked FX/pitch/cassette and zip instructions
- **lesson:** Commercial drumsmith zips need 01 cores + bitcrushed/pitched + cassette + parallel RAW/TAPE/LOFI + Hardware_Processing_Guide in _DOCS; synthesis-only grit is not enough.
- **do:** Use scripts/black_box/process_fx.py + package_bb96.py process stages; keep release_ready false until HITL.
- **evidence:** Hardware_Processing_Guide.md; package_bb96.py counts; user 2026-11-17
- **confidence:** executed
- **status:** active

### L-0019 · Black Box Collection needs all era kits not just flagship
- **date:** 2026-11-17
- **area:** drumsmith
- **when:** User asked review everything do not skip
- **lesson:** Franchise delivery is incomplete if only BB96 ships; BB93/98/2000 + Dirty Circuits need generate+package+guide in the same change.
- **do:** Run scripts/black_box/build_all_collection.py; keep profiles locked to SONIC_DNA counts.
- **evidence:** build_all_collection.py artifacts; test_black_box_collection.py 2026-11-17
- **confidence:** executed
- **status:** active

### L-0020 · Black Box: voice profiles + crc32 seeds + Splice 16-bit gap
- **date:** 2026-11-17
- **area:** black-box
- **when:** thermo-nuclear refactor + Splice audit
- **lesson:** Era personality belongs in profile voice JSON; use zlib.crc32 sku seeds; mini-kits copy masters; Dirty Circuits mode_fx is data. Splice prefers 24-bit — BB PCM16 is intentional aesthetic gap, not a silent pass.
- **do:** Keep generate_era free of char==; run splice_grade_audit.py before HITL; never set release_ready without human listen.
- **evidence:** 9/9 unittest OK; Splice audit A- PASS_TECHNICAL with PCM16 warn on BB93/96/98/00
- **confidence:** executed
- **status:** active

### L-0021 · BB96 808 amp_decay must be wired through generate_era
- **date:** 2026-11-17
- **area:** claude/drumsmith/black-box
- **when:** 808s sounded soft and gated despite profile amp_decay_kick
- **lesson:** phonk_kick uses amp_decay for the envelope; generate_era previously omitted it (default 0.14) and used short kick_duration for 808s
- **do:** Set engine.808_duration + amp_decay_808 + voice.808_click; pass amp_decay/click/seed; use voices.py diversify recipes; filter loop JSONs that lack bpm/lanes
- **evidence:** tests.test_black_box_96.test_808_hard_hit_and_long_decay; Thick 1.65s Crawl 2.39s tail_rms>0.13
- **confidence:** executed
- **status:** active

### L-0022 · Black Box quality: era recipe_sets + layered + pack lanes
- **date:** 2026-11-18
- **area:** claude/drumsmith/black-box
- **when:** Implementing diversify/layered/loops/desk/curation/calibration
- **lesson:** Era distinctness needs recipe_set tables not only engine knobs; layered_snare must avoid stacking unstable BP renders; package hero curation + 24-bit studio lane closes Splice gap without abandoning PCM16 machine cores
- **do:** Use voices.RECIPE_SETS + layered.py; package_era builds desk/studio/stereo/velocity/construction/starter; run reference_calibrate.py --all after regen
- **evidence:** 15/15 unittest OK; calibrate bb93/96/98/00 all A after BB00 punch bump
- **confidence:** executed
- **status:** active
