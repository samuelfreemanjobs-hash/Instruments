#include "MixerBank.h"
#include "ChannelInsertPanel.h"

namespace resonance::view
{
namespace
{
class InsertWindow : public juce::DocumentWindow
{
public:
    InsertWindow(int channel, resonance::audio::PluginHostService& host)
        : DocumentWindow("Channel " + juce::String(channel + 1) + " inserts",
                         juce::Colour(0xff111318),
                         DocumentWindow::closeButton)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new ChannelInsertPanel(host, channel), true);
        setResizable(true, true);
        centreWithSize(420, 160);
        setVisible(true);
    }

    void closeButtonPressed() override { setVisible(false); }
};
} // namespace

MixerBank::MixerBank(controller::AppController& controller)
    : appController(controller)
{
    title.setColour(juce::Label::textColourId, juce::Colour(0xff38bdf8));
    addAndMakeVisible(title);

    for (int i = 0; i < resonance::audio::MixConsole::kNumChannels; ++i)
    {
        strips[static_cast<size_t>(i)] = std::make_unique<MixerChannelStrip>("CH " + juce::String(i + 1));
        addAndMakeVisible(*strips[static_cast<size_t>(i)]);

        insertButtons[static_cast<size_t>(i)].setButtonText("Ins");
        insertButtons[static_cast<size_t>(i)].addListener(this);
        addAndMakeVisible(insertButtons[static_cast<size_t>(i)]);

        auto& fader = strips[static_cast<size_t>(i)]->getFader();
        fader.setValue(appController.getPluginHost().getMixConsole().getChannelFader(i), juce::dontSendNotification);
        fader.onValueChange = [this, i]() {
            appController.getPluginHost().getMixConsole().setChannelFader(
                i, static_cast<float>(strips[static_cast<size_t>(i)]->getFader().getValue()));
            appController.persistMixingState();
        };
    }
}

MixerBank::~MixerBank() = default;

void MixerBank::resized()
{
    auto area = getLocalBounds().reduced(4);
    title.setBounds(area.removeFromTop(20));
    area.removeFromTop(4);

    const int stripW = juce::jmax(56, area.getWidth() / resonance::audio::MixConsole::kNumChannels);
    for (int i = 0; i < resonance::audio::MixConsole::kNumChannels; ++i)
    {
        auto col = area.removeFromLeft(stripW).reduced(2);
        insertButtons[static_cast<size_t>(i)].setBounds(col.removeFromBottom(22));
        strips[static_cast<size_t>(i)]->setBounds(col);
    }
}

void MixerBank::buttonClicked(juce::Button* button)
{
    for (int i = 0; i < resonance::audio::MixConsole::kNumChannels; ++i)
    {
        if (button == &insertButtons[static_cast<size_t>(i)])
        {
            openInsertsForChannel(i);
            return;
        }
    }
}

void MixerBank::openInsertsForChannel(int channelIndex)
{
    insertWindow = std::make_unique<InsertWindow>(channelIndex, appController.getPluginHost());
}

void MixerBank::meterUpdate(int channel, float peak)
{
    if (channel >= 0 && channel < resonance::audio::MixConsole::kNumChannels && strips[static_cast<size_t>(channel)] != nullptr)
        strips[static_cast<size_t>(channel)]->meterUpdate(peak);
}
} // namespace resonance::view
