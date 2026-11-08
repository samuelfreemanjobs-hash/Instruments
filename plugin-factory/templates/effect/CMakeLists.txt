pf_add_juce_plugin(
    TARGET {{PLUGIN_TARGET}}
    PRODUCT_NAME "{{PLUGIN_PRODUCT_NAME}}"
    PLUGIN_CODE {{PLUGIN_CODE}}
    MANUFACTURER_CODE Inst
    BUNDLE_ID com.instruments.{{PLUGIN_DIR_SLUG}}
    IS_SYNTH FALSE
    SOURCES
        ParameterIds.h
        PluginProcessor.h
        PluginProcessor.cpp
        PluginEditor.h
        PluginEditor.cpp
)
