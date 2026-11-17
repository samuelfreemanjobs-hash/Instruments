#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>

namespace drumsmith {
enum class Kind : int { Bass808, Kick, Snare, Clap, Hat, Cowbell, Perc, Fx };
struct Recipe {
    Kind kind{Kind::Bass808};
    float pitchHz{52.0f};
    float decaySeconds{0.4f};
    float drive{1.3f};
    float tone{0.5f};
    float noiseColor{0.5f};
    float durationSeconds{1.0f};
    std::uint32_t seed{12345};
};
inline bool valid(const Recipe& r) noexcept {
    return static_cast<int>(r.kind)>=0 && static_cast<int>(r.kind)<=7 &&
       std::isfinite(r.pitchHz)&&r.pitchHz>=25.f&&r.pitchHz<=2500.f&&
       std::isfinite(r.decaySeconds)&&r.decaySeconds>=.01f&&r.decaySeconds<=2.f&&
       std::isfinite(r.drive)&&r.drive>=.1f&&r.drive<=8.f&&
       std::isfinite(r.tone)&&r.tone>=0.f&&r.tone<=1.f&&
       std::isfinite(r.noiseColor)&&r.noiseColor>=0.f&&r.noiseColor<=1.f&&
       std::isfinite(r.durationSeconds)&&r.durationSeconds>=.08f&&r.durationSeconds<=3.f;
}
struct Voice {
    Recipe recipe{};
    double phase{0.0};
    double phase2{0.0};
    double age{0.0};
    float noiseLow{0};
    std::uint32_t rng{1};
    int midiNote{-1};
    bool active{false};
    float velocity{1};
    void start(const Recipe& r, float vel, int note) noexcept {
        recipe=r;phase=0;phase2=0;age=0;noiseLow=0;rng=r.seed?r.seed:1;
        midiNote=note;velocity=std::clamp(vel,0.f,1.f);active=valid(r)&&velocity>0;
    }
    float noise() noexcept {
        rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;
        return static_cast<float>((rng & 0xffffffu)/8388607.5-1.0);
    }
    float process(double sr) noexcept {
        if(!active) return 0.f;
        const auto& r=recipe;
        const double t=age;
        const double env=std::exp(-t/r.decaySeconds);
        const double tau=2.0*std::numbers::pi;
        double x=0.0;
        if(r.kind==Kind::Bass808 || r.kind==Kind::Kick) {
            const double glide=(r.kind==Kind::Bass808?95.0:150.0)*std::exp(-t/(.012+r.tone*.013));
            phase+=tau*(r.pitchHz+glide)/sr;
            if(phase>tau) phase=std::fmod(phase,tau);
            const double body=std::sin(phase)+(.10+r.tone*.12)*std::sin(2*phase);
            x=std::tanh(r.drive*body)*(1-std::exp(-t/.0015))*env*.64;
        } else if(r.kind==Kind::Cowbell) {
            phase+=tau*r.pitchHz/sr;
            phase2+=tau*r.pitchHz*(1.48+r.tone*.05)/sr;
            if(phase>tau) phase-=tau;
            if(phase2>tau) phase2-=tau;
            x=(std::sin(phase)+(.30+.3*r.tone)*std::sin(phase2))*env*.42;
        } else {
            const float raw=noise();
            noiseLow=noiseLow*(.45f+.5f*r.tone)+raw*(.55f-.5f*r.tone);
            const double hp=raw-noiseLow;
            double burst=1.0;
            if(r.kind==Kind::Clap) {
                burst=0.0;
                for(const double offset:{0.0,.011,.024,.038})
                    if(t>=offset) burst+=std::exp(-(t-offset)/(.006+.014*r.tone));
            } else if(r.kind==Kind::Hat) burst=std::exp(-t/(.025+.055*r.tone))/std::max(env,1e-8);
            else if(r.kind==Kind::Snare) burst=1+.35*std::sin(tau*150*t);
            else if(r.kind==Kind::Perc) burst=std::sin(tau*(230+r.tone*330)*t)*1.1;
            else if(r.kind==Kind::Fx) burst=std::sin(tau*(15*t+45*t*t));
            x=std::tanh(r.drive*(hp*r.noiseColor+.22*std::sin(tau*(120+r.tone*150)*t))*burst)*env*.49;
        }
        age+=1.0/sr;
        if(age>=r.durationSeconds || env<.000001) active=false;
        // Offline and realtime both apply a bounded sample clip, not a true-peak limiter.
        return std::clamp(static_cast<float>(x)*velocity,-.97f,.97f);
    }
};
class Engine {
public:
    static constexpr int maxVoices=32;
    bool prepare(double sr) noexcept {
        if(!std::isfinite(sr)||sr<8000||sr>192000) return false;
        sampleRate=sr;reset();return true;
    }
    void reset() noexcept { for(auto& voice:voices) voice.active=false; nextVoice=0; }
    void setRecipe(const Recipe& r) noexcept { if(valid(r)) recipe=r; }
    const Recipe& getRecipe() const noexcept {return recipe;}
    void noteOn(int note,float velocity) noexcept {
        if(note<0||note>127||velocity<=0) return;
        Voice* candidate=nullptr;
        for(auto& voice:voices) if(!voice.active) {candidate=&voice;break;}
        if(!candidate) candidate=&voices[nextVoice++%maxVoices];
        Recipe r=recipe;
        // Pitch follows MIDI note for pitched 808s, preserving the base recipe note at MIDI 36.
        if(r.kind==Kind::Bass808 || r.kind==Kind::Cowbell)
            r.pitchHz=std::clamp(r.pitchHz*std::pow(2.f,(note-36)/12.f),25.f,2500.f);
        candidate->start(r,velocity,note);
    }
    void noteOff(int note) noexcept { (void)note; /* one-shot tails do not cut on note-off */ }
    float next() noexcept {
        double sum=0;
        for(auto& voice:voices) if(voice.active) sum+=voice.process(sampleRate);
        return std::tanh(static_cast<float>(sum)*1.0f)*.92f;
    }
    void process(float* left,float* right,int count) noexcept {
        if(!left||count<=0) return;
        for(int i=0;i<count;++i) {const float v=next();left[i]=v;if(right)right[i]=v;}
    }
    double getSampleRate()const noexcept {return sampleRate;}
private:
    double sampleRate{44100}; Recipe recipe{};
    std::array<Voice,maxVoices> voices{};
    std::uint32_t nextVoice{0};
};
} // namespace drumsmith
