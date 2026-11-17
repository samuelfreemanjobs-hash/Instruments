#include "DrumEngine.h"
#include "WavWriter.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
int main(int argc,char** argv){
    if(argc!=5 && argc!=11 && argc!=12){std::cerr<<"Usage: drumsmith-render <out.wav> <kind:0..7> <pitchHz> <seed> [decaySec drive tone noiseColor durationSec sampleRate [fxFamily]]\n";return 2;}
    try{
        const int k=std::stoi(argv[2]);if(k<0||k>7)throw std::invalid_argument("kind");
        drumsmith::Recipe r;r.kind=static_cast<drumsmith::Kind>(k);
        r.pitchHz=std::stof(argv[3]);r.seed=static_cast<std::uint32_t>(std::stoul(argv[4]));
        unsigned sr=44100;
        if(argc>=11){r.decaySeconds=std::stof(argv[5]);r.drive=std::stof(argv[6]);r.tone=std::stof(argv[7]);r.noiseColor=std::stof(argv[8]);r.durationSeconds=std::stof(argv[9]);sr=static_cast<unsigned>(std::stoul(argv[10]));}
        if(argc==12){r.fxFamily=std::stoi(argv[11]);}
        if(!drumsmith::valid(r)||sr<8000||sr>192000)throw std::invalid_argument("recipe out of range");
        drumsmith::Voice v;v.start(r,1.f,36);std::vector<float> data;
        data.reserve(static_cast<std::size_t>(sr*r.durationSeconds));
        while(v.active)data.push_back(v.process(sr));
        drumsmith::writeWav(argv[1],data,sr);
        std::cout<<"Rendered "<<data.size()<<" samples to "<<argv[1]<<"\n";
    }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}
}
