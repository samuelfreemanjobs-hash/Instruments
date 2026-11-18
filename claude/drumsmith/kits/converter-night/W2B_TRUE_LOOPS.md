# Converter Night — true 2-bar loops

**Script:** `scripts/render_loop_from_blueprint.py`  
**Grid:** blueprint `samples_per_bar` = `loop_samples(1, bpm, 44100)`  
**File length:** `loop_samples(bars, bpm, 44100)` (may differ ±1 from `bars * samples_per_bar`)  
**Master offset:** `--variant` cycles kit oneshots so 18×18 packs stay unique

| File pattern | BPM | Frames |
|--------------|----:|-------:|
| `CNV_03_LOOP_Rhythm_94_0{1..09}.wav` | 94 | 225191 |
| `CNV_03_LOOP_Rhythm_134_{10..18}.wav` | 134 | 157970 |
| `CNV_03_TOP_Hats_94_0{1..09}.wav` | 94 | 225191 |
| `CNV_03_TOP_Hats_134_{10..18}.wav` | 134 | 157970 |

**36** loops installed into storefront `03_…` for the 384-hit SKU.  

**DR-660 technique set** (original blueprints `loops/cnv-dr660-*.json`) replaces rhythm slots  
`94_01..03` and `134_10..12` — pitched-sub/cowbell language per `recipes/DR660_LOOPS.md`.  
True Boss DR-660 WAV still **not** in uploads (`references/dr660/STAGED.md`).

Still `release_ready=false` until HITL.
