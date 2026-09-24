#include "VoyagerPresetBank.h"
#include <cassert>
#include <iostream>

int main()
{
    constexpr int kExpected = 300;
    assert(voyager::getFactoryPresetCount() == kExpected);
    assert(voyager::getFactoryPresetName(0) == "001 Voyager Init");
    assert(voyager::getFactoryPresetName(2) == "003 G-Funk Lead");
    assert(voyager::getFactoryPresetName(6) == "007 G-Funk Bass");
    assert(voyager::getFactoryPresetName(kExpected - 1).size() > 4);
    assert(voyager::getFactoryPresetName(kExpected).empty());
    std::cout << "VoyagerPresetTests: OK (" << voyager::getFactoryPresetCount() << " Moog factory presets)\n";
    return 0;
}
