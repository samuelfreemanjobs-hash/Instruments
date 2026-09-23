/* tb303_detroit_acid — edit dev_id/unit_id before public release. */
#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "303 Detroit",
    .num_params = 7,
    .params = {
        {0, 1023, 0, 184, k_unit_param_type_none, 0, 0, 0, {"WAVE"}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"SLID"}},
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"ACNT"}},
        {0, 1023, 0, 532, k_unit_param_type_none, 0, 0, 0, {"ENV "}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"DRVE"}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
    },
};
