#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "OB-Xa",
    .num_params = 5,
    .params = {
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"MIX "}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"DETU"}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"PWM "}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"BRUT"}},
        {0, 1023, 0, 286, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
    },
};
