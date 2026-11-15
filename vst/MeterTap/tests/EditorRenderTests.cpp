/**
 * Contract: MeterTap editor offscreen renders at 100% and 200% for the configured
 * theme; key pixels match DesignTokens; PNG files are single-image (one IEND);
 * normal vs over+bypass renders differ; titles use real UTF-8 middle-dot (no mojibake);
 * RMS wells always show a value.
 *
 * PNGs → METERTAP_RENDER_OUT_DIR/<theme>/MeterTapEditor[@scale] / -over-bypass@
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

/** Count PNG IEND chunks; a valid single image has exactly one, at EOF-12. */
int countPngIend(const juce::File& file)
{
  juce::MemoryBlock mb;
  if (!file.loadFileAsData(mb) || mb.getSize() < 12)
    return 0;
  const auto* data = static_cast<const unsigned char*>(mb.getData());
  const size_t n = mb.getSize();
  int count = 0;
  // PNG chunk type is 4 bytes at offset 4 within each chunk; scan for IEND
  for (size_t i = 0; i + 8 <= n; ++i) {
    if (data[i] == 'I' && data[i + 1] == 'E' && data[i + 2] == 'N' && data[i + 3] == 'D')
      ++count;
  }
  return count;
}

bool pngIendAtEof(const juce::File& file)
{
  juce::MemoryBlock mb;
  if (!file.loadFileAsData(mb) || mb.getSize() < 12)
    return false;
  // IEND chunk: 4 length(0) + 'IEND' + 4 CRC = 12 bytes at end
  const auto* data = static_cast<const unsigned char*>(mb.getData());
  const size_t n = mb.getSize();
  return data[n - 8] == 'I' && data[n - 7] == 'E' && data[n - 6] == 'N' && data[n - 5] == 'D';
}

void writePng(const juce::Image& img, const juce::File& file)
{
  file.getParentDirectory().createDirectory();
  // FileOutputStream appends by default — truncate so we never concatenate PNGs.
  if (file.existsAsFile())
    file.deleteFile();
  juce::FileOutputStream stream(file);
  if (stream.openedOk()) {
    stream.setPosition(0);
    stream.truncate();
    juce::PNGImageFormat png;
    png.writeImageToStream(img, stream);
    stream.flush();
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

bool imagesDiffer(const juce::Image& a, const juce::Image& b)
{
  if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight())
    return true;
  for (int y = 0; y < a.getHeight(); y += juce::jmax(1, a.getHeight() / 32))
    for (int x = 0; x < a.getWidth(); x += juce::jmax(1, a.getWidth() / 32))
      if (a.getPixelAt(x, y) != b.getPixelAt(x, y))
        return true;
  return false;
}

void assertUtf8Title(const juce::String& title, float scale)
{
  // Must contain U+00B7 middle dot, must NOT contain the mojibake sequence "Â·"
  const juce::String mid(juce::CharPointer_UTF8("\xC2\xB7"));
  const juce::String mojibake(juce::CharPointer_UTF8("\xC3\x82\xC2\xB7")); // Â·
  expect(title.contains(mid), "meter title missing U+00B7 middle-dot @" + juce::String(scale)
                                  + " got=[" + title + "]");
  expect(!title.contains(mojibake) && !title.containsChar((juce::juce_wchar)0x00C2),
         "meter title has mojibake @" + juce::String(scale) + " got=[" + title + "]");
  expect(title.contains("METERS") && (title.contains("INPUT") || title.contains("OUTPUT")),
         "meter title content @" + juce::String(scale) + " got=[" + title + "]");
}

void assertReadoutNonEmpty(fds::gui::ValueReadout& r, const juce::String& name, float scale)
{
  const auto t = r.getValueText();
  expect(t.isNotEmpty(), name + " readout empty @" + juce::String(scale));
  // Must be −∞ or a signed one-decimal value
  const juce::String inf(juce::CharPointer_UTF8("\xe2\x88\x92\xe2\x88\x9e"));
  const juce::String minus(juce::CharPointer_UTF8("\xe2\x88\x92"));
  const bool ok = (t == inf) || t.startsWith("+") || t.startsWith(minus);
  expect(ok, name + " readout format @" + juce::String(scale) + " got=[" + t + "]");
  expect(!t.contains("...") && !t.containsChar((juce::juce_wchar)0x2026),
         name + " readout text must not contain ellipsis @" + juce::String(scale)
             + " got=[" + t + "]");
  expect(r.valueTextFitsWithoutEllipsis(),
         name + " value must fit well without ellipsis @" + juce::String(scale)
             + " text=[" + t + "] width=" + juce::String(r.getValueLabelBounds().getWidth()));
}

void assertReadoutExact(fds::gui::ValueReadout& r, const juce::String& expected,
                        const juce::String& name, float scale)
{
  const auto t = r.getValueText();
  expect(t == expected,
         name + " readout must equal [" + expected + "] got=[" + t + "] @" + juce::String(scale));
  expect(r.valueTextFitsWithoutEllipsis(),
         name + " exact value must fit without ellipsis @" + juce::String(scale));
}

void assertProductNameInsideHeader(MeterTapAudioProcessorEditor& editor, float scale)
{
  auto& header = editor.getHeader();
  const auto nameBounds = header.getProductNameBounds();
  const auto headerBounds = header.getLocalBounds();
  expect(headerBounds.contains(nameBounds),
         "productName bounds must lie inside header @" + juce::String(scale)
             + " name=" + nameBounds.toString() + " header=" + headerBounds.toString());
  // Descenders must not be clipped: name height >= productName lineHeight (or full avail).
  const int need = fds::typography::style::productName.lineHeight;
  expect(nameBounds.getHeight() >= juce::jmin(need, headerBounds.getHeight()),
         "productName height must cover lineHeight @" + juce::String(scale)
             + " got=" + juce::String(nameBounds.getHeight()) + " need=" + juce::String(need));
  expect(nameBounds.getBottom() <= headerBounds.getBottom(),
         "productName bottom must not exceed header @" + juce::String(scale));
}

void runScale(float scale)
{
  MeterTapAudioProcessor proc;
  proc.prepareToPlay(48000.0, 512);
  MeterTapAudioProcessorEditor editor(proc);
  editor.setSize(480, 440);
  editor.resized();
  editor.seedSilenceReadouts();
  editor.forcePaintForTest();

  // --- Normal (silence, no bypass) ---
  editor.setTestBypass(false);
  editor.seedSilenceReadouts();
  editor.forcePaintForTest();
  auto imgNormal = renderComponent(editor, scale);
  expect(imageNonBlank(imgNormal), "normal blank @" + juce::String(scale));
  assertReadoutNonEmpty(editor.getRmsL(), "RMS L (normal)", scale);
  assertReadoutNonEmpty(editor.getRmsR(), "RMS R (normal)", scale);
  assertReadoutNonEmpty(editor.getPeakL(), "PEAK L (normal)", scale);
  {
    const juce::String inf(juce::CharPointer_UTF8("\xe2\x88\x92\xe2\x88\x9e"));
    assertReadoutExact(editor.getRmsL(), inf, "RMS L silence", scale);
  }
  expect(editor.getTrimValueText() == "+0.0 dB",
         "trim value must be '+0.0 dB' got=[" + editor.getTrimValueText() + "]");
  expect(editor.getPresetNameText() == "Init",
         "preset name must be 'Init' (as-is) got=[" + editor.getPresetNameText() + "]");
  assertUtf8Title(editor.getMeterTitleText(), scale);
  expect(editor.getMeterTitleText().contains("OUTPUT"),
         "normal title should be OUTPUT @" + juce::String(scale));
  assertProductNameInsideHeader(editor, scale);

  auto normalFile = outDir().getChildFile("MeterTapEditor@" + juce::String(scale, 1) + "x.png");
  writePng(imgNormal, normalFile);
  expect(countPngIend(normalFile) == 1, "normal PNG must have exactly one IEND @" + juce::String(scale)
                                            + " count=" + juce::String(countPngIend(normalFile)));
  expect(pngIendAtEof(normalFile), "normal PNG IEND must be at EOF @" + juce::String(scale));

  // --- Over + bypass ---
  editor.setTestOverLatched(true, true);
  editor.setTestBypass(true);
  editor.forcePaintForTest();
  auto imgOver = renderComponent(editor, scale);
  expect(imageNonBlank(imgOver), "over-bypass blank @" + juce::String(scale));
  assertReadoutNonEmpty(editor.getRmsL(), "RMS L (over)", scale);
  assertReadoutNonEmpty(editor.getRmsR(), "RMS R (over)", scale);
  assertReadoutNonEmpty(editor.getPeakL(), "PEAK L (over)", scale);
  {
    // Signal present: full one-decimal string, no "..." (design-spec 80 px wells, no unit).
    const juce::String expectedRms = juce::String(juce::CharPointer_UTF8("\xe2\x88\x92")) + "10.0";
    assertReadoutExact(editor.getRmsL(), expectedRms, "RMS L over-bypass", scale);
    assertReadoutExact(editor.getRmsR(), expectedRms, "RMS R over-bypass", scale);
    // Extreme signed widths that must still fit valueLarge in an 80 px well
    // (−96.0 is at numericFloor and becomes −∞; use −95.9 / +12.0 for digit fit).
    editor.setTestLevels(0, -6.0f, -95.9f, -6.0f, false);
    editor.forcePaintForTest();
    const juce::String nearFloor = juce::String(juce::CharPointer_UTF8("\xe2\x88\x92")) + "95.9";
    assertReadoutExact(editor.getRmsL(), nearFloor, "RMS L −95.9 fit", scale);
    editor.setTestLevels(0, 1.0f, 12.0f, 1.0f, true);
    editor.forcePaintForTest();
    assertReadoutExact(editor.getRmsL(), "+12.0", "RMS L +12.0 fit", scale);
    editor.setTestOverLatched(true, true);
    editor.forcePaintForTest();
    assertReadoutExact(editor.getRmsL(), expectedRms, "RMS L restored −10.0", scale);
    assertReadoutExact(editor.getRmsR(), expectedRms, "RMS R restored −10.0", scale);
  }
  assertUtf8Title(editor.getMeterTitleText(), scale);
  expect(editor.getMeterTitleText().contains("INPUT"),
         "bypass title should be INPUT @" + juce::String(scale));
  assertProductNameInsideHeader(editor, scale);

  // Chassis background
  expect(colourClose(imgOver.getPixelAt(2, 2), fds::gui::argb(fds::color::bg::chassis), 30),
         "chassis bg @" + juce::String(scale));

  // Over cap L centre ≈ (64+18, 92+3) = (82, 95) at 100%
  {
    const int cx = (int)std::round(82.0f * scale);
    const int cy = (int)std::round(95.0f * scale);
    const auto pix = imgOver.getPixelAt(juce::jlimit(0, imgOver.getWidth() - 1, cx),
                                        juce::jlimit(0, imgOver.getHeight() - 1, cy));
    expect(colourClose(pix, fds::gui::argb(fds::color::meter::over), 50),
           "over cap L pixel " + juce::String::toHexString((int)pix.getARGB())
               + " != meter.over @" + juce::String(scale));
  }

  // Bypass LED caution colour near header right
  {
    const int bx = (int)std::round(410.0f * scale);
    const int by = (int)std::round(20.0f * scale);
    bool foundCaution = false;
    for (int dy = -8; dy <= 8 && !foundCaution; ++dy)
      for (int dx = -16; dx <= 16 && !foundCaution; ++dx) {
        const int x = juce::jlimit(0, imgOver.getWidth() - 1, bx + (int)std::round(dx * scale));
        const int y = juce::jlimit(0, imgOver.getHeight() - 1, by + (int)std::round(dy * scale / 2));
        if (colourClose(imgOver.getPixelAt(x, y), fds::gui::argb(fds::color::state::caution), 55))
          foundCaution = true;
      }
    expect(foundCaution, "bypass LED caution colour not found @" + juce::String(scale));
  }

  // Caution title dot at (28,66)+centre ≈ (31, 69) — must not sit on the 'M'
  {
    const int dx = (int)std::round(31.0f * scale);
    const int dy = (int)std::round(69.0f * scale);
    const auto pix = imgOver.getPixelAt(juce::jlimit(0, imgOver.getWidth() - 1, dx),
                                        juce::jlimit(0, imgOver.getHeight() - 1, dy));
    expect(colourClose(pix, fds::gui::argb(fds::color::state::caution), 55),
           "bypass title caution dot @" + juce::String(scale));
  }

  expect(imagesDiffer(imgNormal, imgOver),
         "normal and over-bypass renders must differ @" + juce::String(scale));

  auto overFile =
      outDir().getChildFile("MeterTapEditor-over-bypass@" + juce::String(scale, 1) + "x.png");
  writePng(imgOver, overFile);
  expect(countPngIend(overFile) == 1, "over PNG must have exactly one IEND @" + juce::String(scale)
                                          + " count=" + juce::String(countPngIend(overFile)));
  expect(pngIendAtEof(overFile), "over PNG IEND must be at EOF @" + juce::String(scale));
  expect(normalFile.getSize() != overFile.getSize()
             || imagesDiffer(imgNormal, imgOver),
         "normal/over files must not be byte-identical evidence @" + juce::String(scale));

  // Second write must not concatenate (regression for FileOutputStream append bug)
  writePng(imgOver, overFile);
  expect(countPngIend(overFile) == 1, "rewritten over PNG still one IEND @" + juce::String(scale));

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
