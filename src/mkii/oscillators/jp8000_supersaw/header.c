/* jp8000_supersaw — edit dev_id/unit_id before public release. */
#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "JP8000 Super",
    .num_params = 9,
    .params = {
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"DTUN"}},
        {0, 1023, 0, 870, k_unit_param_type_none, 0, 0, 0, {"VOIC"}},
        {0, 1023, 0, 532, k_unit_param_type_none, 0, 0, 0, {"WDTH"}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"SUB "}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"TONE"}},
        {0, 1023, 0, 870, k_unit_param_type_none, 0, 0, 0, {"LVL "}},
        {0, 1023, 0, 61, k_unit_param_type_none, 0, 0, 0, {"ATK "}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"REL "}},
        {0, 1023, 0, 163, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
    },
};
