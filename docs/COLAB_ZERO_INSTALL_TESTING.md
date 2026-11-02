# Google Colab — zero-install testing

Use Colab to validate **Python factory / DSP / RAG chunking** without installing tooling on a new machine. The **Next.js SaaS** still needs Node locally or Cloud Agent; Colab is for the **audio + data** lane.

---

## What runs well in Colab

| Component | Path | Notes |
|-----------|------|--------|
| Stub kit generation | `disklordz/sound-factory/scripts/generate_stub_kits.py` | Clone repo or upload script |
| RAG chunk build | `disklordz/rag/scripts/chunk_corpus.py` | No GPU required |
| Parametric experiments | Copy `disklordz/website/src/lib/generation/` logic | Port to notebook cells |
| WAV listen | `IPython.display.Audio` | Quick ear check |

---

## Quick start notebook

Open or upload:

- [`disklordz/colab/disklordz_factory_smoke.ipynb`](../disklordz/colab/disklordz_factory_smoke.ipynb)

Cells:

1. `!git clone https://github.com/samuelfreemanjobs-hash/Instruments.git` (or mount Drive)  
2. `cd Instruments && python3 disklordz/sound-factory/scripts/generate_stub_kits.py`  
3. `python3 disklordz/rag/scripts/chunk_corpus.py`  
4. Play a generated WAV from `disklordz/website/public/samples/` if present  

---

## Secrets in Colab

Use **Colab secrets** (key icon) for:

- `OPENAI_API_KEY` — embedding tests only  
- Never commit keys; do not paste into notebook outputs  

---

## vs Cursor Cloud Agent

| Colab | Cloud Agent |
|-------|-------------|
| Zero install, shareable link | Full repo, CI, PRs |
| Good for ML/audio experiments | Good for app + deploy |
| Ephemeral runtime | Persistent environment.json |

**Workflow:** Prototype in Colab → port winning code to repo → Agent Mode PR with tests.

---

## Agent instruction snippet

When asking Cursor to use Colab:

```markdown
Add a Colab smoke cell that runs generate_stub_kits.py and asserts output WAV count > 0.
Do not require GPU. Document in COLAB_ZERO_INSTALL_TESTING.md.
```
