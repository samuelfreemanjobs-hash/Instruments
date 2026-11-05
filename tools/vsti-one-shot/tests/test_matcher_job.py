from pathlib import Path

from vsti_one_shot.matcher import build_render_job
from vsti_one_shot.recipe_author import build_recipe


def test_build_render_job_requires_all_slots():
    recipe = build_recipe("test", slot_count=4)
    assignments = {
        "assignments": [
            {
                "slot_id": s["id"],
                "vst3_path": "/fake/Synth.vst3",
                "program": i,
                "root_midi": s["root_midi"],
            }
            for i, s in enumerate(recipe["slots"])
        ]
    }
    job = build_render_job(recipe, assignments, Path("/tmp/out"))
    assert len(job["renders"]) == 4
    assert job["output_dir"].endswith("out")
    assert all("max_capture_sec" in r for r in job["renders"])
