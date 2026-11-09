# TEMPLATE — Cloud agent prompt for VST Factory work

Use with [SOP-VF-11 Agent delegation](../sops/SOP-VF-11-agent-delegation.md). Replace every `<…>`.

```text
Repo: Instruments monorepo (samuelfreemanjobs-hash/Instruments). Base branch: <branch> (<PR link if stacked>).
Operator approval: <quote or "autonomous work approved on <date>">.

Read first: /ARCHITECTURE.md, AGENTS.md, CLAUDE.md (META: R5 executable success criteria, R6 every test
names a contract, R8 evidence tags), docs/DPCS.md, <phase SOP>, <product docs: plugins/<Name>/...>.

CONTEXT: <2-4 sentences: what exists, what phase we are in, what is frozen>.

OWN ONLY: <paths this agent may write>.
DO NOT edit: <paths other agents own right now>; specs under plugins/<Name>/.ideas/** are frozen (report spec bugs, don't deviate).

DPCS ORDER: Phase 2 headers (compile-check) -> Phase 3 one module per commit -> Phase 4 verify script exit 0.

TASKS:
1. <task with exact file paths>
2. ...

SUCCESS CRITERIA (show exact output):
- <command> -> exit 0
- <command> -> <expected text>
- Mutation check: break <mechanism>, show a test fails, restore.

DELIVERY: commit on your branch, push, open a DRAFT PR against <base> titled '<title>'.
No merge, no push to main, no force-push, no secrets.
Return: branch, PR URL, files changed +/- counts, exact outputs, spec issues found,
assumptions tagged [executed]/[inspected]/[assumed].
```

## Checklist before sending

- [ ] Base branch exists on origin (cloud agents cannot see unpushed work).
- [ ] Owned paths do not overlap with any other running agent.
- [ ] Success criteria are commands, not opinions.
- [ ] The prompt names the SOP for the phase.
