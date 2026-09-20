# SP-1200-style standalone sampler (Instruments monorepo)

juce_add_plugin (SP1200
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.sp1200
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
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
        Source/Memory/TwelveBitBuffer.cpp
        Source/Memory/SampleMemoryPool.cpp
        Source/Import/SampleImporter.cpp
        Source/Engine/SampleVoice.cpp
        Source/Engine/SamplerEngine.cpp
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

add_executable (SP1200Tests
    Tests/MemoryTests.cpp
    Source/Memory/TwelveBitBuffer.cpp
    Source/Memory/SampleMemoryPool.cpp
)

target_include_directories (SP1200Tests PRIVATE Source)
target_compile_features (SP1200Tests PRIVATE cxx_std_20)

add_test (NAME SP1200Memory COMMAND SP1200Tests)
