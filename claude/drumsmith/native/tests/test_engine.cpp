#include "DrumEngine.h"
#include "WavWriter.h"
#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <vector>
int main(){
    using namespace drumsmith;
    Engine e;assert(!e.prepare(0));assert(e.prepare(48000));
    Recipe r;assert(valid(r));r.drive=100;assert(!valid(r));r.drive=1.3;
    for(int k=0;k<8;++k){
        r.kind=static_cast<Kind>(k);r.seed=1234;
        Voice a,b;a.start(r,1.f,36);b.start(r,1.f,36);
        double energy=0;
        for(int i=0;i<4800;++i){const float x=a.process(48000);const float y=b.process(48000);
            assert(std::isfinite(x)&&std::abs(x)<=.971f);assert(x==y);energy+=x*x;}
        assert(energy>0.01);
    }
    e.setRecipe(r);e.noteOn(36,1);std::vector<float> l(4096),rr(4096);e.process(l.data(),rr.data(),4096);
    for(int i=0;i<4096;++i)assert(l[i]==rr[i]&&std::isfinite(l[i]));
    const auto tmp=std::filesystem::temp_directory_path()/"drumsmith_v2_native_test.wav";
    std::filesystem::remove(tmp);writeWav(tmp,l,48000);assert(std::filesystem::file_size(tmp)==44+8192);
    bool rejected=false;try{writeWav(tmp,l,48000);}catch(const std::runtime_error&){rejected=true;}
    assert(rejected);std::filesystem::remove(tmp);
    std::cout<<"Native DSP: 8 kinds, deterministic renders, stereo, bounds, WAV safety PASS\n";
}
