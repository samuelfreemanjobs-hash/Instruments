#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../Presets/GenreBankLibrary.h"
#include <cstring>

namespace
{
template <typename T>
T* raw(juce::AudioProcessorValueTreeState& apvts, const char* id)
{
    return dynamic_cast<T*>(apvts.getParameter(id));
}

void fireTrack(disklordz::MultiGenreDrumEngine& eng, const disklordz::bang::GrooveTrack& track, int step)
{
    if (step < 0 || step >= 64)
        return;
    const auto& hit = track.steps[static_cast<size_t>(step)];
    if (! hit.active)
        return;
    eng.handleMidiNote(36 + static_cast<int>(hit.pad), juce::jlimit(0.05f, 1.f, hit.velocity));
}
} // namespace

DrumDesignLabAudioProcessor::DrumDesignLabAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "DDL", createParameterLayout())
{
    engine_ = std::make_unique<disklordz::MultiGenreDrumEngine>();
    groove_ = disklordz::bang::makeOddity(disklordz::bang::GrooveFamily::MilliPressure, 0xD15CULL);
}

juce::AudioProcessorValueTreeState::ParameterLayout DrumDesignLabAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::StringArray styleNames;
    for (int i = 0; i < static_cast<int>(disklordz::KitStyle::Count); ++i)
        styleNames.add(disklordz::kitStyleName(static_cast<disklordz::KitStyle>(i)));

    juce::StringArray formulaNames;
    for (int i = 0; i < disklordz::genre_bank::kFormulaCount; ++i)
        formulaNames.add(disklordz::genre_bank::formulaAt(i).displayName);

    juce::StringArray modes { "SYNTH", "DESIGN", "DNA", "GROOVE", "FACTORY" };
    juce::StringArray destruction { "Clean", "Mixed", "Destroyed" };
    juce::StringArray dnaMatrix { "HyperDNA", "PercussionDNA", "ZayDNA", "SouthDNA" };
    juce::StringArray grooves;
    for (int i = 0; i < static_cast<int>(disklordz::bang::GrooveFamily::Count); ++i)
        grooves.add("Groove " + juce::String(i + 1));
    // Prefer readable names for known families
    grooves.clear();
    grooves.addArray({ "Milli Pressure", "Dirty Digital", "Boss Bounce", "Southern Oddity",
                       "Raw Machine", "Minimal Monster", "Eccentric Atlanta", "Mechanical Bounce",
                       "Digital Funk", "Experimental Street", "Sparse Knock", "Rhythmic Hypnosis" });

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_mode", 1 }, "Mode", modes, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_kit_style", 1 }, "Kit Style", styleNames, 1));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_dna_formula", 1 }, "DNA Formula", formulaNames, 8));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_output_gain", 1 }, "Output",
        juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 0.95f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_master_gain_db", 1 }, "Master Output",
        juce::NormalisableRange<float>(-24.f, 6.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "ddl_bypass", 1 }, "Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ddl_selected_pad", 1 }, "Selected Pad", 1, 16, 1));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_destruction_mode", 1 }, "Destruction Tier", destruction, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_sub_freq_hz", 1 }, "Sub Frequency",
        juce::NormalisableRange<float>(35.f, 75.f, 0.1f), 49.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_punch_freq_hz", 1 }, "Initial Punch",
        juce::NormalisableRange<float>(80.f, 250.f, 0.1f), 165.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_pitch_sweep_ms", 1 }, "Pitch Sweep",
        juce::NormalisableRange<float>(5.f, 100.f, 0.1f), 24.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_amp_decay_ms", 1 }, "Amp Decay",
        juce::NormalisableRange<float>(20.f, 2500.f, 1.f, 0.4f), 340.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_body_level", 1 }, "Body Layer",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 85.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_attack_level", 1 }, "Attack Layer",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 70.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_texture_level", 1 }, "Texture Layer",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 50.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_drive", 1 }, "Drive",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 35.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_dirt_blend", 1 }, "Dirt Blend",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 55.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_sampler_grit", 1 }, "12-Bit Sampler Grit",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_stereo_width", 1 }, "Stereo Width",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 30.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_reverb_mix", 1 }, "Space Mix",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_filter_cutoff_hz", 1 }, "Filter Cutoff",
        juce::NormalisableRange<float>(40.f, 18000.f, 1.f, 0.3f), 9500.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_fm_ratio", 1 }, "FM Ratio",
        juce::NormalisableRange<float>(0.5f, 8.f, 0.01f), 2.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_fm_index", 1 }, "FM Index",
        juce::NormalisableRange<float>(0.f, 6.f, 0.01f), 2.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_pad_gain", 1 }, "Pad Gain",
        juce::NormalisableRange<float>(0.1f, 2.f, 0.01f), 1.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_dna_x", 1 }, "Hyper DNA X",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 50.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_dna_y", 1 }, "Hyper DNA Y",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 50.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_dna_influence", 1 }, "DNA Influence",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_dna_matrix", 1 }, "DNA Matrix", dnaMatrix, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_mutate_strength", 1 }, "Mutate Strength",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 35.f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ddl_seed", 1 }, "Recipe Seed", 1, 999999, 313808));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "ddl_seq_enabled", 1 }, "Sequencer Play", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_groove_family", 1 }, "Groove Family", grooves, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_bpm", 1 }, "Internal BPM",
        juce::NormalisableRange<float>(70.f, 200.f, 0.1f), 142.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_swing", 1 }, "Bounce Swing",
        juce::NormalisableRange<float>(0.f, 65.f, 0.1f), 16.f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ddl_factory_variations", 1 }, "Factory Variations", 1, 8, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_export_ceiling_db", 1 }, "Export Ceiling",
        juce::NormalisableRange<float>(-12.f, -0.1f, 0.1f), -1.f));

    return { params.begin(), params.end() };
}

disklordz::sound_edit::Values DrumDesignLabAudioProcessor::readEditValuesFromApvts() const
{
    auto load = [this](const char* id) { return apvts.getRawParameterValue(id)->load(); };
    disklordz::sound_edit::Values v;
    v.subHz = load("ddl_sub_freq_hz");
    v.punchHz = load("ddl_punch_freq_hz");
    v.sweepMs = load("ddl_pitch_sweep_ms");
    v.decayMs = load("ddl_amp_decay_ms");
    v.bodyLevel = load("ddl_body_level") * 0.01f;
    v.attackLevel = load("ddl_attack_level") * 0.01f;
    v.textureLevel = load("ddl_texture_level") * 0.01f;
    v.drive01 = load("ddl_drive") * 0.01f;
    v.dirt01 = load("ddl_dirt_blend") * 0.01f;
    v.grit01 = load("ddl_sampler_grit") * 0.01f;
    v.width01 = load("ddl_stereo_width") * 0.01f;
    v.reverb01 = load("ddl_reverb_mix") * 0.01f;
    v.cutoffHz = load("ddl_filter_cutoff_hz");
    v.fmRatio = load("ddl_fm_ratio");
    v.fmIndex = load("ddl_fm_index");
    v.padGain = load("ddl_pad_gain");
    return v;
}

void DrumDesignLabAudioProcessor::writeEditValuesToApvts(const disklordz::sound_edit::Values& v)
{
    suppressEditPush_ = true;
    auto setFloat = [this](const char* id, float value) {
        if (auto* p = apvts.getParameter(id))
            p->setValueNotifyingHost(p->convertTo0to1(value));
    };
    setFloat("ddl_sub_freq_hz", v.subHz);
    setFloat("ddl_punch_freq_hz", v.punchHz);
    setFloat("ddl_pitch_sweep_ms", v.sweepMs);
    setFloat("ddl_amp_decay_ms", v.decayMs);
    setFloat("ddl_body_level", v.bodyLevel * 100.f);
    setFloat("ddl_attack_level", v.attackLevel * 100.f);
    setFloat("ddl_texture_level", v.textureLevel * 100.f);
    setFloat("ddl_drive", v.drive01 * 100.f);
    setFloat("ddl_dirt_blend", v.dirt01 * 100.f);
    setFloat("ddl_sampler_grit", v.grit01 * 100.f);
    setFloat("ddl_stereo_width", v.width01 * 100.f);
    setFloat("ddl_reverb_mix", v.reverb01 * 100.f);
    setFloat("ddl_filter_cutoff_hz", v.cutoffHz);
    setFloat("ddl_fm_ratio", v.fmRatio);
    setFloat("ddl_fm_index", v.fmIndex);
    setFloat("ddl_pad_gain", v.padGain);
    suppressEditPush_ = false;
}

void DrumDesignLabAudioProcessor::applySoundEditsFromApvts()
{
    if (suppressEditPush_ || engine_ == nullptr)
        return;
    const auto v = readEditValuesFromApvts();
    if (hasLastApplied_ && std::memcmp(&v, &lastApplied_, sizeof(v)) == 0)
        return;
    lastApplied_ = v;
    hasLastApplied_ = true;
    engine_->applySoundEdit(selectedPad_, v);

    if (auto* dest = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_destruction_mode")))
        engine_->setDestruction(selectedPad_, static_cast<disklordz::DestructionTier>(dest->getIndex()));
}

void DrumDesignLabAudioProcessor::applyHyperDnaFromApvts()
{
    if (engine_ == nullptr || ! engine_->usesThreeLayerPads())
        return;
    const float x = apvts.getRawParameterValue("ddl_dna_x")->load() * 0.01f;
    const float y = apvts.getRawParameterValue("ddl_dna_y")->load() * 0.01f;
    const float infl = apvts.getRawParameterValue("ddl_dna_influence")->load() * 0.01f;
    int matrix = 0;
    if (auto* m = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_dna_matrix")))
        matrix = m->getIndex();
    engine_->applyHyperDna(selectedPad_, x, y, infl,
                           static_cast<disklordz::dna::MatrixMode>(matrix));
    hasLastApplied_ = false;
    knobsNeedPull_.store(true, std::memory_order_release);
}

void DrumDesignLabAudioProcessor::pullKnobsFromEngine()
{
    if (engine_ == nullptr)
        return;
    writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
    hasLastApplied_ = false;
}

void DrumDesignLabAudioProcessor::selectPad(int padIndex, bool preview)
{
    selectedPad_ = juce::jlimit(0, 15, padIndex);
    if (auto* p = raw<juce::AudioParameterInt>(apvts, "ddl_selected_pad"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(selectedPad_ + 1)));
    hasLastApplied_ = false;
    if (engine_ != nullptr)
    {
        engine_->captureDnaBase(selectedPad_);
        writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
        if (preview)
            engine_->triggerUiPad(selectedPad_, 0.9f);
    }
}

void DrumDesignLabAudioProcessor::loadDnaFormula(int formulaIndex)
{
    dnaFormula_ = juce::jlimit(0, disklordz::genre_bank::kFormulaCount - 1, formulaIndex);
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_dna_formula")))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(dnaFormula_)));
    if (engine_ != nullptr)
    {
        // DNA formula is independent of Kit Style — activates 16-pad bank mode
        engine_->loadDnaFormula(dnaFormula_);
        hasLastApplied_ = false;
        writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
        knobsNeedPull_.store(true, std::memory_order_release);
    }
}

void DrumDesignLabAudioProcessor::mutateSelectedPad()
{
    if (engine_ == nullptr)
        return;
    const float strength = apvts.getRawParameterValue("ddl_mutate_strength")->load() * 0.01f;
    const int seed = static_cast<int>(apvts.getRawParameterValue("ddl_seed")->load());
    engine_->mutatePad(selectedPad_, strength, static_cast<uint64_t>(seed));
    writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
    hasLastApplied_ = false;
    queueUiNote(36 + selectedPad_, 0.9f);
}

juce::String DrumDesignLabAudioProcessor::exportKitPack(const juce::File& folder, bool allFormulas, int variations)
{
    if (! folder.isDirectory() && ! folder.createDirectory())
    {
        lastExportStatus_ = "Export failed: could not create folder";
        return lastExportStatus_;
    }
    disklordz::kit_factory::ExportOptions opt;
    opt.sampleRate = sampleRate_ > 0.0 ? sampleRate_ : 48000.0;
    opt.maxFrames = static_cast<int>(opt.sampleRate);
    opt.variations = juce::jlimit(1, 8, variations);
    opt.familyFolders = true;
    const float ceilingDb = apvts.getRawParameterValue("ddl_export_ceiling_db")->load();
    opt.peakCeiling = juce::Decibels::decibelsToGain(ceilingDb);

    disklordz::kit_factory::ExportResult r;
    if (allFormulas)
        r = disklordz::kit_factory::exportAllDnaFormulas(folder.getFullPathName().toStdString(), opt);
    else if (engine_ != nullptr && engine_->usesThreeLayerPads())
        r = disklordz::kit_factory::exportKitProgram(
            engine_->kit(), folder.getFullPathName().toStdString(),
            disklordz::genre_bank::formulaAt(dnaFormula_).id,
            disklordz::genre_bank::formulaAt(dnaFormula_).displayName, opt);
    else
        r = disklordz::kit_factory::exportDnaFormula(dnaFormula_, folder.getFullPathName().toStdString(), opt);

    lastExportStatus_ = "Exported " + juce::String(r.written) + " WAVs (failed "
                        + juce::String(r.failed) + ", qa-skip " + juce::String(r.skippedQa) + ") -> "
                        + folder.getFullPathName();
    return lastExportStatus_;
}

DrumDesignLabAudioProcessor::WorkspaceMode DrumDesignLabAudioProcessor::getWorkspaceMode() const
{
    if (auto* m = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_mode")))
        return static_cast<WorkspaceMode>(juce::jlimit(0, static_cast<int>(WorkspaceMode::Count) - 1, m->getIndex()));
    return WorkspaceMode::Synth;
}

juce::String DrumDesignLabAudioProcessor::getStatusLine() const
{
    juce::String s = juce::String(disklordz::kitStyleName(currentStyle_));
    s += " | Pad " + juce::String(selectedPad_ + 1);
    s += " | Formula " + juce::String(dnaFormula_);
    if (apvts.getRawParameterValue("ddl_seq_enabled")->load() > 0.5f)
        s += " | Groove step " + juce::String(grooveStep_ + 1);
    if (lastExportStatus_.isNotEmpty())
        s += " | " + lastExportStatus_;
    return s;
}

void DrumDesignLabAudioProcessor::queueUiNote(int midiNote, float velocity01) noexcept
{
    uiVel_.store(velocity01, std::memory_order_relaxed);
    uiNote_.store(midiNote, std::memory_order_release);
}

void DrumDesignLabAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    sampleRate_ = sampleRate;
    if (engine_ == nullptr)
        engine_ = std::make_unique<disklordz::MultiGenreDrumEngine>();
    engine_->prepare(sampleRate);
    if (auto* f = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_dna_formula")))
        dnaFormula_ = f->getIndex();
    engine_->loadDnaFormula(dnaFormula_);
    writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
    const float bpm = apvts.getRawParameterValue("ddl_bpm")->load();
    samplesPerStep_ = (60.0 / static_cast<double>(juce::jmax(40.f, bpm))) * sampleRate_ * 0.25;
}

bool DrumDesignLabAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

void DrumDesignLabAudioProcessor::setKitStyle(disklordz::KitStyle style)
{
    currentStyle_ = style;
    if (engine_ != nullptr)
        engine_->setStyle(style); // clears DNA bank mode
    if (auto* p = apvts.getParameter("ddl_kit_style"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(static_cast<int>(style))));
    hasLastApplied_ = false;
    if (engine_ != nullptr)
        writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
    knobsNeedPull_.store(true, std::memory_order_release);
}

void DrumDesignLabAudioProcessor::setCurrentProgram(int index)
{
    const int n = static_cast<int>(disklordz::KitStyle::Count);
    setKitStyle(static_cast<disklordz::KitStyle>(juce::jlimit(0, n - 1, index)));
}

const juce::String DrumDesignLabAudioProcessor::getProgramName(int index)
{
    const int n = static_cast<int>(disklordz::KitStyle::Count);
    if (index < 0 || index >= n)
        return {};
    return disklordz::kitStyleName(static_cast<disklordz::KitStyle>(index));
}

void DrumDesignLabAudioProcessor::syncFromApvts()
{
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_kit_style")))
    {
        const auto style = static_cast<disklordz::KitStyle>(choice->getIndex());
        if (style != currentStyle_)
        {
            currentStyle_ = style;
            if (engine_ != nullptr)
                engine_->setStyle(style);
            hasLastApplied_ = false;
            knobsNeedPull_.store(true, std::memory_order_release);
        }
    }
    if (auto* f = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_dna_formula")))
    {
        if (f->getIndex() != dnaFormula_)
        {
            dnaFormula_ = f->getIndex();
            if (engine_ != nullptr)
            {
                engine_->loadDnaFormula(dnaFormula_);
                currentStyle_ = engine_->style();
            }
            hasLastApplied_ = false;
            knobsNeedPull_.store(true, std::memory_order_release);
        }
    }
    if (auto* padParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("ddl_selected_pad")))
    {
        const int pad = juce::jlimit(0, 15, padParam->get() - 1);
        if (pad != selectedPad_)
        {
            selectedPad_ = pad;
            hasLastApplied_ = false;
            knobsNeedPull_.store(true, std::memory_order_release);
        }
    }
    if (auto* gf = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_groove_family")))
    {
        const auto fam = static_cast<disklordz::bang::GrooveFamily>(gf->getIndex());
        if (groove_.family != fam)
            groove_ = disklordz::bang::makeOddity(fam, 0xD15CULL + static_cast<uint64_t>(gf->getIndex()));
    }
    const float bpm = apvts.getRawParameterValue("ddl_bpm")->load();
    samplesPerStep_ = (60.0 / static_cast<double>(juce::jmax(40.f, bpm))) * sampleRate_ * 0.25;
}

void DrumDesignLabAudioProcessor::tickGroove(int numSamples)
{
    const bool on = apvts.getRawParameterValue("ddl_seq_enabled")->load() > 0.5f;
    if (! on)
    {
        grooveWasOn_ = false;
        return;
    }
    if (! grooveWasOn_)
    {
        groovePhase_ = 0.0;
        grooveStep_ = 0;
        grooveWasOn_ = true;
    }
    if (engine_ == nullptr || samplesPerStep_ < 1.0)
        return;

    const float swing = apvts.getRawParameterValue("ddl_swing")->load() * 0.01f;
    groove_.kick.swing01 = swing;
    groove_.hat.swing01 = swing;

    for (int i = 0; i < numSamples; ++i)
    {
        groovePhase_ += 1.0;
        if (groovePhase_ >= samplesPerStep_)
        {
            groovePhase_ -= samplesPerStep_;
            fireTrack(*engine_, groove_.kick, grooveStep_);
            fireTrack(*engine_, groove_.snare, grooveStep_);
            fireTrack(*engine_, groove_.hat, grooveStep_);
            fireTrack(*engine_, groove_.perc, grooveStep_);
            grooveStep_ = (grooveStep_ + 1) % juce::jmax(1, groove_.steps);
        }
    }
}

void DrumDesignLabAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    if (engine_ == nullptr)
        return;

    syncFromApvts();
    if (! suppressEditPush_)
        applySoundEditsFromApvts();

    const bool bypass = apvts.getRawParameterValue("ddl_bypass")->load() > 0.5f;
    const float outGain = apvts.getRawParameterValue("ddl_output_gain")->load()
                          * juce::Decibels::decibelsToGain(apvts.getRawParameterValue("ddl_master_gain_db")->load());
    const int n = buffer.getNumSamples();
    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    const int pending = uiNote_.exchange(-1, std::memory_order_acq_rel);
    if (pending >= 0)
        engine_->handleMidiNote(pending, uiVel_.load(std::memory_order_relaxed));

    tickGroove(n);

    auto it = midi.findNextSamplePosition(0);
    for (int i = 0; i < n; ++i)
    {
        while (it != midi.cend() && (*it).samplePosition <= i)
        {
            const auto msg = (*it).getMessage();
            if (msg.isNoteOn())
                engine_->handleMidiNote(msg.getNoteNumber(), msg.getFloatVelocity());
            ++it;
        }
        float l = 0.f, r = 0.f;
        if (! bypass)
            engine_->renderTick(l, r);
        L[i] = l * outGain;
        if (R != nullptr)
            R[i] = r * outGain;
    }
}

void DrumDesignLabAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void DrumDesignLabAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* DrumDesignLabAudioProcessor::createEditor()
{
    return new DrumDesignLabAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumDesignLabAudioProcessor();
}
