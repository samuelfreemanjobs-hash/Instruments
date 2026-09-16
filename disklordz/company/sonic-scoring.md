# Sonic Scoring System

Score **0–100** per asset or product demo against `sonic-constitution.md`.

## Rubric (weights)

| Dimension | Weight | 0 = fail | 100 = ideal |
|-----------|--------|----------|-------------|
| Character fit | 30 | Off-brand sterile | Clearly DiskLordz |
| Texture / degradation | 25 | Clean stock | Intentional digital dirt |
| Transient / punch | 20 | Weak, mushy | Heavy, punchy |
| Uniqueness | 15 | Generic | Recognizable identity |
| Usability in mix | 10 | Harsh unusable | Harsh but musical |

## Thresholds

| Score | Action |
|-------|--------|
| &lt; 50 | Reject or send back to Sound Factory |
| 50–69 | Revise; may not enter `approved` assets |
| 70–84 | Approve for library / secondary products |
| 85+ | Flag for flagship / marketing lead |

## Recording scores

- **Assets:** `sonic_score` field in Sound DNA JSON + Airtable  
- **Products:** `Releases` notes + genome `qa.sonic_score_min`  

Agents **must not** self-approve flagship scores ≥85 without Creative Director for first release in a family.
