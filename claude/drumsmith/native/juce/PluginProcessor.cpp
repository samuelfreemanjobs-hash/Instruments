#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout parameters(){
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterChoice>("kind","Drum Type",
        juce::StringArray{"808","Kick","Snare","Clap","Hat","Cowbell","Perc","FX"},0));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("pitch","Pitch (Hz)",juce::NormalisableRange<float>(25,2500,1),52));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("decay","Decay",juce::NormalisableRange<float>(.01f,2.f,.001f),.4f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("drive","Drive",juce::NormalisableRange<float>(.1f,8.f,.01f),1.3f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("tone","Tone",juce::NormalisableRange<float>(0,1,.001f),.5f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("noise","Noise Color",juce::NormalisableRange<float>(0,1,.001f),.5f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("duration","Duration",juce::NormalisableRange<float>(.08f,3.f,.01f),1.f));
    return {p.begin(),p.end()};
}
DrumsmithProcessor::DrumsmithProcessor():AudioProcessor(BusesProperties().withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"DRUMSMITH",parameters()){}
void DrumsmithProcessor::prepareToPlay(double sr,int){engine.prepare(sr);}
bool DrumsmithProcessor::isBusesLayoutSupported(const BusesLayout& layouts)const{
    return layouts.getMainOutputChannelSet()==juce::AudioChannelSet::stereo() ||
           layouts.getMainOutputChannelSet()==juce::AudioChannelSet::mono();
}
void DrumsmithProcessor::updateRecipe() noexcept {
    drumsmith::Recipe r;
    r.kind=static_cast<drumsmith::Kind>(static_cast<int>(state.getRawParameterValue("kind")->load()));
    r.pitchHz=state.getRawParameterValue("pitch")->load();
    r.decaySeconds=state.getRawParameterValue("decay")->load();
    r.drive=state.getRawParameterValue("drive")->load();
    r.tone=state.getRawParameterValue("tone")->load();
    r.noiseColor=state.getRawParameterValue("noise")->load();
    r.durationSeconds=state.getRawParameterValue("duration")->load();
    engine.setRecipe(r);
}
void DrumsmithProcessor::processBlock(juce::AudioBuffer<float>& audio,juce::MidiBuffer& midi){
    juce::ScopedNoDenormals noDenormals;
    audio.clear();updateRecipe();
    // MIDI positions are sample accurate; fixed-size internal voices avoid audio callback allocation.
    int cursor=0;
    for(const auto metadata:midi){
        const int pos=juce::jlimit(cursor,audio.getNumSamples(),metadata.samplePosition);
        for(;cursor<pos;++cursor){const float v=engine.next();for(int c=0;c<audio.getNumChannels();++c)audio.setSample(c,cursor,v);}
        const auto m=metadata.getMessage();
        if(m.isNoteOn())engine.noteOn(m.getNoteNumber(),m.getFloatVelocity());
        else if(m.isNoteOff())engine.noteOff(m.getNoteNumber());
        else if(m.isAllNotesOff()||m.isAllSoundOff())engine.reset();
    }
    for(;cursor<audio.getNumSamples();++cursor){const float v=engine.next();for(int c=0;c<audio.getNumChannels();++c)audio.setSample(c,cursor,v);}
}
void DrumsmithProcessor::getStateInformation(juce::MemoryBlock& dest){
    auto xml=state.copyState().createXml();copyXmlToBinary(*xml,dest);
}
void DrumsmithProcessor::setStateInformation(const void* data,int size){
    if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(state.state.getType()))
        state.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* DrumsmithProcessor::createEditor(){return new DrumsmithEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new DrumsmithProcessor();}
