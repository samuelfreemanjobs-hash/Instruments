#include "ChopModalComponent.h"

#include "../Engine/SegmentEditor.h"
#include "../SP1200Constants.h"

ChopModalComponent::ChopModalComponent (sp1200::SamplerEngine& engine,
                                        std::size_t segmentIndex,
                                        std::function<void()> onClosed)
    : engine_ (engine), segmentIndex_ (segmentIndex), onClosed_ (std::move (onClosed))
{
    titleLabel_.setText ("MOD 11 — CHOP / TRUNCATE", juce::dontSendNotification);
    titleLabel_.setFont (juce::FontOptions (16.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel_);

    truncateButton_.onClick = [this]
    {
        if (sp1200::truncateSegmentInPlace (engine_.memoryPool(), segmentIndex_, startSample_, endSample_))
            refreshWaveform();
    };
    addAndMakeVisible (truncateButton_);

    chop8Button_.onClick = [this]
    {
        const auto created = engine_.autoChopSegment (segmentIndex_, 8);
        if (! created.empty())
        {
            segmentIndex_ = created.front();
            refreshWaveform();
        }
    };
    chop16Button_.onClick = [this]
    {
        const auto created = engine_.autoChopSegment (segmentIndex_, 16);
        if (! created.empty())
        {
            segmentIndex_ = created.front();
            refreshWaveform();
        }
    };
    addAndMakeVisible (chop8Button_);
    addAndMakeVisible (chop16Button_);

    normalizeButton_.onClick = [this]
    {
        sp1200::normalizeSegmentInPlace (engine_.memoryPool(), segmentIndex_);
        refreshWaveform();
    };
    reverseButton_.onClick = [this]
    {
        sp1200::reverseSegmentInPlace (engine_.memoryPool(), segmentIndex_);
        refreshWaveform();
    };
    addAndMakeVisible (normalizeButton_);
    addAndMakeVisible (reverseButton_);

    previewButton_.onClick = [this] { engine_.previewSegment (segmentIndex_, startSample_, endSample_); };
    addAndMakeVisible (previewButton_);

    closeButton_.onClick = [this]
    {
        if (onClosed_)
            onClosed_();
    };
    addAndMakeVisible (closeButton_);

    refreshWaveform();
}

void ChopModalComponent::setEmbeddedMode (bool embedded) noexcept
{
    embeddedMode_ = embedded;
    closeButton_.setVisible (! embedded);
}

void ChopModalComponent::refreshWaveform()
{
    const auto* seg = engine_.memoryPool().getSegment (segmentIndex_);
    if (seg == nullptr)
    {
        waveform_.clear();
        startSample_ = 0;
        endSample_ = 0;
        repaint();
        return;
    }

    waveform_ = sp1200::segmentToFloat (*seg);
    startSample_ = 0;
    endSample_ = static_cast<std::int64_t> (waveform_.size());
    if (const auto* s = engine_.memoryPool().getSegment (segmentIndex_))
        titleLabel_.setText ("MOD 11 — " + juce::String (s->name), juce::dontSendNotification);
    repaint();
}

juce::Rectangle<int> ChopModalComponent::waveformBounds() const
{
    auto r = getLocalBounds().reduced (16);
    r.removeFromTop (36);
    r.removeFromBottom (48);
    return r.reduced (8, 24);
}

void ChopModalComponent::sampleFromX (int x, std::int64_t& outSample) const
{
    const auto wb = waveformBounds();
    if (waveform_.empty() || wb.isEmpty())
    {
        outSample = 0;
        return;
    }
    const float t = static_cast<float> (x - wb.getX()) / static_cast<float> (wb.getWidth());
    const auto n = static_cast<std::int64_t> (waveform_.size());
    outSample = std::clamp (static_cast<std::int64_t> (t * static_cast<float> (n)), std::int64_t { 0 }, n);
}

void ChopModalComponent::mouseDown (const juce::MouseEvent& e)
{
    if (! waveformBounds().contains (e.getPosition()))
        return;

    std::int64_t s = 0;
    sampleFromX (e.x, s);
    const float startDist = static_cast<float> (std::abs (s - startSample_));
    const float endDist = static_cast<float> (std::abs (s - endSample_));
    drag_ = startDist <= endDist ? DragTarget::start : DragTarget::end;
}

void ChopModalComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (drag_ == DragTarget::none)
        return;

    std::int64_t s = 0;
    sampleFromX (e.x, s);
    const auto n = static_cast<std::int64_t> (waveform_.size());
    if (drag_ == DragTarget::start)
    {
        startSample_ = std::clamp (s, std::int64_t { 0 }, std::max (std::int64_t { 0 }, endSample_ - 1));
    }
    else
    {
        endSample_ = std::clamp (s, startSample_ + 1, n);
    }
    repaint();
}

void ChopModalComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xee2a2520));

    const auto wb = waveformBounds();
    g.setColour (juce::Colour (0xff1a1814));
    g.fillRoundedRectangle (wb.toFloat(), 6.0f);
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawRoundedRectangle (wb.toFloat(), 6.0f, 1.5f);

    if (waveform_.size() >= 2)
    {
        juce::Path path;
        const float midY = wb.getCentreY();
        const float halfH = wb.getHeight() * 0.42f;
        const float x0 = static_cast<float> (wb.getX());
        const float w = static_cast<float> (wb.getWidth());
        for (std::size_t i = 0; i < waveform_.size(); ++i)
        {
            const float x = x0 + (static_cast<float> (i) / static_cast<float> (waveform_.size() - 1)) * w;
            const float y = midY - waveform_[i] * halfH;
            if (i == 0)
                path.startNewSubPath (x, y);
            else
                path.lineTo (x, y);
        }
        g.setColour (juce::Colour (0xff4ade80));
        g.strokePath (path, juce::PathStrokeType (1.2f));

        const auto n = static_cast<std::int64_t> (waveform_.size());
        const auto xFor = [&] (std::int64_t sample) -> float
        {
            return x0 + (static_cast<float> (sample) / static_cast<float> (std::max (std::int64_t { 1 }, n))) * w;
        };
        const float xs = xFor (startSample_);
        const float xe = xFor (endSample_);
        g.setColour (juce::Colour (0x44fbbf24));
        g.fillRect (xs, static_cast<float> (wb.getY()), xe - xs, static_cast<float> (wb.getHeight()));
        g.setColour (juce::Colours::orange);
        g.drawLine (xs, static_cast<float> (wb.getY()), xs, static_cast<float> (wb.getBottom()), 2.0f);
        g.setColour (juce::Colours::deepskyblue);
        g.drawLine (xe, static_cast<float> (wb.getY()), xe, static_cast<float> (wb.getBottom()), 2.0f);
    }

    g.setColour (juce::Colours::white.withAlpha (0.7f));
    g.setFont (11.0f);
    g.drawText ("Drag orange (start) and blue (end) handles · TRUNCATE commits range",
                wb.withY (wb.getBottom() + 4).withHeight (18),
                juce::Justification::centredLeft);
}

void ChopModalComponent::resized()
{
    auto r = getLocalBounds().reduced (12);
    titleLabel_.setBounds (r.removeFromTop (28));
    auto buttons = r.removeFromBottom (40);
    const int bw = 88;
    truncateButton_.setBounds (buttons.removeFromLeft (bw).reduced (2));
    chop8Button_.setBounds (buttons.removeFromLeft (bw).reduced (2));
    chop16Button_.setBounds (buttons.removeFromLeft (bw).reduced (2));
    normalizeButton_.setBounds (buttons.removeFromLeft (bw).reduced (2));
    reverseButton_.setBounds (buttons.removeFromLeft (bw).reduced (2));
    previewButton_.setBounds (buttons.removeFromLeft (bw).reduced (2));
    closeButton_.setBounds (buttons.removeFromRight (bw).reduced (2));
}
