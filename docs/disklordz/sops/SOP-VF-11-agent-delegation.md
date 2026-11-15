# SOP-VF-11 — Agent delegation for factory work

## Purpose

Run several agents (local + Cloud) on one product or platform without collisions, lost work, or unverifiable claims.

## Procedure

1. **Split by owned paths**, not by topic. Each agent gets an explicit "OWN ONLY" list and a "DO NOT edit" list naming the other agents' paths. No two running agents share a path.
2. **Base branch must be on origin.** Cloud agents start from the remote; unpushed work is invisible. Push before launching.
3. **Stack, don't fork:** an agent that extends another branch opens its draft PR *against that branch*, not `main`. Merge bottom-up.
4. **Dependencies:** if B needs A's output (e.g. editor needs design-system components), give B a safe interim (generic editor) and a follow-up step after A merges, rather than letting B invent its own version.
5. **Prompt:** use [TEMPLATE-factory-cloud-agent-prompt.md](../templates/TEMPLATE-factory-cloud-agent-prompt.md): read-first list, owned paths, DPCS order, success criteria as commands, delivery rules, evidence tags.
6. **Specs frozen:** agents report spec bugs; they don't silently deviate.
7. **On completion:** pull the branch locally (worktree), rerun the success commands yourself, then mark ready / merge when CI is green.
8. **Memory ([SOP-019](SOP-019-agent-memory.md)):** every prompt includes the recall/add block; when an agent reports back, the orchestrator records review findings the agent missed (`agent_memory.py add`), and promotes repeats into this SOP or the templates.
9. **Lost artifacts:** if an agent exported a bundle or left work only on its VM, ask that agent to push its branch; if it can't, rebuild from the spec on a new branch. Never block on a file that only exists on another machine.

## Definition of done

- Every agent's PR has its success-command output, CI green, and was re-verified locally where possible.

## Related

[SOP-001 Cloud agent task](SOP-001-cloud-agent-task.md) · [SOP-VF-00](SOP-VF-00-vst-factory-master.md)
