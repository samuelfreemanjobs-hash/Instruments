# drumsmith — learnings (validated lessons)

Format and rules: [MEMORY.md](MEMORY.md).


### L-0001 · Cloud/Linux native drumsmith builds need g++-12
- **date:** 2026-11-17
- **area:** drumsmith,native,cmake,cloud
- **when:** building native/ or running test_v20 / native_factory on Cloud Ubuntu images
- **lesson:** Default /usr/bin/c++ links fail against libstdc++ on this image; g++-12 succeeds.
- **do:** Export CXX=g++-12; cmake -DCMAKE_CXX_COMPILER=g++-12; CI already installs g++-12.
- **evidence:** drumsmith CI + Cloud verify 2026-11-17; test_v20 46/46 with CXX=g++-12
- **confidence:** executed
- **status:** active

### L-0002 · Technical PASS is not release approval
- **date:** 2026-11-17
- **area:** drumsmith,qa,release
- **when:** verify_kit or native_factory reports technical PASS
- **lesson:** PASS covers inventory/PCM/peak/hash screens only; listening, provenance, devices, and commercial gates are separate.
- **do:** Run CALIPER (docs/CALIPER_CURSOR.md); keep release_ready=false until Sam HITL; never claim storefront-ready from scripts alone.
- **evidence:** RELEASE_GATE.md + tls-v20-native-cxx STATUS NATIVE_RENDERED_NOT_RELEASED inspected 2026-11-17
- **confidence:** executed
- **status:** active
