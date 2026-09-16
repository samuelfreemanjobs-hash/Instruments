# Portable DSP (`shared/dsp/`)

Float-only, **no JUCE**, **no logue SDK** — matches `docs/dsp-architecture-specification.md` §3.4.

| Module | Header | Use |
|--------|--------|-----|
| Biquad LP | `biquad.h` | Tone shaping, filters |
| ADSR | `envelope_adsr.h` | Amplitude envelopes |
| Smoothing | `parameter_smoothing.h` | De-zipper knobs |
| Phasor | `phasor.h` | Oscillator phase |

```cpp
#include "biquad.h"
#include "envelope_adsr.h"
```

In JUCE CMake:

```cmake
target_include_directories(MyPlugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)
```

logue: wrap outputs to q31 at `OSC_CYCLE` boundary.
