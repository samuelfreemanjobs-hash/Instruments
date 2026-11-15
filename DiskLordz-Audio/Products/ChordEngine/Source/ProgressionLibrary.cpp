#include "ProgressionLibrary.h"

namespace disklordz::chordengine
{

using namespace disklordz::theory;

const std::array<StarterProgression, kNumStarterProgressions>& starterProgressions()
{
    static const std::array<StarterProgression, kNumStarterProgressions> kPresets = { {
        { "dark_trap_01",
          "Dark Trap — tension loop",
          "Dark Trap",
          ScaleType::naturalMinor,
          5,
          { { { 1, ChordQuality::minor },
              { 6, ChordQuality::major },
              { 3, ChordQuality::major },
              { 7, ChordQuality::major } } } },
        { "dark_trap_02",
          "Dark Trap — descent",
          "Dark Trap",
          ScaleType::phrygian,
          5,
          { { { 1, ChordQuality::minor },
              { 2, ChordQuality::major },
              { 6, ChordQuality::major },
              { 5, ChordQuality::minor } } } },
        { "drift_phonk_01",
          "Drift Phonk — crawl",
          "Drift Phonk",
          ScaleType::naturalMinor,
          8,
          { { { 1, ChordQuality::minor7 },
              { 4, ChordQuality::minor7 },
              { 7, ChordQuality::major },
              { 6, ChordQuality::major } } } },
        { "drift_phonk_02",
          "Drift Phonk — sustained",
          "Drift Phonk",
          ScaleType::harmonicMinor,
          8,
          { { { 1, ChordQuality::minor7 },
              { 5, ChordQuality::dominant7 },
              { 4, ChordQuality::minor7 },
              { 6, ChordQuality::major7 } } } },
        { "memphis_01",
          "Memphis — underground loop",
          "Memphis Underground",
          ScaleType::naturalMinor,
          2,
          { { { 1, ChordQuality::minor },
              { 7, ChordQuality::major },
              { 4, ChordQuality::minor },
              { 1, ChordQuality::minor } } } },
        { "luxury_bounce",
          "Luxury — moody bounce",
          "Luxury Rap",
          ScaleType::naturalMinor,
          9,
          { { { 1, ChordQuality::minor7 },
              { 6, ChordQuality::major7 },
              { 4, ChordQuality::minor7 },
              { 5, ChordQuality::dominant7 } } } },
    } };
    return kPresets;
}

} // namespace disklordz::chordengine