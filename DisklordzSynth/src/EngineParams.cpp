#include "disklordz/EngineParams.h"

#include <cstring>

namespace disklordz::synth
{

const char* engineIdName (EngineId id)
{
    switch (id)
    {
        case EngineId::subtractive: return "subtractive";
        case EngineId::additive: return "additive";
        case EngineId::karplus: return "karplus";
        case EngineId::wave: return "wave";
    }
    return "unknown";
}

EngineId parseEngineId (const char* name)
{
    if (name == nullptr)
        return EngineId::additive;

    if (std::strcmp (name, "subtractive") == 0 || std::strcmp (name, "sub") == 0 || std::strcmp (name, "sub808") == 0)
        return EngineId::subtractive;
    if (std::strcmp (name, "additive") == 0 || std::strcmp (name, "bell") == 0)
        return EngineId::additive;
    if (std::strcmp (name, "karplus") == 0 || std::strcmp (name, "keys") == 0)
        return EngineId::karplus;
    if (std::strcmp (name, "wave") == 0 || std::strcmp (name, "wavetable") == 0 || std::strcmp (name, "pad") == 0)
        return EngineId::wave;

    return EngineId::additive;
}

} // namespace disklordz::synth
