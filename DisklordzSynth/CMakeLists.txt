# DisklordzSynth — offline procedural engines (retain as separate commercial tools).

add_library (DisklordzSynth STATIC
    src/EngineParams.cpp
    src/ProceduralSynth.cpp
    src/FactoryPackBuilder.cpp
    src/RawRomBuilder.cpp
    src/WavWriter.cpp
    src/WavReader.cpp
    src/MultisampleManifest.cpp
    src/MultisampleRomBuilder.cpp
)

target_include_directories (DisklordzSynth
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features (DisklordzSynth PUBLIC cxx_std_20)

add_executable (DisklordzSynth_BuildFactoryPack apps/BuildFactoryPackMain.cpp)
target_link_libraries (DisklordzSynth_BuildFactoryPack PRIVATE DisklordzSynth)

add_executable (DisklordzSynth_BuildRawRom apps/BuildRawRomMain.cpp)
target_link_libraries (DisklordzSynth_BuildRawRom PRIVATE DisklordzSynth)

add_executable (DisklordzSynth_BundleRawRoms apps/BundleRawRomsMain.cpp)
target_link_libraries (DisklordzSynth_BundleRawRoms PRIVATE DisklordzSynth)

add_executable (DisklordzSynth_RomFactory apps/RomFactoryMain.cpp)
target_link_libraries (DisklordzSynth_RomFactory PRIVATE DisklordzSynth)

add_executable (DisklordzSynth_RenderWav apps/RenderWavMain.cpp)
target_link_libraries (DisklordzSynth_RenderWav PRIVATE DisklordzSynth)

add_executable (DisklordzSynth_ExportWavPack apps/ExportWavPackMain.cpp)
target_link_libraries (DisklordzSynth_ExportWavPack PRIVATE DisklordzSynth)

add_executable (DisklordzSynthTests tests/SynthSmokeTests.cpp)
target_link_libraries (DisklordzSynthTests PRIVATE DisklordzSynth)
target_compile_features (DisklordzSynthTests PRIVATE cxx_std_20)

add_test (NAME DisklordzSynthSmoke COMMAND DisklordzSynthTests)
