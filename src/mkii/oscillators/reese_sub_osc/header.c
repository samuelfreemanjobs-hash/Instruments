#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Reese Sub",
    .num_params = 6,
    .params = {
        {0, 1023, 0, 798, k_unit_param_type_none, 0, 0, 0, {"DTUN"}},
        {0, 1023, 0, 737, k_unit_param_type_none, 0, 0, 0, {"SPRD"}},
        {0, 1023, 0, 819, k_unit_param_type_none, 0, 0, 0, {"SUB "}},
        {0, 1023, 0, 400, k_unit_param_type_none, 0, 0, 0, {"TONE"}},
        {0, 1023, 0, 880, k_unit_param_type_none, 0, 0, 0, {"LVL "}},
        {0, 1023, 0, 286, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
    },
};
