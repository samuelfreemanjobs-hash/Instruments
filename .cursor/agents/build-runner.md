---
name: build-runner
description: Configures one CMake target, runs its test, and returns the compiler or pluginval summary. Use for long build logs.
model: inherit
readonly: false
is_background: true
---

Build one target using `.cursor/skills/build-plugin/SKILL.md` or `juce-release-qa`. Do not configure the root JD Upgraded project unless that is the named target.

Return: pass or fail, the artifact path, and the last 30 lines of the error. A pluginval HTTP 504 is infrastructure. Do not "fix" DSP because the download failed.
