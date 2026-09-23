#include "fx.h"
#include "unit_modfx.h"
#include "utils/int_math.h"
#include <algorithm>

static ParallelCompFx s_processor_instance;
static int32_t cached_values[UNIT_MODFX_MAX_PARAM_COUNT];

__unit_callback int8_t unit_init(const unit_runtime_desc_t *desc)
{
  if (!desc)
    return k_unit_err_undef;
  if (desc->target != unit_header.target)
    return k_unit_err_target;
  if (!UNIT_API_IS_COMPAT(desc->api))
    return k_unit_err_api_version;
  if (desc->samplerate != s_processor_instance.getSampleRate())
    return k_unit_err_samplerate;
  if (desc->input_channels != 2 || desc->output_channels != 2)
    return k_unit_err_geometry;

  s_processor_instance.init(nullptr);
  for (int id = 0; id < UNIT_MODFX_MAX_PARAM_COUNT; ++id) {
    cached_values[id] = static_cast<int32_t>(unit_header.params[id].init);
  }
  return k_unit_err_none;
}

__unit_callback void unit_teardown() { s_processor_instance.teardown(); }
__unit_callback void unit_reset() { s_processor_instance.reset(); }
__unit_callback void unit_resume() { s_processor_instance.resume(); }
__unit_callback void unit_suspend() { s_processor_instance.suspend(); }
__unit_callback void unit_render(const float *in, float *out, uint32_t frames)
{
  s_processor_instance.process(in, out, frames);
}

__unit_callback void unit_set_param_value(uint8_t id, int32_t value)
{
  value = clipminmaxi32(unit_header.params[id].min, value, unit_header.params[id].max);
  cached_values[id] = value;
  s_processor_instance.setParameter(id, value);
}

__unit_callback int32_t unit_get_param_value(uint8_t id) { return cached_values[id]; }

__unit_callback const char *unit_get_param_str_value(uint8_t id, int32_t value)
{
  value = clipminmaxi32(unit_header.params[id].min, value, unit_header.params[id].max);
  return s_processor_instance.getParameterStrValue(id, value);
}

__unit_callback void unit_set_tempo(uint32_t tempo)
{
  float bpm = (tempo >> 16) + (tempo & 0xFFFF) / static_cast<float>(0x10000);
  s_processor_instance.setTempo(bpm);
}

__unit_callback void unit_tempo_4ppqn_tick(uint32_t counter)
{
  s_processor_instance.tempo4ppqnTick(counter);
}
