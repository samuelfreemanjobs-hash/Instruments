#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Drill 808",
    .num_params = 8,
    .params = {
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"SLID"}},
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"TAIL"}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"TUNE"}},
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"PNCH"}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 655, k_unit_param_type_none, 0, 0, 0, {"SUB "}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"DRVE"}},
        {0, 1023, 0, 880, k_unit_param_type_none, 0, 0, 0, {"LVL "}},
    },
};
