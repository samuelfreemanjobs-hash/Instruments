#include "RenaissanceSurfaceMap.h"

#include <iostream>

int main()
{
    renaissance::RenaissanceSurfaceMap map;
    map.setMidiOmni (true);
    if (map.padIndexForNote (36) != 0 || map.padIndexForNote (51) != 15)
    {
        std::cerr << "pad note map failed\n";
        return 1;
    }
    if (map.qlinkCc (0) != 1 || map.qlinkCc (15) != 16)
    {
        std::cerr << "qlink cc map failed\n";
        return 1;
    }
    if (! map.acceptsChannel (10))
    {
        std::cerr << "omni channel accept failed\n";
        return 1;
    }
    std::cout << "RenaissanceHostSurfaceMapTests OK\n";
    return 0;
}
