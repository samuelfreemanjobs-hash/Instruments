#include "VoyagerPresetBank.h"
#include <cassert>
#include <iostream>

int main()
{
    assert(voyager::getFactoryPresetCount() == 24);
    assert(voyager::getFactoryPresetName(0) == "01 Voyager Init");
    assert(voyager::getFactoryPresetName(23) == "24 Full Stack Unison");
    assert(voyager::getFactoryPresetName(99).empty());
    std::cout << "VoyagerPresetTests: OK (" << voyager::getFactoryPresetCount() << " factory presets)\n";
    return 0;
}
