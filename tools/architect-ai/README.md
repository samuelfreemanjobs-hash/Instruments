# ArchitectAI

Local **ArchitectAI** agent: COSI-grounded architecture mentor with Gemini (default) or OpenAI adapters.

## Setup

```bash
cd tools/architect-ai
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
cp .env.example .env   # add GEMINI_API_KEY
```

## Run

Interactive REPL:

```bash
python architect_ai/cli.py
python architect_ai/cli.py --cosi
python architect_ai/cli.py --review ../../disklordz/website --once "COSI review of this web app"
```

OpenAI:

```bash
ARCHITECTAI_PROVIDER=openai python architect_ai/cli.py
```

Print system prompt (no API):

```bash
python architect_ai/cli.py --print-system
```

## Cursor / workspace

Copy `prompts.ARCHITECTAI_SYSTEM_PROMPT` or use [.cursor/rules/architect-ai.mdc](../../.cursor/rules/architect-ai.mdc) to mirror persona in the IDE.

## LangChain (optional)

```bash
pip install langchain-google-genai langchain-core
```

Use `architect_ai.adapters.langchain_gemini.LangChainGeminiChatModel` in your own runner.

See [ARCHITECTURE.md](ARCHITECTURE.md).
