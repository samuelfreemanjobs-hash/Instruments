#!/usr/bin/env python3
"""Unit tests for autonomous coder launch policy."""

from __future__ import annotations

import unittest
from unittest.mock import patch

from cursor_cloud import (
    count_running,
    slice_already_running,
)
from github_pr import pr_is_open


class TestLaunchPolicy(unittest.TestCase):
    def test_pr_is_open(self) -> None:
        self.assertTrue(pr_is_open({"state": "open", "merged_at": None}))
        self.assertFalse(pr_is_open({"state": "closed", "merged_at": None}))
        self.assertFalse(pr_is_open({"state": "open", "merged_at": "2026-01-01T00:00:00Z"}))
        self.assertFalse(pr_is_open(None))

    def test_count_running(self) -> None:
        agents = [
            {"status": "RUNNING"},
            {"status": "FINISHED"},
            {"status": "ACTIVE"},
        ]
        self.assertEqual(count_running(agents), 2)

    def test_slice_dedupe(self) -> None:
        agents = [
            {
                "status": "RUNNING",
                "prompt": {"text": "Queue id: `plugin-pr-146-trap-lord`"},
            }
        ]
        self.assertTrue(slice_already_running(agents, "plugin-pr-146-trap-lord"))
        self.assertFalse(slice_already_running(agents, "plugin-pr-101-auto-sampler"))

    @patch.dict("os.environ", {}, clear=True)
    def test_launch_skips_without_key(self) -> None:
        from cursor_cloud import launch_slice_agent

        state: dict = {}
        ok = launch_slice_agent(
            {"id": "plugin-pr-146-trap-lord", "title": "Trap Lord"},
            "slice body",
            repo_url="https://github.com/example/x",
            ref="main",
            pr={"number": 146, "title": "Trap Lord"},
            state=state,
        )
        self.assertFalse(ok)
        self.assertNotIn("launched_agents", state)


if __name__ == "__main__":
    unittest.main()