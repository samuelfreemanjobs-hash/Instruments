/** Next agent owners after factory output (see disklordz/factory/WORKFLOW.md). */

export type AgentPipelineStep = {
  agentId: string;
  role: string;
  doc: string;
};

export function agentPipelineAfterFactory(presetId: string): AgentPipelineStep[] {
  const laneAgent =
    presetId === "midnight-circuit"
      ? "artist-midnight-circuit"
      : presetId === "boulevard-86"
        ? "artist-boulevard-86"
        : presetId === "disklordz-screw"
          ? "artist-disklordz-screw"
          : presetId === "terminal-mirage"
            ? "artist-terminal-mirage"
            : "ar-lane-guardian";

  return [
    { agentId: laneAgent, role: "Lane guardian review", doc: "disklordz/ar/subagents/" },
    { agentId: "ar-kit-qc", role: "Keep/reject WAVs", doc: "disklordz/ar/QC_TRIPWIRE_SAMPLE.md" },
    { agentId: "ar-director", role: "CROW pack sign-off", doc: "docs/DISKLORDZ_AR_DEPARTMENT.md" },
    { agentId: "product-launch-manager", role: "HELM launch assets", doc: "docs/DISKLORDZ_PRODUCT_LAUNCH_MANAGER.md" },
    { agentId: "workflow-automation-engineer", role: "FORGE schedule", doc: "docs/DISKLORDZ_WORKFLOW_AUTOMATION_AGENT.md" },
    { agentId: "marketing-director", role: "VARA campaign", doc: "docs/DISKLORDZ_MARKETING_DIRECTOR.md" },
  ];
}
