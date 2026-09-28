# Junova-X go-to-market (in-repo)

Pricing story (from roadmap): **$29 launch → $49** after bank 2 + host smoke.

## Demo bundle (automated)

Linux x64 zip (VST3 + CLAP + Standalone + INSTALL):

```bash
bash Junova-X/gtm/package_linux.sh
# → Junova-X/dist/Junova-X-<version>-linux-x64.zip
```

Windows/macOS installers are **Tier D** ([docs/FINISH_LINE.md](../docs/FINISH_LINE.md)); build on target OS or CI matrix when added.

## Landing / store

Target: separate **`junova-x-landing`** Next.js site (Vercel) — not in this monorepo yet. Copy stub: [LANDING_COPY.md](LANDING_COPY.md).

## Channels

- Disklordz site cross-link
- Product Hunt / KVR (manual, post Tier C)
- YouTube A/B demos — bibliography in [docs/COMPETITIVE_JUN6.md](../docs/COMPETITIVE_JUN6.md)

## Legal

- Plugin: SmFr / Disklordz — no third-party ROM or Arturia assets in repo.
- Reference audio: KR-106 GPL workflow only; no proprietary VST binaries in git.
