/**
 * Contract: MeterTap editor offscreen renders at 100% and 200% for the configured
 * theme; key pixels match DesignTokens (over cap = meter.over; bypass LED = state.caution).
 *
 * PNGs → METERTAP_RENDER_OUT_DIR/<theme>/MeterTapEditor@{scale}x.png
 */
#include "../Source/PluginEditor.h"
#include "../Source/PluginProcessor.h"

#include <FactoryGui.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <iostream>
#include <vector>

namespace {

struct Failure { juce::String message; };
std::vector<Failure> failures;
int checks = 0;

void expect(bool cond, const juce::String& msg)
{
  ++checks;
  if (!cond)
    failures.push_back({msg});
}

bool colourClose(juce::Colour a, juce::Colour b, int tol = 24)
{
  return std::abs((int)a.getRed() - (int)b.getRed()) <= tol
      && std::abs((int)a.getGreen() - (int)b.getGreen()) <= tol
      && std::abs((int)a.getBlue() - (int)b.getBlue()) <= tol;
}

bool imageNonBlank(const juce::Image& img)
{
  if (!img.isValid() || img.getWidth() < 2 || img.getHeight() < 2)
    return false;
  const auto first = img.getPixelAt(0, 0);
  for (int y = 0; y < img.getHeight(); y += juce::jmax(1, img.getHeight() / 16))
    for (int x = 0; x < img.getWidth(); x += juce::jmax(1, img.getWidth() / 16))
      if (img.getPixelAt(x, y) != first)
        return true;
  return first.getAlpha() > 0;
}

juce::Image renderComponent(juce::Component& c, float scale)
{
  const int w = juce::jmax(1, (int)std::ceil(c.getWidth() * scale));
  const int h = juce::jmax(1, (int)std::ceil(c.getHeight() * scale));
  juce::Image img(juce::Image::ARGB, w, h, true);
  juce::Graphics g(img);
  g.addTransform(juce::AffineTransform::scale(scale));
  c.paintEntireComponent(g, true);
  return img;
}

void writePng(const juce::Image& img, const juce::File& file)
{
  file.getParentDirectory().createDirectory();
  juce::FileOutputStream stream(file);
  if (stream.openedOk()) {
    juce::PNGImageFormat png;
    png.writeImageToStream(img, stream);
  }
}

juce::File outDir()
{
#ifdef METERTAP_RENDER_OUT_DIR
  return juce::File(METERTAP_RENDER_OUT_DIR).getChildFile(fds::themeId);
#else
  return juce::File::getCurrentWorkingDirectory()
      .getChildFile("editor-renders")
      .getChildFile(fds::themeId);
#endif
}

void runScale(float scale)
{
  MeterTapAudioProcessor proc;
  proc.prepareToPlay(48000.0, 512);
  MeterTapAudioProcessorEditor editor(proc);
  editor.setSize(480, 440);
  editor.resized();

  // Latch over on both meters + bypass caution LED for pixel asserts
  editor.setTestOverLatched(true, true);
  editor.setTestBypass(true);
  editor.forcePaintForTest();

  auto img = renderComponent(editor, scale);
  expect(imageNonBlank(img), "MeterTapEditor@" + juce::String(scale) + " blank");

  // Chassis background
  expect(colourClose(img.getPixelAt(2, 2), fds::gui::argb(fds::color::bg::chassis), 30),
         "chassis bg @" + juce::String(scale));

  // Over cap L centre ≈ (64+18, 92+3) = (82, 95) at 100%
  {
    const int cx = (int)std::round(82.0f * scale);
    const int cy = (int)std::round(95.0f * scale);
    const auto pix = img.getPixelAt(juce::jlimit(0, img.getWidth() - 1, cx),
                                    juce::jlimit(0, img.getHeight() - 1, cy));
    expect(colourClose(pix, fds::gui::argb(fds::color::meter::over), 50),
           "over cap L pixel " + juce::String::toHexString((int)pix.getARGB())
               + " != meter.over @" + juce::String(scale));
  }

  // Bypass LED region — header bypass sits near right; sample LED area roughly
  // Header bypass bounds from HeaderBar: right side. Sample around (430, 20).
  {
    const int bx = (int)std::round(410.0f * scale);
    const int by = (int)std::round(20.0f * scale);
    // Scan a small neighbourhood for caution colour (LED glow)
    bool foundCaution = false;
    for (int dy = -8; dy <= 8 && !foundCaution; ++dy)
      for (int dx = -16; dx <= 16 && !foundCaution; ++dx) {
        const int x = juce::jlimit(0, img.getWidth() - 1, bx + (int)std::round(dx * scale));
        const int y = juce::jlimit(0, img.getHeight() - 1, by + (int)std::round(dy * scale / 2));
        if (colourClose(img.getPixelAt(x, y), fds::gui::argb(fds::color::state::caution), 55))
          foundCaution = true;
      }
    expect(foundCaution, "bypass LED caution colour not found @" + juce::String(scale));
  }

  // Caution title dot when bypassed at (28,66)+centre ≈ (31, 69)
  {
    const int dx = (int)std::round(31.0f * scale);
    const int dy = (int)std::round(69.0f * scale);
    const auto pix = img.getPixelAt(juce::jlimit(0, img.getWidth() - 1, dx),
                                    juce::jlimit(0, img.getHeight() - 1, dy));
    expect(colourClose(pix, fds::gui::argb(fds::color::state::caution), 55),
           "bypass title caution dot @" + juce::String(scale));
  }

  writePng(img, outDir().getChildFile("MeterTapEditor@" + juce::String(scale, 1) + "x.png"));
  // Also write over/bypass state variants for evidence
  writePng(img, outDir().getChildFile("MeterTapEditor-over-bypass@" + juce::String(scale, 1) + "x.png"));

  proc.releaseResources();
}

} // namespace

int main(int argc, char** argv)
{
  juce::ignoreUnused(argc, argv);
  juce::ScopedJuceInitialiser_GUI juceInit;

  outDir().createDirectory();
  std::cout << "MeterTapEditorRenderTests theme=" << fds::themeId
            << " brand=" << fds::brandName << std::endl;

  for (float scale : {1.0f, 2.0f})
    runScale(scale);

  std::cout << "checks=" << checks << " failures=" << failures.size() << std::endl;
  for (auto& f : failures)
    std::cerr << "FAIL: " << f.message << std::endl;

  if (failures.empty()) {
    std::cout << "PASS MeterTapEditorRenderTests [" << fds::themeId << "]" << std::endl;
    return 0;
  }
  return 1;
}
