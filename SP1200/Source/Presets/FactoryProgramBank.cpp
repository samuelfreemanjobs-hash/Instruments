#include "Presets/FactoryProgramBank.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <vector>

namespace sp1200::presets
{

namespace
{

float rand01 (std::uint32_t& state) noexcept
{
    state = state * 1664525u + 1013904223u;
    return static_cast<float> (state >> 8) * (1.0f / 16777216.0f);
}

const char* categoryForIndex (std::size_t index) noexcept
{
    static constexpr std::array<const char*, 8> kCategories { "Trap",
                                                              "Phonk",
                                                              "Boom Bap",
                                                              "Plugg Kit",
                                                              "Memphis",
                                                              "Dirty South",
                                                              "Drill",
                                                              "Lo-Fi MPC" };
    return kCategories[index % kCategories.size()];
}

FactoryProgram buildProgram (std::size_t index)
{
    std::uint32_t state = 0x1200u ^ static_cast<std::uint32_t> (index * 2654435761ull);
    FactoryProgram prog;
    const char* category = categoryForIndex (index);

    char nameBuf[64];
    std::snprintf (nameBuf, sizeof (nameBuf), "%s %03zu", category, index + 1);
    prog.name = nameBuf;

    prog.filterCutoffNorm = 0.35f + rand01 (state) * 0.55f;
    prog.filterResonanceNorm = 0.08f + rand01 (state) * 0.45f;

    for (int pad = 0; pad < 16; ++pad)
    {
        const float role = static_cast<float> (pad) / 15.0f;
        prog.padLevel[pad] = 0.35f + rand01 (state) * 0.85f;
        prog.padDecayNorm[pad] = 0.12f + rand01 (state) * 0.82f;

        if (pad < 4)
            prog.padTuneSemitones[pad] = -24.0f + rand01 (state) * 6.0f;
        else if (pad < 8)
            prog.padTuneSemitones[pad] = (rand01 (state) - 0.5f) * 7.0f;
        else
            prog.padTuneSemitones[pad] = 3.0f + role * 14.0f + (rand01 (state) - 0.5f) * 4.0f;
    }

    if (index % 9 == 0)
    {
        for (int pad = 0; pad < 4; ++pad)
            prog.padDecayNorm[pad] = 0.65f + rand01 (state) * 0.3f;
    }

    return prog;
}

std::vector<FactoryProgram> buildBank()
{
    std::vector<FactoryProgram> bank;
    bank.reserve (kFactoryProgramCount);
    for (std::size_t i = 0; i < kFactoryProgramCount; ++i)
        bank.push_back (buildProgram (i));
    return bank;
}

} // namespace

const std::vector<FactoryProgram>& getFactoryPrograms() noexcept
{
    static const std::vector<FactoryProgram> kPrograms = buildBank();
    return kPrograms;
}

void applyFactoryProgram (sp1200::SamplerEngine& engine, int programIndex) noexcept
{
  if (programIndex < 0 || static_cast<std::size_t> (programIndex) >= kFactoryProgramCount)
    return;

  const auto& prog = getFactoryPrograms()[static_cast<std::size_t> (programIndex)];
  engine.setFilterCutoffNorm (prog.filterCutoffNorm);
  engine.setFilterResonance (prog.filterResonanceNorm);

  for (int pad = 0; pad < 16; ++pad)
  {
    engine.setPadLevel (pad, prog.padLevel[pad]);
    engine.setPadTune (pad, prog.padTuneSemitones[pad]);
    engine.setPadDecay (pad, prog.padDecayNorm[pad]);
  }
}

const std::string& programNameAt (int programIndex) noexcept
{
  static const std::string kEmpty;
  if (programIndex < 0 || static_cast<std::size_t> (programIndex) >= kFactoryProgramCount)
    return kEmpty;
  return getFactoryPrograms()[static_cast<std::size_t> (programIndex)].name;
}

} // namespace sp1200::presets
