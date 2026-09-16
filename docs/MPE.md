# MIDI expression (per-voice)

JD Upgraded routes performance controllers **per active voice** (note + channel), suitable for MPE-style masters (one pitch bend and pressure stream per member channel).

| MIDI | Behavior |
|------|----------|
| Pitch wheel | Applied only to voices on the **same MIDI channel** as the message |
| Channel pressure | All sounding voices on that channel |
| Poly aftertouch | Voice matching **note and channel** |
| Pitch bend range | APVTS `pitchBendRange` (semitones, default 2) |
| Expression depth | APVTS `expressionDepth` scales pressure → filter cutoff |

Global “mono” bend on channel 1 affects only notes that were started on channel 1.
