#include "WheelV2Strip.h"

namespace junovax::ui::celestial
{
WheelV2Strip::WheelV2Strip (Kind kind) : kind_ (kind)
{
    setInterceptsMouseClicks (false, false);
}

void WheelV2Strip::paint (juce::Graphics& g)
{
    const bool pitch = kind_ == Kind::pitch;
    const auto accent = pitch ? Theme::stellarCrimson() : Theme::electricCyan();
    auto r = getLocalBounds().toFloat().reduced (2.0f);

    g.setFont (Theme::labelFont());
    g.setColour (accent);
    g.drawText (pitch ? "PITCH" : "MOD", r.removeFromTop (12.0f), juce::Justification::centred);

    g.setColour (Theme::ledInactive());
    g.fillRoundedRectangle (r.reduced (3.0f, 0.0f), 4.0f);

  #if ! JUCE_MINGW
    const float fillY = pitch ? r.getY() + r.getHeight() * 0.35f : r.getY() + r.getHeight() * 0.55f;
  #else
    const float fillY = r.getY() + r.getHeight() * (pitch ? 0.35f : 0.55f);
  #endif
    g.setColour (accent.withAlpha (0.95f));
    g.fillRoundedRectangle (r.getX() + 5.0f, fillY - 8.0f, r.getWidth() - 10.0f, 16.0f, 3.0f);

    g.setColour (Theme::textMuted());
    g.drawText ("v2", r.removeFromBottom (12.0f), juce::Justification::centred);
}
} // namespace junovax::ui::celestial