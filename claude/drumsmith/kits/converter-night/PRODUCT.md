# Product — Night Crawl

| Field | Value |
|-------|--------|
| **Store title** | **Night Crawl** |
| **Subtitle** | Memphis Phonk Drum Kit |
| **SKU / product code** | CNV (wav prefixes `CNV_*`; repo path `kits/converter-night/`) |
| **Category** | Drum sample kit (oneshots + loops) |
| **Promise** | Original 90s Memphis / phonk drums — rate-dropped subs, converter snap, tape-cutting hats |
| **Format** | 44.1 kHz · PCM16 · mono WAV |
| **Hit count** | 512 core + competitive extras (parallel RAW/TAPE/LOFI + matched mini-kits) |
| **Street price** | HITL (target band $24–$34 drums-only) |
| **release_ready** | **false** until ear / price / cover / legal |

## Locked name decision

**Night Crawl** is the commercial store title (replaces working title “Converter Night”).  
Customers see **Night Crawl** on cover, zip, and store page. Internal SKU code stays **CNV**.

Do not use artist names, trademarked machine product names (DR-5 / SP-1200 / DR-660 dumps), or “Memphis Underground” clones on the storefront.

Internal archetype lanes (not SKU names): Converter Crawl · Bitcrush Bed · Horror Snap · Club Bounce.

## Why this name

- Commercial / memorable (mood + motion) vs technical “Converter Night”
- Signals Memphis crawl + night/phonk vibe without copying competitor titles
- Leaves RATE / converter story for the *promise* line and docs, not the title

## Package layout

```text
Night_Crawl/
  01_RAW_HARDWARE_ONESHOTS/
  02_BITCRUSHED_AND_PITCHED/
  03_LOOP_BUILDERS_AND_GROOVES/   # BPM in filenames (94 / 134)
  04_LOFI_TEXTURES_AND_NOISE/
  05_PARALLEL_PROCESS_BANKS/      # RAW / TAPE / LOFI same-hit lanes
  06_MATCHED_MINI_KITS/           # oneshots + BPM-labeled loops
  _DOCS/
    PRODUCT.txt
    ReadMe_License.txt
    Hardware_Processing_Guide.md
```

## Rebuild

```bash
cd claude/drumsmith
python3 scripts/package_converter_night.py \
  --zip /opt/cursor/artifacts/Night_Crawl.zip
```
