/**
 * Contract checks for JD Upgraded default MIDI render — implemented in OfflineRender
 * (--smoke-check / --smoke-lazy-check).
 */
#include <cstdio>
#include <cstdlib>
#include <string>

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::fprintf (stderr, "Usage: JDUpgradedMidiSmoke <path-to-OfflineRender>\n");
        return 1;
    }

    const std::string offlineRender = std::string ("\"") + argv[1] + "\"";

    const int lazyRc = std::system ((offlineRender + " --smoke-lazy-check").c_str());
    if (lazyRc != 0)
    {
        std::fprintf (stderr, "FAIL: lazy DSP init / no prepare smoke (exit %d)\n", lazyRc);
        return 1;
    }

    const int prepRc = std::system ((offlineRender + " --smoke-check").c_str());
    if (prepRc != 0)
    {
        std::fprintf (stderr, "FAIL: prepared smoke (exit %d)\n", prepRc);
        return 1;
    }

    std::printf ("PASS: JD Upgraded MIDI smoke (via OfflineRender)\n");
    return 0;
}