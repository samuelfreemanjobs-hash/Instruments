/* memphis_dust_sub — edit dev_id/unit_id before public release. */
#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Dust Sub",
    .num_params = 10,
    .params = {
        {0, 1023, 0, 542, k_unit_param_type_none, 0, 0, 0, {"VIBE"}},
        {0, 1023, 0, 798, k_unit_param_type_none, 0, 0, 0, {"SUB "}},
        {0, 1023, 0, 716, k_unit_param_type_none, 0, 0, 0, {"TRNK"}},
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"DUST"}},
        {0, 1023, 0, 573, k_unit_param_type_none, 0, 0, 0, {"TAPE"}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"MID "}},
        {0, 1023, 0, 205, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
        {0, 1023, 0, 61, k_unit_param_type_none, 0, 0, 0, {"ATK "}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 491, k_unit_param_type_none, 0, 0, 0, {"REL "}},
    },
};
