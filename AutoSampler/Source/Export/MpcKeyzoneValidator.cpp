#include "MpcKeyzoneValidator.h"

namespace autosampler
{

MpcKeyzoneValidation MpcKeyzoneValidator::validateProgram (const MultisampleProgram& program)
{
    MpcKeyzoneValidation result;

    if (program.empty())
    {
        result.ok = false;
        result.errors.add ("No zones in program.");
        result.reportText = result.errors.joinIntoString ("\n");
        return result;
    }

    struct ZoneView
    {
        int low;
        int high;
        int root;
        int layerCount;
    };

    std::vector<ZoneView> views;
    views.reserve (program.zoneCount());

    for (std::size_t i = 0; i < program.zoneCount(); ++i)
    {
        const auto& z = program.zones()[i];
        views.push_back ({ program.lowKeyForZone (i),
                           program.highKeyForZone (i),
                           z.rootNote,
                           z.velocityLayerCount() });
    }

    std::sort (views.begin(), views.end(), [] (const ZoneView& a, const ZoneView& b) { return a.low < b.low; });

    const bool spansFullKeyboard = views.size() > 1 || (views.size() == 1 && views.front().low == 0 && views.front().high == 127);
    if (spansFullKeyboard)
    {
        if (views.front().low != 0)
        {
            result.ok = false;
            result.errors.add ("First zone starts at " + juce::String (views.front().low) + ", not 0.");
        }

        if (views.back().high != 127)
        {
            result.ok = false;
            result.errors.add ("Last zone ends at " + juce::String (views.back().high) + ", not 127.");
        }
    }

    for (std::size_t i = 1; i < views.size(); ++i)
    {
        const auto& prev = views[i - 1];
        const auto& curr = views[i];
        if (curr.low != prev.high + 1)
        {
            result.ok = false;
            result.errors.add ("Gap/overlap: zone ending " + juce::String (prev.high)
                               + " then starting " + juce::String (curr.low));
        }
        if (curr.low > curr.high)
        {
            result.ok = false;
            result.errors.add ("Invalid range " + juce::String (curr.low) + "-" + juce::String (curr.high));
        }
    }

    for (std::size_t zi = 0; zi < program.zoneCount(); ++zi)
    {
        const auto& z = program.zones()[zi];
        const auto layers = z.velocityLayersForExport();

        if (layers.size() > 4)
        {
            result.ok = false;
            result.errors.add ("Zone root " + juce::String (z.rootNote) + ": more than 4 velocity layers.");
        }

        for (const auto& layer : layers)
        {
            if (layer.velStart < 0 || layer.velEnd > 127 || layer.velStart > layer.velEnd)
            {
                result.ok = false;
                result.errors.add ("Zone " + juce::String (z.rootNote) + ": invalid velocity span.");
            }
        }
    }

    juce::String report = result.ok ? "MPC keyzone validation: OK\n" : "MPC keyzone validation: FAILED\n";
    for (const auto& e : result.errors)
        report << "  - " << e << "\n";

    result.reportText = report;
    return result;
}

} // namespace autosampler
