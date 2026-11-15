# shared — incidents (dated problems and fixes)

Format and rules: [MEMORY.md](MEMORY.md).


### I-0001 · pluginval GitHub release download 504 outlasted 5 retries
- **date:** 2026-11-09
- **area:** ci, pluginval, flaky, github
- **when:** MeterTap verify on PR #127 @749c83e0, 2026-11-09 07:52 UTC
- **lesson:** Five exponential-backoff retries (2-32 s) all got HTTP 504 from GitHub release downloads; retries alone do not make CI reliable during a CDN outage.
- **do:** Cache the pluginval binary in actions/cache (PR #132) and rerun with 'gh run rerun <id> --failed'; treat download failure as infrastructure, not plugin validation.
- **evidence:** run 42581473079 job 129040956468: attempts 1/5..5/5 failed; twin verify job passed
- **confidence:** executed
- **status:** active
