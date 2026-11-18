#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
namespace drumsmith {
inline void writeU16(std::ostream& os,std::uint16_t n){for(int i=0;i<2;++i)os.put(static_cast<char>((n>>(8*i))&255));}
inline void writeU32(std::ostream& os,std::uint32_t n){for(int i=0;i<4;++i)os.put(static_cast<char>((n>>(8*i))&255));}
inline void writeWav(const std::filesystem::path& path,const std::vector<float>& samples,unsigned rate){
    if(rate<8000||rate>192000||samples.size()>(UINT32_MAX-44u)/2u)throw std::invalid_argument("wav size/rate");
    if(std::filesystem::exists(path)) throw std::runtime_error("Refusing to overwrite existing WAV");
    std::ofstream f(path,std::ios::binary); if(!f)throw std::runtime_error("Cannot open WAV output");
    const auto bytes=static_cast<std::uint32_t>(samples.size()*2u);
    f.write("RIFF",4);writeU32(f,36u+bytes);f.write("WAVEfmt ",8);
    writeU32(f,16);writeU16(f,1);writeU16(f,1);writeU32(f,rate);
    writeU32(f,rate*2);writeU16(f,2);writeU16(f,16);f.write("data",4);writeU32(f,bytes);
    for(float v:samples){if(!std::isfinite(v))throw std::runtime_error("Nonfinite audio");
        const auto q=static_cast<std::int16_t>(std::lrint(std::clamp(v,-1.f,1.f)*32767.f));writeU16(f,static_cast<std::uint16_t>(q));}
    if(!f)throw std::runtime_error("Failed writing WAV");
}
}
