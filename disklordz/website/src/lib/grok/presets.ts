/** One-tap mobile prompts — Grok Bot runs these on its cloud VM. */

export type GrokPresetId = "qa_build" | "bug_squash" | "code_review";

export const GROK_PRESETS: Record<
  GrokPresetId,
  { label: string; description: string; buildMessage: (repoUrl: string) => string }
> = {
  qa_build: {
    label: "Run QA build",
    description: "Compile + CI smoke on main",
    buildMessage: (repoUrl) =>
      `Clone ${repoUrl}. On the default integration branch, run the project's CI harness ` +
      `(python3 vst-testing-ops/run_business.py --profile ci-verify when in the Instruments monorepo, ` +
      `else npm run build). Report type/build errors only. Do not merge.`,
  },
  bug_squash: {
    label: "Bug squash",
    description: "Triage GitHub bugs",
    buildMessage: (repoUrl) =>
      `Clone ${repoUrl}. List open GitHub issues labeled bug (newest first). Pick one tractable issue, ` +
      `branch cursor/grok-fix-<issue>, implement fix, run tests, open a draft PR, and summarize on the issue.`,
  },
  code_review: {
    label: "Code review",
    description: "Summarize latest PR diff",
    buildMessage: (repoUrl) =>
      `Clone ${repoUrl}. Find the latest open pull request, read the full diff, run applicable tests if cheap, ` +
      `and post a concise markdown review (risks, missing tests, merge recommendation).`,
  },
};
