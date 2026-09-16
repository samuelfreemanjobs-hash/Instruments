# Product lifecycle folders

```text
products/
  concepts/       # genomes only, no build
  development/    # active compile (genome.yaml + manifest)
  qa/             # RC candidates
  released/       # pointers to tags / store ids
  archived/
```

Genome lives at `development/<product_id>/genome.yaml`.
