# Workbench ↔ agent loop

Human ear feedback is mandatory for W5 and for any commercial gate. Agents do not listen.

## Path

1. **Render** (new dir only):

```bash
cd claude/drumsmith
# Python genre factory, or:
cmake -S native -B native/build -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_BUILD_TYPE=Release
cmake --build native/build -j
python3 scripts/native_factory.py --out /tmp/MyKit --renderer native/build/drumsmith-render \
  --genre drift-phonk --count 64 --sku TLS --plan tail-light-static
```

2. **Technical QA** — `verify_kit.py` + `diversity_audit.py` → then CALIPER (`docs/CALIPER_CURSOR.md`).

3. **Listen** — one of:
   - `python3 scripts/kit_browser.py --kit <KIT> --port 8765` (bind `127.0.0.1`)
   - Open `workbench/DRUMSMITH_Workbench.html` locally and point at kit masters

4. **Write notes** (human or pasted by Sam) to:

```text
<KIT>/docs/listening-notes.md
```

Use severity: blocker / major / minor; name the file id; one change request per W5 pass when possible.

5. **W5 IMPROVE** — agent adjusts recipe/DSP for that hit only → re-render → re-VERIFY → optional second listen.

6. **Memory** — if a non-obvious failure/fix was learned:

```bash
python3 scripts/agent_memory.py add --agent drumsmith --kind learning ...
python3 scripts/agent_memory.py check
```

## Notes template

```markdown
# Listening notes — <KIT> — <date>
Reviewer: <name>

| File | Severity | Note | Requested change |
|------|----------|------|------------------|
| … | minor | … | … |

Session result: continue agent-demo | ship-candidate HITL
```
