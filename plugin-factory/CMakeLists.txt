cmake_minimum_required(VERSION 3.22)

project(VstPluginFactoryOS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstrumentsPluginFactory.cmake")

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.15
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

pf_load_plugin_registry("${CMAKE_CURRENT_SOURCE_DIR}/plugins/registry.txt" PF_REGISTERED_PLUGINS)

message(STATUS "Plugin Factory OS — registered plugins: ${PF_REGISTERED_PLUGINS}")

foreach(pf_plugin_dir IN LISTS PF_REGISTERED_PLUGINS)
    set(pf_plugin_path "${CMAKE_CURRENT_SOURCE_DIR}/plugins/${pf_plugin_dir}")
    if(NOT IS_DIRECTORY "${pf_plugin_path}")
        message(FATAL_ERROR "Registry entry '${pf_plugin_dir}' is not a directory under plugins/")
    endif()
    add_subdirectory("plugins/${pf_plugin_dir}")
endforeach()
