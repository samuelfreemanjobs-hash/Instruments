---
name: mpc-sample-virtual-lab
description: Develop and test MPC Sample stock firmware mods using orrpunk-art virtual-mpc-sample (QEMU + browser panel). Firmware and factory content are BYO — never commit them.
---

# MPC Sample — virtual lab

## Authority

- Repo: [virtual-mpc-sample](https://github.com/orrpunk-art/virtual-mpc-sample) (MIT)
- SDK doc: [mpc-sample-sdk `23-autonomous-development.md`](https://github.com/ioma8/mpc-sample-sdk/blob/main/docs/23-autonomous-development.md)

## Prerequisites (operator machine)

- Docker, QEMU 9+, Python 3.9+, OpenSSH
- Firmware partitions from **your** `MPCSAMPLE-1.3.0.12-update.img` → `firmware/extracted/` (matches `inputs.sha256`)
- Optional: factory `/content` copied from **your** unit for library tests

## Bootstrap

```bash
git clone https://github.com/orrpunk-art/virtual-mpc-sample
cd virtual-mpc-sample
python3 lab.py build
python3 lab.py run          # or --headless → http://127.0.0.1:8765/
```

## Mod loop (with mpc-sample-sdk)

```bash
./build-mod.sh ...          # in mod repo
python3 lab.py preload ./build/my-mod.so
curl -s http://127.0.0.1:8765/screen.png -o /tmp/screen.png
python3 lab.py ssh
python3 lab.py autorun PKG.zip
python3 lab.py stop
```

## What the lab proves

Hooks, UI, Extract, drawing, audio plumbing, autorun packages — **behavioral** parity with stock app.

## What it does not replace

Final audio latency, codec, and panel MCU timing on physical AC50.

## Do not

- Commit firmware blobs or factory library to Instruments
- Claim hardware verified from emulator screenshots alone

## In-repo

- Agent routes here via `mpc_sample_coder.py --route "qemu virtual lab"`
- No bundled firmware in this monorepo