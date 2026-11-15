# Sub-agents — Linux VST3 DSP Engineer

Coordinator/worker split per Claude Code coordinator mode.

| Sub-agent | Model | Tools | Role |
|-----------|-------|-------|------|
| `linux-vst3-dsp-engineer-explore` | haiku | Read, Grep, Glob | Gather repo + API evidence only |
| `linux-vst3-dsp-engineer-verify` | inherit | Read, Bash | Run verification scripts; no writes |
| `linux-vst3-dsp-engineer-implement` | inherit | Read, Edit, Write | Apply minimal diff after coordinator plan |

## Delegation rules

1. Coordinator **never** delegates understanding — pass exact paths, env names, and API routes.
2. Messages delivered **between tool rounds** only (command queue invariant).
3. `linux-vst3-dsp-engineer-verify` must run `./tools/build-vst.sh <Name>` or `./tools/build-vst-aarch64-linux.sh <Name>` plus `verify-mpc-sample-vst3.sh` when aarch64; run `vst-testing-ops/run_business.py` when DSP C++ changed.
4. Max fan-out: 3 workers per turn unless user approves.

## Fork agents (cache-safe)

When summarizing long transcripts, use fork with **byte-identical** tool array and placeholder tool results (see `loop.md`).
