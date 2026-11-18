"""Original, genre-guided recipe modifiers; aesthetic starting points, not artist cloning."""
from copy import deepcopy
from recipe_engine import recipe_for, validate_recipe

PROFILES = {
    'drift-phonk': {'description': 'Driven subs, bright bells, compact hard drums', 'drive': 1.32, 'tone': 1.16, 'decay': .83, 'noise': 1.06},
    # Converter Night (CNV) default factory profile: rate-drop / converter grit aesthetic.
    'memphis-tape': {'description': 'Memphis converter phonk: darker subs, tape-softened hits, cutting tops', 'drive': 1.18, 'tone': .68, 'decay': .90, 'noise': .88},
    'detroit-trap': {'description': 'Punchy kicks, articulate hats, dry snare attacks', 'drive': 1.13, 'tone': 1.18, 'decay': .79, 'noise': 1.13},
    'southern-trap': {'description': 'Long sub energy, hard snares and metallic accents', 'drive': 1.22, 'tone': 1.04, 'decay': 1.18, 'noise': 1.02},
    'cloud-rap': {'description': 'Airy tails, softer attacks and rounded percussion', 'drive': .84, 'tone': .87, 'decay': 1.24, 'noise': .75},
}

def genre_recipe(kind, variant, genre):
    if genre not in PROFILES: raise ValueError('unsupported genre: '+str(genre))
    r = deepcopy(recipe_for(kind, variant))
    p = PROFILES[genre]
    for field, mult, lo, hi in (
        ('drive', p['drive'], .1, 8.0), ('tone', p['tone'], 0.0, 1.0),
        ('decay_sec', p['decay'], .01, 2.0), ('noise_color', p['noise'], 0.0, 1.0)):
        r[field] = round(max(lo, min(hi, r[field] * mult)), 4)
    # Genre identity is metadata, not a change to the core DSP engine contract.
    r['genre'] = genre
    validate_recipe(r)
    return r
