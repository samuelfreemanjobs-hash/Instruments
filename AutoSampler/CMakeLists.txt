# Auto Sampler — subdirectory of the Instruments monorepo root CMake project.

juce_add_plugin (AutoSampler
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.autosampler
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE AuSa
    FORMATS VST3 Standalone
    PRODUCT_NAME "Auto Sampler"
)

target_sources (AutoSampler
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Engine/MultisampleProgram.cpp
        Source/Engine/AutoRecordEngine.cpp
        Source/Engine/SamplePostProcessor.cpp
        Source/Engine/SamplerEngine.cpp
        Source/Export/MultisampleExporter.cpp
)

target_compile_definitions (AutoSampler
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (AutoSampler
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (AutoSampler PRIVATE Source)

juce_generate_juce_header (AutoSampler)

add_executable (AutoSamplerBatchExport
    tools/AutoSamplerBatchExport.cpp
    Source/Engine/MultisampleProgram.cpp
    Source/Export/MultisampleExporter.cpp
)

target_include_directories (AutoSamplerBatchExport
    PRIVATE
        Source
        "${CMAKE_BINARY_DIR}/AutoSampler/AutoSampler_artefacts/JuceLibraryCode"
)
target_link_libraries (AutoSamplerBatchExport
    PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_data_structures
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_core
    PUBLIC
        juce::juce_recommended_config_flags
)
add_dependencies (AutoSamplerBatchExport AutoSampler)
target_compile_definitions (AutoSamplerBatchExport
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

add_executable (AutoSamplerTests
    Tests/AutoSamplerTests.cpp
    Source/Engine/MultisampleProgram.cpp
    Source/Engine/AutoRecordEngine.cpp
    Source/Engine/SamplePostProcessor.cpp
    Source/Export/MultisampleExporter.cpp
)

target_include_directories (AutoSamplerTests
    PRIVATE
        Source
        "${CMAKE_BINARY_DIR}/AutoSampler/AutoSampler_artefacts/JuceLibraryCode"
)
target_compile_features (AutoSamplerTests PRIVATE cxx_std_20)
target_compile_definitions (AutoSamplerTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries (AutoSamplerTests
    PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_data_structures
        juce::juce_gui_basics
        juce::juce_core
    PUBLIC
        juce::juce_recommended_config_flags
)
add_dependencies (AutoSamplerTests AutoSampler)

add_test (NAME AutoSamplerUnit COMMAND AutoSamplerTests)
