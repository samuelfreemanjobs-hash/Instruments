# Portable DSP (`shared/dsp/`)

Float-only, **no JUCE**, **no logue SDK** headers.

Used by:

- VST program chain (`vst/<Plugin>/Source/Dsp/`)
- logue ports (q31/float glue at unit boundary)

See `docs/dsp-architecture-specification.md` §3.4.

Add modules here as `.h` (header-only) or `.cpp` compiled into VST targets only.
