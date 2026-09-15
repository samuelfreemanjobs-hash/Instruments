cmake_minimum_required(VERSION 3.22)

project(VMPC2000XL VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(juce)

juce_add_gui_app(VMPC2000XL
    PRODUCT_NAME "VMPC2000XL"
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.vmpc2000xl"
    ICON_BIG ""
    ICON_SMALL ""
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "VMPC2000XL needs microphone access for audio input."
)

juce_generate_juce_header(VMPC2000XL)

target_sources(VMPC2000XL
    PRIVATE
        Source/App/Main.cpp
        Source/App/MainComponent.cpp
        Source/Controller/AppController.cpp
        Source/Model/ProjectState.cpp
        Source/Model/HybridWorkspace.cpp
        Source/Model/Electribe/ElectribeSequencer.cpp
        Source/Model/Ableton/SessionMatrix.cpp
        Source/Model/FlStudio/PatternPlaylist.cpp
        Source/Model/Sequencer/SixteenStepPattern.cpp
        Source/Model/Sequencer/SequencerClock.cpp
        Source/Model/Sequencer/SequencerCore.cpp
        Source/Audio/AudioEngine.cpp
        Source/Audio/PluginSlotChain.cpp
        Source/Audio/MixConsole.cpp
        Source/Audio/PluginHostService.cpp
        Source/Model/MixingProjectPersistence.cpp
        Source/Audio/VibeMixEngine.cpp
        Source/Audio/Internal/InternalMixProcessor.cpp
        Source/Audio/Internal/InternalPluginFactory.cpp
        Source/Audio/Internal/GainProcessor.cpp
        Source/Audio/Internal/PanProcessor.cpp
        Source/Audio/Internal/Eq3Processor.cpp
        Source/Audio/Internal/CompressorProcessor.cpp
        Source/Audio/Internal/LimiterProcessor.cpp
        Source/Audio/Internal/HpfProcessor.cpp
        Source/Audio/Internal/DeEsserProcessor.cpp
        Source/View/Components/MixerBank.cpp
        Source/View/Components/ChannelInsertPanel.cpp
        Source/Audio/RealTimeConstraints.h
        Source/View/Components/PluginRackPanel.cpp
        Source/View/LookAndFeel/VMpcLookAndFeel.cpp
        Source/View/Components/Mpc2000xlLcdDisplay.cpp
        Source/View/Components/StepSequencerGrid.cpp
        Source/View/Components/PianoRollGrid.cpp
        Source/View/Components/MixerChannelStrip.cpp
        Source/View/Components/ModeToolbar.cpp
        Source/View/Modes/ElectribeModePanel.cpp
        Source/View/Modes/HybridMpcModePanel.cpp
        Source/View/Modes/MaschineModePanel.cpp
        Source/View/Modes/SessionModePanel.cpp
        Source/View/Modes/PatternModePanel.cpp
)

target_compile_definitions(VMPC2000XL
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_PLUGINHOST_VST3=1
        $<$<PLATFORM_ID:Linux>:JUCE_PLUGINHOST_LV2=1>
        $<$<PLATFORM_ID:Darwin>:JUCE_PLUGINHOST_AU=1>
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:VMPC2000XL,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="${PROJECT_VERSION}"
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(VMPC2000XL
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_warning_flags
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
)

target_include_directories(VMPC2000XL PRIVATE Source)
