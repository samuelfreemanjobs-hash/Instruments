#include "MultiScreenManager.h"
#include "ModeScreenComponent.h"

namespace vmpc::app
{
namespace
{
class ModeDocumentWindow : public juce::DocumentWindow
{
public:
    ModeDocumentWindow(MultiScreenManager& manager,
                       vmpc::model::AppMode mode,
                       controller::AppController& controller,
                       juce::Rectangle<int> bounds)
        : DocumentWindow("VMPC — " + vmpc::model::appModeToString(mode),
                         juce::Colour(0xff0c0c0e),
                         DocumentWindow::allButtons)
        , screenManager(manager)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new ModeScreenComponent(mode, controller), true);
        setResizable(true, true);
        setBounds(bounds);
        setVisible(true);
        toFront(true);
    }

    void closeButtonPressed() override { screenManager.handleWindowClosing(this); }

private:
    MultiScreenManager& screenManager;
};
} // namespace

MultiScreenManager::MultiScreenManager(controller::AppController& controller)
    : appController(controller)
{
}

void MultiScreenManager::openAllModeScreens()
{
    windows.clear();
    openCount = 0;

    const auto modes = allModes();
    for (int i = 0; i < static_cast<int>(modes.size()); ++i)
    {
        const auto mode = modes[static_cast<size_t>(i)];
        windows.push_back(
            std::make_unique<ModeDocumentWindow>(*this, mode, appController, boundsForModeScreen(i)));
        ++openCount;
    }

    appController.setAppMode(vmpc::model::AppMode::Electribe);
}

bool MultiScreenManager::anyWindowOpen() const
{
    return openCount > 0;
}

void MultiScreenManager::handleWindowClosing(juce::DocumentWindow* window)
{
    for (auto it = windows.begin(); it != windows.end(); ++it)
    {
        if (it->get() != window)
            continue;

        juce::DocumentWindow* const raw = window;
        windows.erase(it);
        --openCount;

        juce::MessageManager::callAsync([raw]() { delete raw; });
        break;
    }

    if (openCount <= 0)
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
} // namespace vmpc::app
