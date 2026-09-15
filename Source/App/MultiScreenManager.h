#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include "Controller/AppController.h"
#include "DisplayLayout.h"

namespace vmpc::app
{
/** One top-level window per workflow mode, placed across connected displays. */
class MultiScreenManager
{
public:
    explicit MultiScreenManager(controller::AppController& controller);

    void openAllModeScreens();
    bool anyWindowOpen() const;

    void handleWindowClosing(juce::DocumentWindow* window);

private:
    controller::AppController& appController;
    std::vector<std::unique_ptr<juce::DocumentWindow>> windows;
    int openCount = 0;
};
} // namespace vmpc::app
