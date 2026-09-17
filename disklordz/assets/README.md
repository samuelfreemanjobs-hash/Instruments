# Asset storage layout

```text
assets/
  raw/           # ingest only; not for ship
  processing/    # in-flight
  approved/      # gate-passed audio
  registry/      # index by sha256 → asset id
  provenance/    # legal docs per batch
```

Canonical long-term library may also live in `sample-library/` at repo root when created.

Every approved file: **Sound DNA** + **sha256** in `registry/`.
