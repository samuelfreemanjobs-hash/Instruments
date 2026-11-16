#pragma once

// No-op unless the translation unit is built with TRACY_ENABLE and the Tracy
// client headers are on the include path. Do not vendor Tracy in this repo.
#if defined(TRACY_ENABLE)
#include <tracy/Tracy.hpp>
#define DLZ_ZONE() ZoneScoped
#else
#define DLZ_ZONE()                                                                                                     \
    do {                                                                                                               \
    } while (0)
#endif
