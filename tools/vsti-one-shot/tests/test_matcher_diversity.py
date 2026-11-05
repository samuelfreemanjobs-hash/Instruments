from vsti_one_shot.matcher import unique_program_count
from vsti_one_shot.recipe_author import build_recipe


def test_offline_assignments_spread_programs():
    recipe = build_recipe("spread test style", slot_count=20)
    from vsti_one_shot.matcher import match_assignments_offline

    doc = match_assignments_offline(recipe, __import__("pathlib").Path("/fake/x.vst3"), assumed_programs=64)
    assert unique_program_count(doc) >= 10
