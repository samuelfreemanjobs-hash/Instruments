#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Mini Phatt",
    .num_params = 5,
    .params = {
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"MIX "}},
        {0, 1023, 0, 532, k_unit_param_type_none, 0, 0, 0, {"DRVE"}},
        {0, 1023, 0, 245, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
        {0, 1023, 0, 80, k_unit_param_type_none, 0, 0, 0, {"ATK "}},
        {0, 1023, 0, 380, k_unit_param_type_none, 0, 0, 0, {"REL "}},
    },
};
