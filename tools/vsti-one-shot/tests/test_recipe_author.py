from vsti_one_shot.recipe_author import SLOT_COUNT, build_recipe


def test_build_recipe_slot_count():
    recipe = build_recipe("dark Memphis melodic")
    assert recipe["slot_count"] == SLOT_COUNT
    assert len(recipe["slots"]) == SLOT_COUNT


def test_slot_ids_unique_and_slugged():
    recipe = build_recipe("Bright French Touch!")
    ids = [s["id"] for s in recipe["slots"]]
    assert len(ids) == len(set(ids))
    assert ids[0].startswith("bright_french_touch_")


def test_root_midi_in_range():
    recipe = build_recipe("test style")
    for slot in recipe["slots"]:
        assert 0 <= slot["root_midi"] <= 127
