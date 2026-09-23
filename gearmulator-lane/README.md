# Gearmulator (submodule copy)

Default upstream: https://github.com/dsp56300/gearmulator.git  
Pinned path: **`gearmulator-lane/gearmulator/`** (git submodule).

**Your fork:** create on GitHub, then [docs/GEARMULATOR_FORK.md](../docs/GEARMULATOR_FORK.md) and `./scripts/gearmulator/repoint-submodule-to-fork.sh <your-fork-url>`.

## Get the copy

From the Instruments repo root:

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator
```

Fresh clone of Instruments with submodules:

```bash
git clone --recurse-submodules https://github.com/samuelfreemanjobs-hash/Instruments.git
```

## Docker multisample container

On a machine with Docker (not Cloud Agent pods):

```bash
./scripts/gearmulator/docker-multisample.sh build
./scripts/gearmulator/docker-multisample.sh run --plugin /plugins/YourPlugin.vst3 --name session1
```

See [docker/](docker/) and [docs/GEARMULATOR_MULTISAMPLING.md](../docs/GEARMULATOR_MULTISAMPLING.md).

## Build smoke test

```bash
./scripts/gearmulator/build-je8086-console.sh
```

Full architecture and policy: [ARCHITECTURE.md](ARCHITECTURE.md) · [docs/GEARMULATOR_LANE.md](../docs/GEARMULATOR_LANE.md).
