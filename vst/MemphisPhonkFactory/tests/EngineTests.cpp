#include <cmath>
#include <iostream>
#include <juce_core/juce_core.h>
#include "Dsp/MemphisPhonkEngine.h"

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

  const auto loop = engine.generateDrumLoop(118.0f, 2, 0.03f);
  EXPECT(loop.size() > 44100, "loop should be at least one second");

  const juce::File tmp = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("memphis_phonk_test");
  tmp.deleteRecursively();
  tmp.createDirectory();
  const juce::String result = engine.exportMpcKit(tmp, 0.03f);
  EXPECT(result.contains("Exported"), result.toStdString());

  const juce::File kit = tmp.getChildFile("Memphis_Phonk_MPC_Kit");
  int wavCount = 0;
  for (auto &f : kit.findChildFiles(juce::File::findFiles, false, "*.wav")) {
    (void)f;
    ++wavCount;
  }
  EXPECT(wavCount == 23, "expected 23 wav files");

  if (failures == 0) {
    std::cout << "MemphisPhonkEngineTests: OK\n";
  }
  return failures == 0 ? 0 : 1;
}
