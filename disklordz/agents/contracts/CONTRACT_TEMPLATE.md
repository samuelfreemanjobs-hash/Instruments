# Agent contract template

```yaml
agent_id: role-name
mission: one sentence
inputs:
  - artifact types
outputs:
  - artifact types
tools:
  - allowed MCP / scripts
owns:
  - paths agent may create/delete
may_read:
  - paths
may_modify:
  - paths
must:
  - non-negotiable behaviors
cannot:
  - hard prohibitions
definition_of_done:
  - checklist
failure_conditions:
  - when to mark Failed / escalate
escalation:
  - to whom / what artifact
```

All production work references a **Work Order** id.
