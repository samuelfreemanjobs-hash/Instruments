pf_add_juce_plugin(
    TARGET MoogVoyager
    PRODUCT_NAME "V Voyager"
    PLUGIN_CODE OfqX
    MANUFACTURER_CODE Inst
    BUNDLE_ID com.instruments.moog-voyager
    IS_SYNTH TRUE
    SOURCES
        SynthParameterIds.h
        SynthProcessor.h
        SynthProcessor.cpp
        SynthEditor.h
        SynthEditor.cpp
        GfunkPresets.h
        GfunkPresets.cpp
)

add_executable(VoyagerBassDemo VoyagerBassDemo.cpp)
target_link_libraries(VoyagerBassDemo PRIVATE MoogVoyager juce::juce_audio_formats)
target_compile_definitions(VoyagerBassDemo PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_include_directories(VoyagerBassDemo PRIVATE
    "${CMAKE_BINARY_DIR}/plugins/moog-voyager/MoogVoyager_artefacts/JuceLibraryCode")
add_dependencies(VoyagerBassDemo MoogVoyager)

add_executable(VoyagerLeadDemo VoyagerLeadDemo.cpp)
target_link_libraries(VoyagerLeadDemo PRIVATE MoogVoyager juce::juce_audio_formats)
target_compile_definitions(VoyagerLeadDemo PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_include_directories(VoyagerLeadDemo PRIVATE
    "${CMAKE_BINARY_DIR}/plugins/moog-voyager/MoogVoyager_artefacts/JuceLibraryCode")
add_dependencies(VoyagerLeadDemo MoogVoyager)
