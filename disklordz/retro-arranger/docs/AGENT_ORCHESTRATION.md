# Master prompt → code map

The production master prompt for the **1980s Retro-Modern MIDI Arranger** maps to this package as follows.

| Prompt agent | Module |
|--------------|--------|
| `arranger_agent` | `retro_arranger/agents/arranger_agent.py` |
| `keys_agent` | `retro_arranger/agents/keys_agent.py` |
| `bass_agent` | `retro_arranger/agents/bass_agent.py` |
| `drum_agent` | `retro_arranger/agents/drum_agent.py` |
| `lead_agent` | `retro_arranger/agents/lead_agent.py` |
| `guitar_agent` | `retro_arranger/agents/guitar_agent.py` |
| Pipeline orchestration | `retro_arranger/pipeline.py` |
| MIDI export | `retro_arranger/midi_writer.py` |
| Web + DAW DnD | `static/index.html` (`DownloadURL` drag) |
| HTTP API | `app.py` |

Stem filenames match the spec: `01_Drums.mid` … `07_Lead_Solo.mid`, plus `Full_Arrangement_Multitrack.mid`.
