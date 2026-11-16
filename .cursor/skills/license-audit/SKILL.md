---
name: license-audit
description: Scans the diff for live payment secrets, private keys, and absolute sample-library paths. Use before a commit or a release.
---

# License audit

Search added lines for:

- `sk_live_` or `whsec_` followed by 8 or more characters
- `-----BEGIN PRIVATE KEY-----` and RSA/EC/OpenSSH variants
- `/Users/` or `C:\Users\` paths that continue into `Samples` or a plug-in library

```bash
python3 .cursor/hooks/scan-secrets.py <<EOF
{"file_path":"PATH"}
EOF
```

Skip markdown docs that only name an env var. Report file, line, and a redacted match. Do not rewrite the file until the user picks the fix (env var, or delete the line).

If `.cursor/hooks/state/violation.json` exists, treat the tree as blocked.
