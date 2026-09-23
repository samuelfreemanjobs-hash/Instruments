# Korg Logue Agent — PM, team integration & weekly automation

**Product name (team-facing):** **Korg Logue Agent**  
**Technical name:** logue SDK mkII custom oscillator agent (`logue-mkii-oscillator`).

This doc is the **PM add-on**: how the agent joins your weekly rhythm, how ideas enter the backlog, and what to improve **now**.

---

## Artifacts map

| Artifact | Path |
|----------|------|
| Collection phases & slugs | [logue-oscillator-collection-roadmap.md](logue-oscillator-collection-roadmap.md) |
| **Idea backlog (keep & revisit)** | [logue-oscillator-ideas-backlog.md](logue-oscillator-ideas-backlog.md) |
| Agent workflow | [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md) |
| System prompt | [.cursor/agents/logue-mkii-oscillator/SYSTEM.md](../.cursor/agents/logue-mkii-oscillator/SYSTEM.md) |
| Weekly GitHub Action | [.github/workflows/logue-weekly-ideas-intake.yml](../.github/workflows/logue-weekly-ideas-intake.yml) |
| Issue template (human ideas) | [.github/ISSUE_TEMPLATE/logue_osc_idea.yml](../.github/ISSUE_TEMPLATE/logue_osc_idea.yml) |
| Intake script | [tools/logue/weekly-ideas-intake.sh](../tools/logue/weekly-ideas-intake.sh) |

---

## Weekly automation (once per week)

### What runs automatically

Every **Monday 15:00 UTC**, GitHub Actions workflow **`logue-weekly-ideas-intake`**:

1. Opens a GitHub issue titled `[Korg Logue Agent] Weekly oscillator ideas — YYYY-MM-DD`.
2. Embeds a checklist for **3–5 new oscillator or FX ideas** (genre, slug, one-line DSP).
3. Tags **`logue-agent`** and **`pm-intake`** (create labels in GitHub if missing).
4. Posts to Slack when `SLACK_WEBHOOK_URL` or `SLACK_WEBHOOK_LOGUE_URL` is set.

### What the team does (15 min)

1. Comment on the issue with approvals (`+1 slug`) or edits.
2. Move approved slugs into **backlog** `planned` or into **roadmap** phase table.
3. Optional: trigger implementation (below).

### What Korg Logue Agent does

**Option A — Cursor Cloud (recommended)**

1. In Cursor → **Automations**, create a scheduled automation (weekly, same day/time):
   - **Repo:** Instruments
   - **Branch:** `cursor/logue-agent-interface-7bcd` or team integration branch
   - **Prompt:** use the body from [tools/logue/weekly-ideas-intake.sh](../tools/logue/weekly-ideas-intake.sh) (`--print-agent-prompt`).
2. Agent output: PR that appends rows to `docs/logue-oscillator-ideas-backlog.md` (section after `WEEKLY_IDEAS_ANCHOR`) and summarizes in the issue comment.

**Option B — Manual Cloud Agent**

After the issue is filed, run Cloud Agent with:

```text
You are the Korg Logue Agent. Read docs/logue-oscillator-ideas-backlog.md and
docs/logue-oscillator-collection-roadmap.md. Propose 3–5 NEW mkII oscillator
ideas not already listed. Append approved-style rows to the weekly intake table.
Do not implement DSP unless a slug is marked planned and assigned this sprint.
```

**Option C — repository_dispatch (Airtable / button)**

```http
POST /repos/{owner}/{repo}/dispatches
{ "event_type": "logue-weekly-ideas", "client_payload": { "source": "airtable" } }
```

Same workflow as schedule; use for ad-hoc intake.

### Work order IDs (Airtable / PM)

Align with [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md):

| WO prefix | Use |
|-----------|-----|
| `WO-LOGUE-IDEAS-YYYY-Www` | Weekly ideas intake (no code) |
| `WO-LOGUE-IMPL-<slug>` | Implement one backlog slug |
| `WO-LOGUE-FX-<slug>` | Custom FX lane |

PR titles: `WO-LOGUE-IMPL-plug_nether_pluck: mkII pluck osc`.

---

## Team interaction (Slack + GitHub)

| Event | Channel / surface | Owner |
|-------|-------------------|--------|
| Weekly issue opened | `#disklordz-dev` or dedicated `#logue-mkii` | GitHub Action |
| PR touches `src/mkii/**` | `#disklordz-ci` (existing CI workflow) | Already wired |
| Idea approved | React on weekly issue; PM updates backlog | Human |
| Agent stuck / needs preset taste | Thread on issue; `@mention` | Human |

**Slack setup (optional):**

```bash
# Reuse CI webhook or dedicated:
# GitHub repo secret: SLACK_WEBHOOK_LOGUE_URL
```

**Cursor subscriptions (long-lived Cloud Agent):** an agent can `subscribe_slack_thread` on the weekly issue thread after posting — see [subscribe skill](~/.cursor/skills-cursor/subscribe/SKILL.md). Use for “wait for team approval then implement.”

---

## Improve Korg Logue Agent **now** (prioritized)

| Priority | Improvement | Effort | Impact |
|----------|-------------|--------|--------|
| P0 | **Expand CI** `logue-mkii-eval.yml` to run static eval on all `PORT_COMPLETE` slugs (script list), not only golden | Small | Catches regressions on collection PRs |
| P0 | **Wire weekly loop** — enable workflow + one Cursor Automation using `--print-agent-prompt` | Small | Steady idea supply without waiting on you |
| P1 | **Backlog as source of truth** — agent must read `logue-oscillator-ideas-backlog.md` before proposing new slugs | Doc + SYSTEM.md | Stops duplicate ideas |
| P1 | **Slack on mkII PR** — extend `ci-slack-notify` or add path filter for `logue-mkii-eval` | Small | Team sees agent output |
| P1 | **RAG corpus** — add roadmap + backlog to `disklordz/rag` chunk list for prompt grounding | Medium | Better genre fidelity (Plug, Drill, etc.) |
| P2 | **FX lane bootstrap** — SDK dummy-delay scaffold + `build-mkii-fx.sh` | Medium | Unblocks SP-1200 / dream reverb |
| P2 | **Issue → impl dispatch** — label `logue-implement` triggers workflow that comments “start Cloud Agent with slug X” | Medium | PM-driven sprint |
| P2 | **Preset packs** — `_agent.presets` / mkII header defaults for genre units (rage, drill, plug) | Medium | Faster hardware sign-off |
| P3 | **Hardware snapshot tests** — loguecli load + MIDI clip (manual checklist in PR template) | Large | True E2E |

---

## Korg Logue Agent checklist (definition of done)

- [ ] Idea recorded in backlog or roadmap with slug
- [ ] `.agent/brief.md` + `.agent/design.md` for new units
- [ ] `osc.h` marked `PORT_COMPLETE`
- [ ] `./tools/osc-eval-mkii.sh <slug> --static-only` (or full eval with SDK)
- [ ] PR references `WO-LOGUE-*` when PM tracking applies
- [ ] Team notified (Slack or issue comment)

---

## Related

- [disklordz/automation/README.md](../disklordz/automation/README.md) — Airtable → GitHub dispatch pattern
- [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md)
