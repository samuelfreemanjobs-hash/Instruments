# DRUMSMITH v1.2 operating procedure

1. Product: Fill INTAKE.md; establish rights, genre direction, inventory and intended hardware. No device claim without evidence.
2. Scaffold: `build_recipe_kit.py` creates a manifest, one deterministic recipe per sound. The plan is 8 808 / 8 kicks / 8 snares / 6 claps / 12 hats / 8 cowbells / 8 percussion / 6 FX.
3. Render: `render_recipes.py` performs input/path/recipe/overwrite preflight before writing. Rendered WAVs and manifest have cryptographic fingerprints.
4. Verification: `verify_kit.py` checks WAV format, sample peaks, clipping, silence, duration, file hashes, pitch for 808s, recipe hashes and release declarations.
5. Diversity: `diversity_audit.py` blocks exact byte duplicates and flags similar normalized amplitude envelopes. Similarity alone is not a release gate.
6. Human listening: Audition EVERY file against musical context, monitor translation, detect clicks and unacceptable resonances. Provide signed evidence in manifest approvals.
7. FORGE: `export_forge.py` writes a JSON parameter contract for JUCE 8/C++20 implementation, not compiled code.
8. Release: Device targets remain UNVERIFIED unless actually tested. Release requires signed listening and release approvals and technical pass.

## Constraints
Offline reference DSP uses Python floats and PCM16, deterministic random seeds, mono 44.1 kHz. It makes no true-peak guarantees. Optional ffmpeg EBU R128 reports are available for the meter. Read-only CALIPER audits claims but cannot override human approval. Altering generated WAVs requires re-rendering from new recipes and updating hashes; prior manifests must be versioned, not silently overwritten.
