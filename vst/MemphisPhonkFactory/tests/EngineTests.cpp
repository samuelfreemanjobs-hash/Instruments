#include <cmath>
#include <iostream>
#include <juce_core/juce_core.h>
#include "Dsp/DrumPattern.h"
#include "Dsp/MemphisPhonkEngine.h"
#include "Dsp/PackExportOptions.h"

static int failures = 0;

#define EXPECT(cond, msg)                                                                          \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      std::cerr << "FAIL: " << msg << '\n';                                                        \
      ++failures;                                                                                  \
    }                                                                                              \
  } while (0)

static float peak(const std::vector<float> &v) {
  float p = 0.0f;
  for (float s : v) {
    p = std::max(p, std::abs(s));
  }
  return p;
}

int main() {
  MemphisPhonkEngine engine(44100.0);
  const auto kick = engine.generateKick();
  EXPECT(!kick.empty(), "kick should have samples");
  EXPECT(peak(kick) > 0.05f, "kick should be audible");

  PackExportOptions opts;
  opts.primaryBpm = 118.0f;
  opts.bars = 2;
  opts.exportMidi = true;
  opts.exportStems = true;
  opts.writeMpcManifest = true;

  const auto loop = engine.generateDrumLoop(opts.primaryBpm, opts.bars, opts.swing, opts.pattern);
  EXPECT(loop.size() > 44100, "loop should be at least one second");

  const juce::File tmp = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("memphis_phonk_test");
  tmp.deleteRecursively();
  tmp.createDirectory();
  const juce::String result = engine.exportMpcKit(tmp, opts);
  EXPECT(result.contains("Exported"), result.toStdString());

  const juce::File kit = tmp.getChildFile("Memphis_Phonk_MPC_Kit");
  int wavCount = 0;
  for (auto &f : kit.findChildFiles(juce::File::findFiles, true, "*.wav")) {
    (void)f;
    ++wavCount;
  }
  EXPECT(wavCount >= 23, "expected at least 23 wav files with stems");

  EXPECT(kit.getChildFile("Memphis_Pattern.mid").existsAsFile(), "pattern midi");
  EXPECT(kit.getChildFile("PACK_MANIFEST.json").existsAsFile(), "manifest");

  PackExportOptions custom;
  custom.primaryBpm = 97.5f;
  custom.includeClassicTempos = false;
  custom.exportMidi = false;
  custom.exportStems = false;
  custom.writeMpcManifest = false;
  const juce::File tmpCustom = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                   .getChildFile("memphis_phonk_custom_bpm");
  tmpCustom.deleteRecursively();
  tmpCustom.createDirectory();
  EXPECT(engine.exportMpcKit(tmpCustom, custom).contains("Exported"), "custom bpm export");
  EXPECT(tmpCustom.getChildFile("Memphis_Phonk_MPC_Kit")
             .getChildFile("Memphis_Loop_98BPM.wav")
             .existsAsFile(),
         "loop at rounded BPM");

  DrumPattern sparse = DrumPattern::trapSparse();
  const auto sparseLoop = engine.generateDrumLoop(120.0f, 1, 0.0f, sparse);
  EXPECT(peak(sparseLoop) > 0.02f, "sparse pattern loop");

  opts.pattern = sparse;
  opts.batchBpmSweep = true;
  opts.includeClassicTempos = false;
  opts.exportStems = false;
  const juce::File batchTmp = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                  .getChildFile("memphis_phonk_batch_test");
  batchTmp.deleteRecursively();
  batchTmp.createDirectory();
  const juce::String batchMsg = engine.exportBatchKits(batchTmp, 2, opts);
  EXPECT(batchMsg.contains("Batch exported"), batchMsg.toStdString());

  if (failures == 0) {
    std::cout << "MemphisPhonkEngineTests: OK\n";
  }
  return failures == 0 ? 0 : 1;
}
