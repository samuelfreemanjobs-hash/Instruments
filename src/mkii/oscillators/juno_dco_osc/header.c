/* juno_dco_osc — edit dev_id/unit_id before public release. */
#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Juno DCO",
    .num_params = 8,
    .params = {
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"VIBE"}},
        {0, 1023, 0, 737, k_unit_param_type_none, 0, 0, 0, {"SAW "}},
        {0, 1023, 0, 286, k_unit_param_type_none, 0, 0, 0, {"PWM "}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"CHOR"}},
        {0, 1023, 0, 245, k_unit_param_type_none, 0, 0, 0, {"GLID"}},
        {0, 1023, 0, 82, k_unit_param_type_none, 0, 0, 0, {"ATK "}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 389, k_unit_param_type_none, 0, 0, 0, {"REL "}},
    },
};
