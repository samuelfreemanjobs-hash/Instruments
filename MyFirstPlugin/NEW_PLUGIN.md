# Start a new plugin from this template

> **New work:** prefer the monorepo [**VST Plugin Factory OS**](../plugin-factory/README.md) (`plugin-factory/scripts/factory.sh`) for registry-based multi-plugin builds and scaffolding.

This folder builds **two** plugins from one CMake project:

| Target | Type | Starting files |
|--------|------|----------------|
| `MyFirstPlugin` | VST3 effect | `PluginProcessor.*`, `PluginEditor.*` |
| `MyFirstSynth` | VSTi | `SynthProcessor.*`, `SynthEditor.*` |

Duplicate the pair that matches your goal, or remove the other `juce_add_plugin` block.

1. **Rename in `CMakeLists.txt`**
   - `project(...)` name
   - `juce_add_plugin(...)` first argument (CMake target)
   - `PLUGIN_MANUFACTURER_CODE` / `PLUGIN_CODE` (four characters each; plugin code needs one uppercase letter)
   - `PRODUCT_NAME` (DAW display name)

2. **Rename C++ class** (optional but clearer)
   - `MyFirstPluginAudioProcessor` → `YourNameAudioProcessor`
   - `MyFirstPluginAudioProcessorEditor` → `YourNameAudioProcessorEditor`
   - Update `createPluginFilter()` at the bottom of `PluginProcessor.cpp`

3. **Parameters**
   - Add IDs in `ParameterIds.h`
   - Register in `createParameterLayout()`
   - Cache `getRawParameterValue` in the processor constructor
   - Add smoothing in `prepareToPlay` / `processBlock`
   - Wire sliders in `PluginEditor.cpp`

4. **Build**
   - Windows: `Build Plugin.bat` or Visual Studio (see `VISUAL_STUDIO.md`)
   - Enable auto-install to the system VST3 folder (optional):
     ```powershell
     cmake --preset windows-vs-release -DMYFIRSTPLUGIN_COPY_AFTER_BUILD=ON
     ```

5. **AI prompt**
   - Copy `CURSOR_COMPOSER_PROMPT.md` into Composer with your metadata filled in.
