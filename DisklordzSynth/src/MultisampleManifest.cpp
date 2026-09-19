#include "disklordz/MultisampleManifest.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace disklordz::factory
{

namespace
{

std::string trim (std::string s)
{
    while (! s.empty() && std::isspace (static_cast<unsigned char> (s.front())))
        s.erase (s.begin());
    while (! s.empty() && std::isspace (static_cast<unsigned char> (s.back())))
        s.pop_back();
    return s;
}

InstrumentFamily parseFamily (std::string_view token)
{
    if (token == "acoustic_piano")
        return InstrumentFamily::acousticPiano;
    if (token == "electric_piano")
        return InstrumentFamily::electricPiano;
    if (token == "keys")
        return InstrumentFamily::keys;
    if (token == "organ")
        return InstrumentFamily::organ;
    if (token == "bells_mallets")
        return InstrumentFamily::bellsMallets;
    if (token == "guitar")
        return InstrumentFamily::guitar;
    if (token == "bass")
        return InstrumentFamily::bass;
    if (token == "sub_808")
        return InstrumentFamily::sub808;
    if (token == "synth_bass")
        return InstrumentFamily::synthBass;
    if (token == "synth_lead")
        return InstrumentFamily::synthLead;
    if (token == "synth_pluck")
        return InstrumentFamily::synthPluck;
    if (token == "pad")
        return InstrumentFamily::pad;
    if (token == "strings")
        return InstrumentFamily::strings;
    if (token == "brass")
        return InstrumentFamily::brass;
    if (token == "woodwind")
        return InstrumentFamily::woodwind;
    if (token == "choir_vocal")
        return InstrumentFamily::choirVocal;
    if (token == "texture")
        return InstrumentFamily::texture;
    if (token == "atmosphere")
        return InstrumentFamily::atmosphere;
    if (token == "fx")
        return InstrumentFamily::fx;
    return InstrumentFamily::keys;
}

RomLayer parseLayer (std::string_view token)
{
    if (token == "classic_digital")
        return RomLayer::classicDigital;
    if (token == "analog")
        return RomLayer::analog;
    if (token == "lo_fi")
        return RomLayer::loFi;
    if (token == "cyber_shift")
        return RomLayer::cyberShift;
    return RomLayer::real;
}

SampleTier parseTier (std::string_view token)
{
    if (token == "hero")
        return SampleTier::hero;
    if (token == "synth")
        return SampleTier::synth;
    if (token == "texture")
        return SampleTier::texture;
    return SampleTier::core;
}

} // namespace

bool loadMultisampleManifest (const std::string& path, MultisampleManifest& out, std::string& error)
{
    out = {};
    std::ifstream in (path);
    if (! in)
    {
        error = "cannot open manifest";
        return false;
    }

    std::string line;
    while (std::getline (in, line))
    {
        line = trim (line);
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss (line);
        std::string key;
        ss >> key;
        if (key == "id")
        {
            ss >> out.id;
        }
        else if (key == "family")
        {
            std::string val;
            ss >> val;
            out.family = parseFamily (val);
        }
        else if (key == "layer")
        {
            std::string val;
            ss >> val;
            out.layer = parseLayer (val);
        }
        else if (key == "tier")
        {
            std::string val;
            ss >> val;
            out.tier = parseTier (val);
        }
        else if (key == "program")
        {
            int prog = 0;
            ss >> prog;
            out.multisampleSetId = static_cast<std::uint16_t> (prog);
        }
        else if (key == "zone")
        {
            SampleZoneSpec z;
            ss >> z.rootMidiNote >> z.wavPath;
            if (ss >> z.velocityMin)
                ss >> z.velocityMax;
            out.zones.push_back (std::move (z));
        }
        else if (key == "zone_looped")
        {
            SampleZoneSpec z;
            z.looped = true;
            ss >> z.rootMidiNote >> z.wavPath;
            out.zones.push_back (std::move (z));
        }
    }

    if (out.id.empty() || out.zones.empty())
    {
        error = "manifest missing id or zones";
        return false;
    }
    return true;
}

} // namespace disklordz::factory
