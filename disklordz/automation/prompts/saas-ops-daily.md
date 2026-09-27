# {{WO}} — SaaS Ops & Go-Live Guardian

Read `.cursor/agents/disklordz-saas-ops-guardian.md`.

1. Run `bash disklordz/automation/scripts/run-saas-ops-daily.sh --skip-agent`.
2. Fix any failure (health API, verify-go-live, build, preset/billing static checks).
3. Improve ops: clearer errors, health checks, docs, or CI smoke—**one focused change**.
4. Push `cursor/saas-ops-<topic>-4170` and open draft PR **`{{WO}}: SaaS ops — <topic>`**.

Do not deploy production or commit secrets.
