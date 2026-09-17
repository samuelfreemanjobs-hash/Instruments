from enum import Enum


class BatchStage(str, Enum):
    discover = "discover"
    ideate = "ideate"
    generate = "generate"
    select = "select"
    produce = "produce"
    humanize = "humanize"
    qa = "qa"
    package = "package"
    publish = "publish"
    distribute = "distribute"
    measure = "measure"
    learn = "learn"


class RightsStatus(str, Enum):
    draft = "draft"
    review = "review"
    cleared = "cleared"
    blocked = "blocked"


class AssetKind(str, Enum):
    track = "track"
    drum = "drum"
    eight_oh_eight = "808"
    loop = "loop"
    preset = "preset"
    fx = "fx"
    visual = "visual"
    video = "video"
    midi = "midi"
    kit = "kit"
