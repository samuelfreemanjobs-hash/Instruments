# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspace/MyFirstPlugin/build/_deps/juce-src"
  "/workspace/MyFirstPlugin/build/_deps/juce-build"
  "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix"
  "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix/tmp"
  "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp"
  "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix/src"
  "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/MyFirstPlugin/build/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
