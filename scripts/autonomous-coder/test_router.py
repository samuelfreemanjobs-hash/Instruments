#!/usr/bin/env python3
"""Unit tests for autonomous job router and dedupe."""

from __future__ import annotations

import unittest

from router import (
    classify_job,
    job_already_assigned,
    pick_agent,
    pick_easy_agent,
    record_assignment,
    secret_available,
)


class TestRouter(unittest.TestCase):
    def test_classify_easy_ci(self) -> None:
        item = {"id": "plugin-pr-146-trap-lord", "title": "Fix failing CI on Trap Lord"}
        self.assertEqual(classify_job(item), "easy")

    def test_classify_hard_default_plugin(self) -> None:
        item = {"id": "plugin-pr-151-chord-engine", "title": "CHORD ENGINE"}
        self.assertEqual(classify_job(item), "hard")

    def test_classify_hard_dsp(self) -> None:
        item = {"id": "x", "title": "JUCE", "acceptance": "Wire processBlock DSP chain"}
        self.assertEqual(classify_job(item), "hard")

    def test_classify_explicit_tier(self) -> None:
        item = {"id": "plugin-x", "job_tier": "easy", "title": "CHORD ENGINE MVP"}
        self.assertEqual(classify_job(item), "easy")

    def test_pick_easy_agent_fallback_order(self) -> None:
        self.assertEqual(pick_easy_agent({"GEMINI_API_KEY": "x"}), "gemini")
        self.assertEqual(
            pick_easy_agent({"OPENROUTER_API_KEY": "x"}),
            "aider",
        )
        self.assertEqual(
            pick_easy_agent({"GITHUB_TOKEN": "x"}),
            "jules",
        )
        self.assertIsNone(pick_easy_agent({}))

    def test_pick_agent_hard_cursor(self) -> None:
        item = {"id": "plugin-pr-1", "title": "Synth MVP"}
        tier, agent = pick_agent(item, pr=None, task_text=None, env={})
        self.assertEqual(tier, "hard")
        self.assertEqual(agent, "cursor")

    def test_pick_agent_easy_prefers_gemini(self) -> None:
        item = {"id": "x", "title": "Fix lint", "job_tier": "easy"}
        tier, agent = pick_agent(
            item, pr=None, task_text=None, env={"GEMINI_API_KEY": "t"}
        )
        self.assertEqual(tier, "easy")
        self.assertEqual(agent, "gemini")

    def test_pick_agent_easy_jules_when_only_github_token(self) -> None:
        item = {"id": "x", "job_tier": "easy", "title": "docs"}
        tier, agent = pick_agent(
            item, pr=None, task_text=None, env={"GITHUB_TOKEN": "t"}
        )
        self.assertEqual(tier, "easy")
        self.assertEqual(agent, "jules")

    def test_pick_agent_easy_cursor_when_no_free(self) -> None:
        item = {"id": "x", "job_tier": "easy", "title": "docs"}
        tier, agent = pick_agent(item, pr=None, task_text=None, env={"CURSOR_API_KEY": "k"})
        self.assertEqual(tier, "easy")
        self.assertEqual(agent, "cursor")

    def test_secret_available(self) -> None:
        self.assertFalse(secret_available("GEMINI_API_KEY", {}))
        self.assertTrue(secret_available("GEMINI_API_KEY", {"GEMINI_API_KEY": "abc"}))

    def test_dedupe(self) -> None:
        state: dict = {}
        self.assertFalse(job_already_assigned(state, 146, "plugin-pr-146"))
        record_assignment(state, agent="gemini", pr_number=146, slice_id="plugin-pr-146", tier="easy")
        self.assertTrue(job_already_assigned(state, 146, "plugin-pr-146"))
        self.assertEqual(state["assigned_agents"]["pr:146"]["agent"], "gemini")


if __name__ == "__main__":
    unittest.main()