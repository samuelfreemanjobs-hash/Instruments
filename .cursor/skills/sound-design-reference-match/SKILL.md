---
name: sound-design-reference-match
description: "Match factory timbre using YouTube and other references: find tracks, extract analysis audio, metrics, spectral diff. Use for 'sounds like' briefs."
---

# Sound design & reference match (incl. YouTube)

## Product policy (owner-approved)

For **timbre matching**, the agent **may**:

1. **Search** for the real recording (YouTube URL, title, artist, official uploads).  
2. **Download short audio** from YouTube **for analysis only** (yt-dlp → temp file).  
3. **Listen/analyze** (metrics, spectrogram, A/B) and tune factory params — **not** redistribute the reference in customer ZIPs or git.

**Never:** commit reference audio to git; ship copyrighted refs in products; publish full rips.

**Workspace:** `disklordz/sound-factory/refs/youtube-cache/` (gitignored). Delete or rotate after job closure when practical.

## Workflow

1. **Resolve track:** `WebSearch` + user hint → pick best YouTube URL (official video / topic when possible).  
2. **Extract:** `yt-dlp -x --audio-format wav --download-sections "*0:00-1:00" -o 'refs/youtube-cache/%(id)s.%(ext)s' <url>` (adjust segment; prefer chorus/hook user names).  
3. **Analyze:** `analyze_reference.py` when shipped; interim: `ffmpeg` + scipy/LUFS + mel PNG.  
4. **Target:** map metrics → `GenerationSpec` / factory params / layer notes.  
5. **Regenerate** multisamples; `SpectralDiff` vs ref slice.  
6. **Evidence:** `reference-report.json` (url, segment, metrics — **not** the WAV in PR), spectrogram PNG, synthetic A/B only.

## Also supported

- User-dropped WAV/MP3 in `disklordz/sound-factory/refs/`  
- `videoReview` subagent when user attaches MP4

## Cloud VM deps (heavy image)

Install when implementing WO-SF-011: `ffmpeg`, `yt-dlp` (analysis lane only).

## Legal note

Owner accepts responsibility for reference use in private factory workflows; agents still **fail closed** on committing or selling reference audio.
