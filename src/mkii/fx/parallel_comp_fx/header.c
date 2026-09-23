#include "unit_modfx.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_modfx,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "Par Comp",
    .num_params = 8,
    .params = {
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"THRS"}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"RAT "}},
        {0, 1023, 0, 245, k_unit_param_type_none, 0, 0, 0, {"ATK "}},
        {0, 1023, 0, 409, k_unit_param_type_none, 0, 0, 0, {"REL "}},
        {0, 1023, 0, 614, k_unit_param_type_none, 0, 0, 0, {"PAR "}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"DRV "}},
        {-1000, 1000, 0, 500, k_unit_param_type_drywet, 1, 1, 0, {"MIX "}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"TONE"}},
    },
};
