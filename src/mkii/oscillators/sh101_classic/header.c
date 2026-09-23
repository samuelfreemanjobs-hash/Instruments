/* sh101_classic — edit dev_id/unit_id before public release. */
#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "SH101 Classic",
    .num_params = 9,
    .params = {
        {0, 1023, 0, 307, k_unit_param_type_none, 0, 0, 0, {"VIBE"}},
        {0, 1023, 0, 471, k_unit_param_type_none, 0, 0, 0, {"PWM "}},
        {0, 1023, 0, 532, k_unit_param_type_none, 0, 0, 0, {"SUB "}},
        {0, 1023, 0, 286, k_unit_param_type_none, 0, 0, 0, {"HOLW"}},
        {0, 1023, 0, 184, k_unit_param_type_none, 0, 0, 0, {"SAW "}},
        {0, 1023, 0, 245, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
        {0, 1023, 0, 102, k_unit_param_type_none, 0, 0, 0, {"ATK "}},
        {0, 1023, 0, 430, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 389, k_unit_param_type_none, 0, 0, 0, {"REL "}},
    },
};
