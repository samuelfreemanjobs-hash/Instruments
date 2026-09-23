#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Plug Pluck",
    .num_params = 8,
    .params = {
        {0, 1023, 0, 655, k_unit_param_type_none, 0, 0, 0, {"PLUK"}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"TONE"}},
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"SUB "}},
        {0, 1023, 0, 286, k_unit_param_type_none, 0, 0, 0, {"DETU"}},
        {0, 1023, 0, 184, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"DRVE"}},
        {0, 1023, 0, 880, k_unit_param_type_none, 0, 0, 0, {"LVL "}},
    },
};
