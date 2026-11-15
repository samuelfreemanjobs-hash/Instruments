/**
 * Offline export: writes one Standard MIDI File per progression preset (content-marketing pack).
 */
#include "../Source/GrimChordEngine.h"
#include "../Source/ProgressionMidiBuilder.h"
#include "../Source/ProgressionPresetsData.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include <cstdio>

namespace {

juce::String safeFilename(juce::String name) {
  juce::String out;
  for (auto c : name) {
    if (juce::CharacterFunctions::isLetterOrDigit(c) || c == '-' || c == '_') {
      out << c;
    } else if (c == ' ') {
      out << '_';
    }
  }
  return out.isEmpty() ? "preset" : out;
}

} // namespace

int main(int argc, char** argv) {
  juce::File outDir = juce::File::getCurrentWorkingDirectory()
                          .getChildFile("release")
                          .getChildFile("GrimMIDI")
                          .getChildFile("midi-pack");

  for (int i = 1; i < argc; ++i) {
    const juce::String arg(argv[i]);
    if (arg.startsWith("--out=")) {
      outDir = juce::File(arg.substring(6));
    }
  }

  if (!outDir.createDirectory().wasOk() && !outDir.isDirectory()) {
    std::fprintf(stderr, "Cannot create output dir: %s\n", outDir.getFullPathName().toRawUTF8());
    return 1;
  }

  int written = 0;
  for (int idx = 0; idx < kProgressionPresetCount; ++idx) {
    const auto& preset = kProgressionPresets[static_cast<size_t>(idx)];

    ProgressionRenderParams params;
    params.rootKeyPc = (preset.keyPc < 12) ? preset.keyPc : 8;
    params.scale = static_cast<GrimChordEngine::ScaleType>(preset.scale);
    params.chordQuality = static_cast<GrimChordEngine::ChordQuality>(preset.chordQuality);
    params.inversion = 0;
    params.spreadPct = 45;
    params.octaveShift = 0;
    params.strumTotalSeconds = 0.022;

    juce::MidiFile file;
    if (!buildProgressionMidiFile(preset, params, file)) {
      std::fprintf(stderr, "SKIP empty preset %d %s\n", idx, preset.name);
      continue;
    }

    juce::String fname =
        juce::String(idx + 1).paddedLeft('0', 3) + "_" + safeFilename(juce::String(preset.name)) + ".mid";
    const auto dest = outDir.getChildFile(fname);
    juce::FileOutputStream stream(dest);
    if (!stream.openedOk()) {
      std::fprintf(stderr, "FAIL open %s (fname=%s)\n", dest.getFullPathName().toRawUTF8(),
                  fname.toRawUTF8());
      return 1;
    }
    if (!file.writeTo(stream)) {
      std::fprintf(stderr, "FAIL midi write %s\n", dest.getFullPathName().toRawUTF8());
      return 1;
    }
    stream.flush();
    ++written;
  }

  std::printf("Exported %d/%d MIDI files to %s\n", written, kProgressionPresetCount,
              outDir.getFullPathName().toRawUTF8());
  return written == kProgressionPresetCount ? 0 : 2;
}
