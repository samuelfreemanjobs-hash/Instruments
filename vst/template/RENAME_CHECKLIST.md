# After copying `vst/template` → `vst/<YourPlugin>`

Replace these identifiers (search whole plugin folder):

| Item | Template value | Your plugin |
|------|----------------|-------------|
| CMake `project()` | `TemplateSynth` | `YourPlugin` |
| `juce_add_plugin()` name | `TemplateSynth` | `YourPlugin` |
| `PRODUCT_NAME` | `Template Synth` | Display name |
| `PLUGIN_MANUFACTURER_CODE` | `Inst` | 4 chars, unique |
| `PLUGIN_CODE` | `Tmpl` | 4 chars, unique |
| C++ class | `TemplateSynthAudioProcessor` | `YourPluginAudioProcessor` |
| Editor class | `TemplateSynthAudioProcessorEditor` | `YourPluginAudioProcessorEditor` |
| `BUNDLE_ID` | `com.instruments.templatesynth` | reverse-DNS unique |

Files to edit: `CMakeLists.txt`, `Source/PluginProcessor.h`, `Source/PluginProcessor.cpp`, `Source/PluginEditor.h`, `Source/PluginEditor.cpp`.
