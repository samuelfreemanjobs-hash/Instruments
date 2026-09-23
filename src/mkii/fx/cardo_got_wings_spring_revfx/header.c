#include "unit_revfx.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_revfx,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Wings Spring",
    .num_params = 8,
    .params = {
        {0, 1023, 0, 520, k_unit_param_type_none, 0, 0, 0, {"SPRG"}},
        {0, 1023, 0, 580, k_unit_param_type_none, 0, 0, 0, {"TONE"}},
        {0, 1023, 0, 420, k_unit_param_type_none, 0, 0, 0, {"SIZE"}},
        {0, 1023, 0, 380, k_unit_param_type_none, 0, 0, 0, {"DAMP"}},
        {0, 1023, 0, 120, k_unit_param_type_none, 0, 0, 0, {"PRE "}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"GLOW"}},
        {-1000, 1000, 0, 420, k_unit_param_type_drywet, 1, 1, 0, {"MIX "}},
        {0, 2, 0, 1, k_unit_param_type_strings, 0, 0, 0, {"DRUM"}},
    },
};
