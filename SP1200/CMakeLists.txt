# SP-1200-style standalone sampler (Instruments monorepo)

juce_add_plugin (SP1200
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.sp1200
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Sp12
    FORMATS Standalone
    PRODUCT_NAME "SP-1200 Drumulator"
)

target_sources (SP1200
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/UI/PianoRollComponent.cpp
        Source/UI/StepStackPanel.cpp
        Source/Memory/TwelveBitBuffer.cpp
        Source/Memory/SampleMemoryPool.cpp
        Source/Import/SampleImporter.cpp
        Source/Engine/SampleVoice.cpp
        Source/Engine/SamplerEngine.cpp
        Source/Engine/PatternSequencer.cpp
        Source/Engine/MidiMapping.cpp
        Source/Engine/SegmentEditor.cpp
        Source/Import/TransientChop.cpp
        Source/DSP/Ssm2044BusFilter.cpp
        Source/Project/ProjectFile.cpp
        Source/UI/ChopModalComponent.cpp
        Source/UI/LcdPanelComponent.cpp
        Source/UI/KeypadComponent.cpp
)

target_compile_definitions (SP1200
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (SP1200
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_formats
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (SP1200 PRIVATE Source)

juce_generate_juce_header (SP1200)

add_executable (SP1200MemoryTests
    Tests/MemoryTests.cpp
    Source/Memory/TwelveBitBuffer.cpp
    Source/Memory/SampleMemoryPool.cpp
    Source/Engine/SegmentEditor.cpp
)

add_executable (SP1200SequencerTests
    Tests/SequencerTests.cpp
    Source/Engine/PatternSequencer.cpp
    Source/Import/TransientChop.cpp
)

add_executable (SP1200ProjectFileTests
    Tests/ProjectFileTests.cpp
    Source/Engine/SamplerEngine.cpp
    Source/Engine/SampleVoice.cpp
    Source/Engine/PatternSequencer.cpp
    Source/Engine/SegmentEditor.cpp
    Source/Memory/TwelveBitBuffer.cpp
    Source/Memory/SampleMemoryPool.cpp
    Source/Import/SampleImporter.cpp
    Source/Import/TransientChop.cpp
    Source/DSP/Ssm2044BusFilter.cpp
    Source/Project/ProjectFile.cpp
    Source/Engine/MidiMapping.cpp
)

foreach (t SP1200MemoryTests SP1200SequencerTests SP1200ProjectFileTests)
    target_include_directories (${t} PRIVATE Source)
    target_compile_features (${t} PRIVATE cxx_std_20)
endforeach()

target_compile_definitions (SP1200ProjectFileTests PRIVATE JUCE_USE_CURL=0)
target_link_libraries (SP1200ProjectFileTests PRIVATE juce::juce_core juce::juce_audio_basics juce::juce_dsp juce::juce_audio_formats)

add_test (NAME SP1200Memory COMMAND SP1200MemoryTests)
add_test (NAME SP1200Sequencer COMMAND SP1200SequencerTests)
add_test (NAME SP1200ProjectFile COMMAND SP1200ProjectFileTests)
