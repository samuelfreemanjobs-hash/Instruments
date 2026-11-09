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

### I-0002 · pluginval actions/cache hit after first download
- **date:** 2026-11-09
- **area:** ci,pluginval,cache
- **when:** after PR #132 added actions/cache for pluginval keyed by OS+version
- **lesson:** First CI run after a cold cache still hits GitHub CDN (can 504); later runs restore build/tools/pluginval from actions/cache and skip the download.
- **do:** Keep PLUGINVAL_DOWNLOAD_FAILED exit 3 distinct from validation failure; rely on actions/cache path build/tools/pluginval keyed pluginval-${{ runner.os }}-1.0.4; do not treat a download miss as a plugin bug.
- **evidence:** PR #132: run 42581812226 cold miss + 504x4 exit 3 (~87s retries); head 9f0fe2c9 MeterTap verify 42596775619 pass in 5m56s with cache present; Build cmake 42596775543 pass
- **confidence:** executed
- **status:** active
