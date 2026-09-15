#include "InternalPluginTypes.h"
#include "GainProcessor.h"
#include "PanProcessor.h"
#include "Eq3Processor.h"
#include "CompressorProcessor.h"
#include "LimiterProcessor.h"
#include "HpfProcessor.h"
#include "DeEsserProcessor.h"

namespace vmpc::audio::internal
{
std::unique_ptr<juce::AudioProcessor> createMixPlugin(MixPluginId id)
{
    switch (id)
    {
        case MixPluginId::Gain:
            return std::make_unique<GainProcessor>();
        case MixPluginId::Pan:
            return std::make_unique<PanProcessor>();
        case MixPluginId::Eq3:
            return std::make_unique<Eq3Processor>();
        case MixPluginId::Compressor:
            return std::make_unique<CompressorProcessor>();
        case MixPluginId::Limiter:
            return std::make_unique<LimiterProcessor>();
        case MixPluginId::Hpf:
            return std::make_unique<HpfProcessor>();
        case MixPluginId::DeEsser:
            return std::make_unique<DeEsserProcessor>();
    }

    return nullptr;
}
} // namespace vmpc::audio::internal
