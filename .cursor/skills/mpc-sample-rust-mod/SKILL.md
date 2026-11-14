---
name: mpc-sample-rust-mod
description: Build Akai MPC Sample 1.3.0.12 in-process mods with mpc-sample-sdk (Rust cdylib, LD_PRELOAD, SD autorun). Use for stock-app hooks — not Melodee replacement or .xpj generation.
---

# MPC Sample — Rust mod SDK

## Authority

- Upstream: [mpc-sample-sdk](https://github.com/ioma8/mpc-sample-sdk) (MIT, Rust 1.97+)
- Example mod: [mpc-minimod](https://github.com/ioma8/mpc-minimod)
- Docs: upstream `docs/README.md` + `docs/24-safety-migration.md` (v0.2.0+ unsafe contracts)

## Minimal mod

```rust
use mpc_sample_sdk::{Mod, PadEvent, PadFlow};

#[derive(Default)]
struct MyMod;

impl Mod for MyMod {
    fn on_pad(&mut self, _event: PadEvent) -> PadFlow {
        PadFlow::Forward
    }
}

mpc_sample_sdk::export_mod!(MyMod);
```

```toml
[lib]
crate-type = ["cdylib"]

[dependencies]
mpc-sample-sdk = { git = "https://github.com/ioma8/mpc-sample-sdk", branch = "main" }
```

## Build & install

```bash
./build-mod.sh --manifest-path Cargo.toml --package my-mod --name my-mod --pack
```

- Device: `LD_PRELOAD` or SD **autorun** zip from `--pack`
- `install` refuses wrong firmware fingerprint (37 words, **1.3.0.12**)

## SDK owns

Hooks (17 sites), typed events, `platform::*` (selection, samples, WAV paths, Extract, audio taps, draw)

## Do not

- Patch stock app without reading safety migration guide
- Mix with **Melodee** autorun on same card without user choosing one stack (`mpc-sample-sd-autorun`)

## Verify

- Upstream `cargo test` / clippy in sdk repo
- **`mpc-sample-virtual-lab`**: `lab.py preload build/my-mod.so`