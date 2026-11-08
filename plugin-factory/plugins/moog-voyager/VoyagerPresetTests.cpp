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
    assert(voyager::getFactoryPresetName(24) == "025 P-Funk Parliament Stack");
    assert(voyager::getFactoryPresetName(29) == "030 DJ Screw Sub Slow");
    assert(voyager::getFactoryPresetName(30) == "031 Roger Troutman Talkbox Lead");
    assert(voyager::getFactoryPresetName(31) == "032 Roger Zapp Talkbox Bass");
    assert(voyager::getFactoryPresetName(kExpected - 1).size() > 4);
    assert(voyager::getFactoryPresetName(kExpected).empty());
    std::cout << "VoyagerPresetTests: OK (" << voyager::getFactoryPresetCount() << " Moog factory presets)\n";
    return 0;
}
