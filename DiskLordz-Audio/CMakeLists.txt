cmake_minimum_required(VERSION 3.22)

project(DiskLordzAudio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../JUCE_VERSION")
  file(READ "${CMAKE_CURRENT_LIST_DIR}/../JUCE_VERSION" JUCE_PINNED_VERSION)
  string(STRIP "${JUCE_PINNED_VERSION}" JUCE_PINNED_VERSION)
else()
  set(JUCE_PINNED_VERSION "8.0.15")
endif()

include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE
  GIT_TAG ${JUCE_PINNED_VERSION}
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

add_subdirectory(Products/ChordEngine)
