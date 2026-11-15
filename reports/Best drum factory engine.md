# Best drum factory engine

Question: how to build a drum factory that can ship Splice-quality kits from original synthesis.

Date: 15 November 2026. Sources were read in this session. The live engine was read in `disklordz/website/src/lib/generation/synth.ts` and `engine-render.ts`. The catalog renderer was read in `disklordz/sound-factory/scripts/render_memphis_vault.py`.

## What Splice quality means

Splice’s own pack guide (Reuben Raman, “Tips for creating your own sample pack,” Splice Blog, 20 October 2016, page still modified 7 October 2024) is the product spec, not a vibe.

- A pack is one-shots and loops meant to drop into a production. A small pack is 50–150 sounds. Extra files that are not different are not variety.
- One-shots live in instrument folders. A tuned drum is named `Pack_Kick_<key>_<number>`. The key is the key of that hit. Loops carry bpm and must cycle without a click; edges get fades.
- Stated delivery: 44.1 kHz or 48 kHz, 24-bit. No mp3. In-the-box work should start at 48 kHz, 24-bit.
- The creative rule on that page is originality. A pack that imitates the other hundred packs fails their own brief.

That article does not measure every file currently on splice.com. It is their published bar. It does not say “process a sine and export.” The sounds have to survive a drum rack: trimmed, labeled, in tune when the name says a note, and distinct from the file next to them.

DiskLordz delivery stays 44.1 kHz, 16-bit, mono, because that is the product format already decided. A 24-bit header on the current oscillator does not make it a Splice kick. If a later export is 24-bit, the WAV header has to say so. Rendering in float and dithering to 16-bit is the honest path.

Splice quality here means four things a buyer can check:

1. The hit is a drum, not a test tone.
2. Tuned kicks and 808s match the note in the filename (scientific pitch, MIDI 60 = C4).
3. The folder is 50–150 files, each one kept because it passed a gate, not because a loop ran 56 times.
4. Start, end, DC, and clip are clean enough that a sampler needs no repair.

It does not mean training on Splice files, cloning a pack, or claiming a machine sample was used.

## Why the live engine cannot get there

`renderSample` in `synth.ts` is a sketch.

- The kick is `sin(2π f(t) t)`. The instantaneous frequency of that expression is `f + t f'`, so the pitch sweep is not the envelope that was written, and the phase is not continuous. A swept drum is a phase accumulator: `phase += f(t) / sampleRate`, then `sin(2π phase)`.
- `noise()` is the product of two sines. Hats, snares, and claps built on it are AM tones. They can pass a crude high-band RMS check and still sound like a beep.
- The hat buffer is 80 ms for both closed and open. Decay does not change the length.
- The snare body is fixed at 180 Hz.
- The “creative” path in `engine-render.ts` resamples the same buffer and adds `sin(i * 0.37) * 0.04`. That is not a second voice.

The offline Memphis renderer is the right family and still short of the bar. It phase-accumulates, uses hash noise, a one-pole highpass, tanh, and edge fades. Hats are six sines plus noise, not the 808’s six squares through a bandpass. Kicks are a sine with a large pitch drop, which is the folklore model the circuit paper argues against. There is no print shared with the website, no per-file pitch check, and no step that throws a weak hit away and renders another.

## The voices

### Kick

Kurt James Werner, Jonathan S. Abel, and Julius O. Smith III, “A Physically-Informed, Circuit-Bendable, Digital Model of the Roland TR-808 Bass Drum Circuit” (DAFx-14), is the primary analysis. Peter Barata’s 1 January 2026 writeup follows that paper and the TR-808 service manual and gives the numbers a factory can use:

- The source is a bridged-T bandpass pinged by a 1 ms trigger, not a free-running sine.
- Service-manual component values land near 49 Hz (about G1). Measured machines sit around 48–56 Hz.
- Attack FM lifts the resonance toward about 130 Hz for about 6 ms. That is the knock. It is too short to hear as a melody.
- A retrigger smooths the handoff from click to body. Leakage through that path is the slow pitch sigh. The sigh and the attack jump are different circuits. Collapsing both into `pitch * (1 + 4 e^{-30t})` is the mistake in the live kick.
- Decay is feedback, about 50–800 ms, about 300 ms at the middle of the knob.
- Tone is a lowpass after the oscillator. A highpass near 6.7 Hz strips DC.

The model that already ships in products is Mutable Instruments’ analog bass drum, exposed by DaisySP `AnalogBassDrum` and SuperCollider `AnalogBassDrum`: frequency in Hz, tone (click, 0–1), decay, attack FM, self FM, accent. Self-FM is what stops the body sounding like a lab sine. Implement that parameterization. A full gen~ copy of Werner’s nodal model can wait; it will not be what makes the first saleable pack.

The long trap 808 is the same oscillator with a longer decay, an octave sub, less click, and a note label. Memphis already separates short kicks from 808s. The website only has `kick`. When `kickDecay` is long, the website voice should grow the sub and the sigh instead of looping an 0.45 s sine.

### Snare

DaisySP `AnalogSnareDrum`: root frequency, tone, decay, snappy, accent. Two body modes plus filtered noise. Body in the 150–250 Hz range for this catalog, with `snareBody` scaling the modes and `snareSnap` scaling the highpassed noise. The noise envelope is shorter than the body. A second noise burst a few milliseconds later is the wire rattle. Memphis already sketches this. The website does not.

### Hat and cymbal

Barata’s “Roland TR-808 Cymbal & Hi-Hat Synthesis” (16 October, reading the service manual) and the Tiptop Audio HATS808 manual agree on the source: six square oscillators, mixed, then bandpass. Not noise.

Factory centers Barata lists, with the caveat that oscillators 3–6 can sit 20% off on a real board and should not be integer multiples:

| Oscillator | Nominal |
|---|---|
| 1 | 800 Hz, trimmable |
| 2 | 540 Hz, trimmable |
| 3 | 522.7 Hz |
| 4 | 369.6 Hz |
| 5 | 304.4 Hz |
| 6 | 205.3 Hz |

Closed and open hats are the same bank. The VCA decay is the difference. `hatBright` moves the bandpass. `hatDecay` sets the closed time; open is several times that and the buffer has to be long enough to hold it. A little hash noise under the squares is optional color. Noise as the only source is why the current hat fails the ear.

### Clap, cowbell, rim

Clap: four filtered-noise bursts with a few milliseconds between them, then a longer tail. `clapWide` spreads the bursts. Memphis does the bursts. Add the tail and the highpass before the waveshaper.

Cowbell: two non-integer partials, about 1.5×, clipped. Memphis already does this. Keep the key label on the lower partial only.

Rim: two partials, decay under 30 ms, plus a noise tick so it is not a beep. `rimMetal` is the partial mix.

## The print

The voice is not the product. Every kept hit runs one deterministic print:

1. DC block around 7 Hz, matching the 808 output stage’s job.
2. One tone filter. Lowpass on kicks, bandpass already done inside hats.
3. `tanh` drive as a recipe number, not a random gain.
4. Trim to the envelope. Fade in over 1–2 ms, fade out over 8–32 ms.
5. Peak scale 0.89, which `encodeWav` already does. Do that once, at the end.
6. Dither only when quantizing to 16-bit.

Same print for the website and the catalog script. Two slightly different `tanh` loops will ship two different products.

## The factory

Render is not the factory. A slot renders two or three candidates from neighboring seeds. Keep the one that passes, or render again. Gates already started in `kit_qa.py` and `audio-contracts.ts`:

- No clip at the 16-bit rails, no silence, no DC, no edge click.
- No exact hash duplicate, no near-duplicate on envelope plus zero-crossing rate.
- Kick low-band ratio and knock in the first 40 ms.
- Snare has body. Hat has high-band energy.
- Add a pitch check: for any file with a note in the name, the steady-state period after 30 ms is within about 15 cents of that note. The attack FM must not be what the checker measures.
- Add a length check: an open hat is longer than the closed hat from the same seed. A long 808 is longer than a short kick.

A human still listens before a pack is for sale. The gate only stops the obvious rejects.

Pack shape, taken from the Splice guide and already matched by Memphis Vault’s count:

- About 50–80 one-shots for a volume. Memphis Vault is 56.
- Folders by instrument. Key in the name only when an oscillator was tuned.
- Loops are a separate product. They have to cycle. One-shots do not pretend to be loops.

## What stays out

- Neural one-shots and “variations” of commercial packs. The training set would be someone else’s samples, and the render would not be a deterministic seed.
- ROM dumps, record chops, and “808 samples from a unit.” Original synthesis is the claim, so the oscillator has to be the claim.
- Artist-named lanes on the public page. They read as endorsements.
- A second creative oscillator that adds a sine on top of a finished hit.
- Calling the pack 24-bit, archival, or lifted.

## Build order

1. Shared primitives in the website engine: phasor, xorshift or the Memphis hash noise, one-pole and a simple bandpass, DC block, tanh, edge fade. `renderSample(name, params)` stays.
2. Replace kick, snare, hat, clap, rim with the graphs above. Delete the creative sine layer. Creative means more drive, more snap, and a deeper sigh on the same voice.
3. Point `render_memphis_vault.py` at the same constants, or generate the catalog from the TypeScript voices so the store and the factory cannot drift.
4. Add the pitch and length tests. Run the existing Spark contract script. Listen to one kick, one 808, one snare, one clap, one closed hat, one open hat before calling the engine done.
5. Only then render the next catalog volume through the gate.

The Werner paper is the reference for the kick circuit. The DaisySP parameter set is the version that can run inside a web request. Splice’s guide is the pack around those voices: original, trimmed, labeled, 50–150 files, 44.1 or 48 kHz. That is the engine. A neural model is a different product.
