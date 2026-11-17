# drumsmith — incidents (dated problems and fixes)

Format and rules: [MEMORY.md](MEMORY.md).


### I-0001 · Cursor skills were not loaded by agent.yaml
- **date:** 2026-11-17
- **area:** drumsmith,skills,cursor
- **when:** agents only followed package .claude/skills while monorepo .cursor/skills grew
- **lesson:** Without agent.yaml + CLAUDE.md router, Cursor skills are invisible to the agent loop.
- **do:** Use skill drumsmith router; agent.yaml v2.1 lists Cursor skills; recall --agent drumsmith.
- **evidence:** agent.yaml bump 2.1.0 and .cursor/skills/drumsmith/SKILL.md added 2026-11-17
- **confidence:** executed
- **status:** active
