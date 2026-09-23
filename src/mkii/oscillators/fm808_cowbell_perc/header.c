/* fm808_cowbell_perc — MIDI: C#1 cowbell, D#1 metal, F1 clave */
#include "unit_osc.h"

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_osc,
    .api = UNIT_API_VERSION,
    .dev_id = 0x0U,
    .unit_id = 0x0U,
    .version = 0x00010000U,
    .name = "808 FM Perc",
    .num_params = 8,
    .params = {
        {0, 1023, 0, 655, k_unit_param_type_none, 0, 0, 0, {"INDX"}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"RAT "}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"DEC "}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"TONE"}},
        {0, 1023, 0, 450, k_unit_param_type_none, 0, 0, 0, {"MTL "}},
        {0, 1023, 0, 880, k_unit_param_type_none, 0, 0, 0, {"LVL "}},
        {0, 1023, 0, 512, k_unit_param_type_none, 0, 0, 0, {"TUNE"}},
        {0, 1023, 0, 368, k_unit_param_type_none, 0, 0, 0, {"DRVE"}},
    },
};
