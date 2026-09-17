# Release engineering

```text
SOURCE → BUILD → UNIT → AUDIO → PLUGIN → INSTALL → PACKAGE → DOC → RC → HUMAN → PUBLISH
```

## Artifacts per release

- Versioned zip / installer  
- `CHANGELOG.md`  
- `LICENSE.txt` + third-party notices (VST3, JUCE)  
- SHA256 checksums file  
- `PRODUCT_INFO.json`  

## Paths

```text
release/
  templates/     # LICENSE, INSTALL, README templates
  checksums/     # generated per build (gitignored if large)
```

## Ownership

Release team role; Orchestrator assembles RC packet; Creative Director approves publish.
