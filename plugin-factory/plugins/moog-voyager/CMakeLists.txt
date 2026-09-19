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
)

add_executable(VoyagerBassDemo VoyagerBassDemo.cpp)
target_link_libraries(VoyagerBassDemo PRIVATE MoogVoyager juce::juce_audio_formats)
target_compile_definitions(VoyagerBassDemo PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_include_directories(VoyagerBassDemo PRIVATE
    "${CMAKE_BINARY_DIR}/plugins/moog-voyager/MoogVoyager_artefacts/JuceLibraryCode")
add_dependencies(VoyagerBassDemo MoogVoyager)
