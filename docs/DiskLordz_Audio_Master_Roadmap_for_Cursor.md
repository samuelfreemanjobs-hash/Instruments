# DISKLORDZ AUDIO

## Business & Technical Master Roadmap

**Version:** 1.0\
**Date:** November 7, 2026\
**Purpose:** Master planning document and implementation brief for
Cursor and AI coding agents.

> **Repo integration (Instruments monorepo)**  
> | Role | Owner | Read first |
> |------|--------|------------|
> | Business execution, weekly review, sprint checklist | **`audio-business-copilot`** | [DISKLORDZ_MASTER_PLAN.md](DISKLORDZ_MASTER_PLAN.md) · [audio-command PWA](../disklordz/audio-command/pwa/index.html) |
> | VST / JUCE / DPF implementation | **Cursor Cloud** (this document §18–20) | [DPF_TECHNICAL_ARCHITECTURE.md](DPF_TECHNICAL_ARCHITECTURE.md) · [CHORD_ENGINE_PRD.md](CHORD_ENGINE_PRD.md) |
> | Plugin CI after C++ changes | **`golden-wav-qa`** | `python3 vst-testing-ops/run_business.py --profile ci` |
>
> Cursor must **not** own store pricing, marketing strategy, or sprint PM — hand that to **audio-business-copilot**. Cursor **does** own Milestone 001 → CHORD ENGINE engineering per §18.2 below.

------------------------------------------------------------------------

# 1. Executive Summary

DiskLordz Audio is an independent audio software company specializing in
MIDI creativity, sound design, and genre-specific audio processing for
trap, phonk, and underground hip-hop producers.

The company will build a family of affordable, professional-quality
audio plugins on a reusable shared framework. The first product will be
**CHORD ENGINE**, a genre-aware MIDI chord and progression assistant.

The business will launch Windows-first, with macOS support planned after
the initial product architecture and commercial model are proven.
Development should remain as close to a \$0 cash budget as practical,
using free and open-source tools where commercially appropriate.

The goal is not to create disconnected plugin prototypes. The goal is to
build a real, maintainable commercial audio-software product line with
shared technology, consistent design, independent products,
customer-friendly licensing, and direct online sales.

## 1.1 Company decisions

  -----------------------------------------------------------------------
  Decision                            Direction
  ----------------------------------- -----------------------------------
  Company                             DiskLordz Audio

  Target market                       Trap, phonk, underground hip-hop

  Initial platform                    Windows 10/11, 64-bit

  Future platform                     macOS

  Development model                   Shared C++ plugin framework; launch
                                      one plugin at a time

  First product                       CHORD ENGINE

  Initial budget                      As close to \$0 as possible

  Revenue model                       Paid perpetual licenses; optional
                                      paid major-version upgrades

  Distribution                        Direct online sales

  Product divisions                   MIDI, FX, SOUND
  -----------------------------------------------------------------------

## 1.2 Product positioning

**Built for the Underground.**

DiskLordz Audio creates specialized production shortcuts and signature
sounds for producers. Products should help users solve recognizable
musical or sound-design problems quickly, without sacrificing control or
audio quality.

The company's differentiators should be:

-   Specialized musical knowledge and genre-aware workflows.
-   Genre-specific processing and original preset libraries.
-   Excellent sound quality and reliable MIDI behavior.
-   Fast, accessible interfaces.
-   Affordable pricing.
-   Customer-friendly installation, licensing, and updates.
-   A consistent visual and sonic identity.

------------------------------------------------------------------------

# 2. Company and Product Structure

Organize DiskLordz Audio into three product divisions. Products share
the company brand, framework, design system, licensing infrastructure,
and store, but remain independently installable and versioned.

## 2.1 DISKLORDZ MIDI

Composition, creativity, and performance tools.

Planned products:

1.  **CHORD ENGINE** --- chord construction, voicings, progressions, and
    MIDI export.
2.  **ARP MACHINE** --- advanced arpeggiator and rhythmic MIDI
    generator.
3.  **TRAP CHORDS** --- focused chord and progression tool/library.
4.  **PHONK SEQUENCER** --- genre-oriented melodic and rhythmic MIDI
    sequencing.

## 2.2 DISKLORDZ FX

Audio processing and sound manipulation.

Planned products:

1.  **808 SATURATOR** --- 808 harmonics, saturation, clipping, and sub
    preservation.
2.  **PHONK TAPE** --- tape/cassette coloration, wow, flutter, noise,
    and degradation.
3.  **DRUM CRUSH** --- drum-bus saturation, transient shaping, and
    parallel compression.
4.  **MEMPHIS GRIT** --- lo-fi distortion, sample degradation, and
    aggressive midrange.
5.  **DRIFT DESTROYER** --- multiband distortion, clipping, and extreme
    sound design.
6.  **SAMPLE DAMAGE** --- bit crushing, sample-rate reduction, aliasing,
    filtering, and glitches.

## 2.3 DISKLORDZ SOUND

Digital products and expansions.

Planned offerings:

-   MIDI progression libraries.
-   Chord and voicing expansions.
-   Genre-specific effect presets.
-   Producer toolkits.
-   Sound-design collections.
-   Product-specific preset expansions.

------------------------------------------------------------------------

# 3. First Flagship Product: CHORD ENGINE

**Status:** First product to build.

## 3.1 Product positioning

Create professional, musical, genre-specific chord progressions in
seconds.

CHORD ENGINE should be more than a basic chord generator. It should be a
specialized composition assistant for producers who want to create
interesting progressions without needing advanced music-theory
knowledge.

The product should generate actual MIDI notes and usable MIDI files, not
merely display chord names.

## 3.2 Core feature requirements

  -----------------------------------------------------------------------
  Module                              Function
  ----------------------------------- -----------------------------------
  Key detection                       Analyze incoming MIDI notes and
                                      estimate the likely key

  Scale engine                        Major, minor, modes, and custom
                                      scales

  Chord library                       Triads through extended chords

  Voicing engine                      Inversions, spreads, close
                                      voicings, and octave placement

  Progression builder                 Arrange multi-chord sequences

  Voice leading                       Minimize unnecessary note movement

  Bass support                        Generate matching bass notes and
                                      slash-chord voicings

  MIDI export                         Export generated progressions as
                                      MIDI files

  Randomizer                          Generate controlled musical
                                      variations

  Presets                             Save and recall custom progressions
                                      and settings
  -----------------------------------------------------------------------

Key detection should be presented as an estimate, not an infallible
result. The user must be able to override it manually.

## 3.3 Signature feature: Genre Intelligence

Genre Intelligence provides original, theory-based progression and
voicing rules. It must not copy copyrighted compositions or imitate
protected preset libraries.

Initial styles:

### Memphis Underground

-   Dark minor progressions.
-   Ominous seventh chords.
-   Diminished colors.
-   Sparse voicings.
-   Repetitive motifs and tension.

### Luxury Rap

-   Jazz-influenced ninths and thirteenths.
-   Extended harmonies.
-   Smooth voice leading.
-   Rich but playable voicings.

### Dark Trap

-   Minor keys.
-   Suspended chords.
-   Haunting intervals.
-   Cinematic movement.
-   Sparse or wide voicings.

### Soul / Quiet Storm

-   Gospel-inspired voicings.
-   Lush seventh chords.
-   Extended harmony.
-   Smooth transitions.

### Drift Phonk

-   Repetitive minor motifs.
-   Tense harmonies.
-   Dark melodic structures.
-   Short, loop-friendly progressions.

Genre labels should describe musical characteristics, not claim that one
progression is the definitive sound of a genre or artist.

## 3.4 Interface areas

The initial interface should have five primary areas:

1.  **Key & Scale:** choose root note and scale.
2.  **Chord Browser:** select chord types, extensions, and qualities.
3.  **Progression Grid:** arrange up to 16 chords.
4.  **Voicing Studio:** adjust inversions, octave, spread, and bass.
5.  **MIDI Export:** send or export the finished progression.

Prioritize clarity, speed, and reliability over excessive feature count.

## 3.5 Initial chord library

  Category    Chords
  ----------- -------------------------------------
  Basic       Major, minor, diminished, augmented
  Seventh     Maj7, min7, dom7, min7b5
  Extended    9, maj9, min9, 11, 13
  Suspended   Sus2, Sus4, 7sus4
  Altered     b9, #9, #11, b13
  Special     Add9, 6/9, slash chords

The theory engine must define chord formulas and interval spelling
consistently. It should distinguish enharmonic note names where useful
and preserve correct chord spelling for the selected key.

## 3.6 Example user workflow

User selections:

-   Key: F minor.
-   Genre: Dark Trap.
-   Progression length: 4 chords.
-   Complexity: Intermediate.
-   Voicing: Wide.

Expected behavior:

1.  Generate a progression using appropriate scale degrees and chord
    formulas.
2.  Apply the selected voicing constraints.
3.  Optimize voice leading within the selected style.
4.  Display chord names and notes.
5.  Allow the user to replace, reorder, or edit individual chords.
6.  Preview the progression using MIDI routed to a host instrument.
7.  Export the progression as a MIDI file or send MIDI to the DAW host
    where supported.

------------------------------------------------------------------------

# 4. Shared Technical Architecture

## 4.1 Core principle

Build the reusable **DiskLordz Plugin Framework (DPF)** first. CHORD
ENGINE should be the first product implemented on it.

Do not create a separate, unrelated codebase for every product. Reuse
common components while keeping MIDI processing and audio DSP as
distinct engine families.

## 4.2 Architecture overview

``` text
DiskLordz Audio
│
├── DPF-Core
│   ├── PluginFramework
│   ├── UserInterface
│   ├── PresetManager
│   ├── ParameterSystem
│   ├── StateManager
│   ├── Licensing
│   └── Utilities
│
├── DPF-MIDI
│   ├── MusicTheory
│   ├── ChordEngine
│   ├── VoicingEngine
│   ├── ProgressionEngine
│   ├── GenreEngine
│   └── MIDIProcessor
│
├── DPF-DSP
│   ├── Saturation
│   ├── Distortion
│   ├── Filtering
│   ├── Dynamics
│   └── Modulation
│
├── Products
│   ├── ChordEngine
│   ├── ArpMachine
│   ├── TrapChords
│   ├── 808Saturator
│   ├── PhonkTape
│   ├── DrumCrush
│   ├── MemphisGrit
│   ├── DriftDestroyer
│   └── SampleDamage
│
├── Tests
│   ├── MusicTheoryTests
│   ├── MIDI_Tests
│   ├── DSP_Tests
│   └── PluginTests
│
└── Build
    ├── Windows
    └── macOS
```

## 4.3 Recommended technology stack

  -------------------------------------------------------------------------
  Technology            Purpose                                 Cost target
  --------------------- ---------------------- ----------------------------
  C++                   Plugin programming                             Free

  JUCE                  Plugin framework and   Free Starter tier within its
                        host integration                              terms

  CMake                 Build automation                               Free

  Visual Studio         Windows compiler and    Free subject to eligibility
  Community             IDE                                           terms

  GitHub                Source control and                        Free tier
                        issue tracking         

  Python                Music-theory utilities                         Free
                        and test tooling       

  JSON                  Preset/configuration                           Free
                        data where appropriate 

  VST3 SDK              VST3 format support                MIT-licensed SDK
  -------------------------------------------------------------------------

Important licensing controls:

-   Review the current JUCE license and revenue ceiling before
    commercial release and before exceeding any applicable threshold.
-   Review the current VST3 SDK license and preserve required notices.
-   Check all third-party dependency licenses.
-   Do not assume that every free tool has unrestricted commercial-use
    terms.
-   Keep a software bill of materials and third-party license inventory.
-   Recheck licensing before each commercial release.

Prefer keeping music-theory and DSP logic independent of JUCE. The
framework should provide host integration, plugin lifecycle, parameters,
UI support, and platform abstraction without entangling the core
algorithms.

## 4.4 Separate MIDI and audio plugin architectures

MIDI generators and audio effects have different processing
responsibilities.

### MIDI products

Require:

-   MIDI input processing where applicable.
-   MIDI event generation.
-   MIDI output to the host.
-   Musical timing and host synchronization.
-   Host transport awareness.
-   Parameter automation.
-   Preset and state persistence.
-   Correct event ordering and note-off handling.
-   Reliable behavior during transport stop, seek, and loop boundaries.

### Audio effects

Require:

-   Audio input and output.
-   Real-time-safe DSP.
-   Correct handling of sample rates and buffer sizes.
-   No memory allocation or blocking operations in the audio callback.
-   Parameter smoothing where needed.
-   Stable denormal handling.
-   Correct channel layouts.
-   Latency reporting when processing introduces latency.
-   CPU and memory profiling.

Do not force MIDI and audio products into one processing model simply to
make the code look uniform.

## 4.5 Shared framework components

Implement reusable versions of:

-   Plugin project/build configuration.
-   Parameter definitions and serialization.
-   Preset manager.
-   Plugin state save/restore.
-   Common interface components.
-   Knobs, sliders, switches, meters, and displays.
-   Theme and typography system.
-   Product identity/configuration.
-   Logging and diagnostics that are safe for release builds.
-   Version information.
-   Installer packaging conventions.
-   Automated test infrastructure.

The framework must support product-specific interfaces and features
without making every product look or behave identically.

------------------------------------------------------------------------

# 5. CHORD ENGINE Technical Specification

Build CHORD ENGINE as a modular product with seven primary engineering
modules.

  -----------------------------------------------------------------------
  Module                              Responsibilities
  ----------------------------------- -----------------------------------
  Theory Engine                       Notes, keys, scales, intervals,
                                      spelling, and chord construction

  Chord Engine                        Chord formulas, qualities,
                                      extensions, and inversions

  Voicing Engine                      Voice leading, octave placement,
                                      spread, and register

  Progression Engine                  Sequences, progression generation,
                                      and controlled variation

  Genre Engine                        Original genre-oriented rule sets
                                      and presets

  MIDI Engine                         MIDI events, timing, host
                                      synchronization, and export

  Interface Engine                    Key selector, browser, progression
                                      grid, voicing controls, and export
  -----------------------------------------------------------------------

## 5.1 Theory engine

Implement and test:

-   MIDI note to pitch-class conversion.
-   Pitch-class to note-name conversion.
-   Enharmonic spelling.
-   Interval calculation.
-   Scale construction.
-   Scale-degree calculation.
-   Chord construction from interval formulas.
-   Chord inversion.
-   Slash-chord/bass-note handling.
-   Chord symbol generation.
-   Key-aware spelling.

The theory engine should be deterministic and independent of the user
interface.

## 5.2 Chord engine

Implement chord formulas as data-driven definitions where practical. A
chord definition should include:

-   Unique identifier.
-   Display name.
-   Symbol.
-   Interval formula.
-   Allowed/typical scale contexts where known.
-   Default voicing behavior.
-   Optional genre tags.
-   Versioned schema identifier.

Avoid hardcoding chord logic separately in UI components.

## 5.3 Voicing engine

Provide:

-   Root position and inversions.
-   Close and open voicings.
-   Spread controls.
-   Octave displacement.
-   Voice-leading optimization.
-   Minimum and maximum note range.
-   Optional bass note generation.
-   Note-count constraints.
-   Configurable voice-leading cost weights.

Voice-leading should be configurable. Do not assume that the
mathematically smallest movement is always the most musical result.

## 5.4 Progression engine

Provide:

-   User-selected progression length, initially 1--16 chords.
-   Scale-degree-based progression generation.
-   Chord substitutions within defined constraints.
-   Controlled randomization.
-   Repeat and variation controls.
-   Replace-one-chord behavior.
-   Undo/redo.
-   User editing and reordering.
-   Genre profile constraints.
-   Reproducible random seeds for debugging and repeatable generation.

## 5.5 Genre engine

Represent genre profiles as versioned data/configuration, not scattered
conditional statements.

Each profile can define:

-   Preferred scale types.
-   Preferred chord qualities.
-   Common chord-extension weights.
-   Voicing density.
-   Typical register.
-   Voice-leading preferences.
-   Progression-length preferences.
-   Variation and repetition weights.
-   Optional bass-note behavior.

All generation must remain musically valid and editable. Genre profiles
are starting points, not rigid rules.

## 5.6 MIDI engine

Implement:

-   MIDI event generation.
-   Correct note-on/note-off pairing.
-   Configurable note duration and gate.
-   Host tempo and transport synchronization where supported.
-   Correct handling of play, stop, seek, and loop.
-   No stuck notes during state changes or transport stops.
-   Correct event timestamps within host blocks.
-   MIDI export to Standard MIDI File.
-   Export verification through round-trip tests.

Investigate DAW-specific MIDI routing and VST3 MIDI-effect support
early. Do not promise universal MIDI output compatibility until verified
in each target DAW.

## 5.7 Presets and state

Support:

-   Factory progression presets.
-   Factory genre profiles.
-   User presets.
-   Save/load.
-   State restoration after closing and reopening a project.
-   Schema versioning and migration strategy.
-   Graceful handling of missing or malformed preset files.

Presets should use a documented, versioned format. JSON is suitable for
readable configuration and progression data, but binary formats may be
used where technically justified.

## 5.8 User interface

Initial screen areas:

1.  Key & Scale.
2.  Chord Browser.
3.  Progression Grid.
4.  Voicing Studio.
5.  MIDI Export.

Initial UI requirements:

-   Clear selected key and scale.
-   Readable chord names.
-   Display notes in each chord.
-   Selectable chord slots.
-   Drag/reorder if practical.
-   Replace and randomize controls.
-   Undo/redo.
-   Preset browser.
-   MIDI preview/output status.
-   Accessible labels and sensible scaling.
-   No decorative control that does not perform a real action.

## 5.9 First-release acceptance criteria

CHORD ENGINE is not commercially ready until it can:

1.  Generate musically valid chords.
2.  Generate coherent progressions.
3.  Respect selected key and scale constraints.
4.  Produce usable MIDI output in a supported host.
5.  Synchronize correctly with DAW transport where supported.
6.  Save and restore plugin state.
7.  Export MIDI without losing notes or timing.
8.  Operate without audio dropouts or crashes.
9.  Handle different DAW buffer sizes and sample rates.
10. Pass plugin validation and real-world DAW testing.
11. Avoid stuck MIDI notes during stop, seek, loop, and preset changes.
12. Provide useful documentation and a reliable installer.

------------------------------------------------------------------------

# 6. Product Catalog and Proposed Pricing

These prices are initial hypotheses, not validated market prices.
Validate willingness to pay through interviews, beta testing, and early
sales.

## 6.1 MIDI products

### CHORD ENGINE

Proposed introductory price: **\$29**\
Proposed regular price: **\$49**

Features: - Chord progression generator. - Key and scale selection. -
Chord types and extensions. - Voicing and inversions. - Genre
Intelligence. - MIDI output and export. - Presets and controlled
randomization.

### ARP MACHINE

Proposed introductory price: **\$19**\
Proposed regular price: **\$39**

Features: - Up, down, up/down, and random modes. - Note divisions from
1/4 through 1/64 where supported. - Triplets and swing. - Ratchets and
note repeats. - Octave range. - Velocity and gate controls. - Pattern
randomization. - MIDI output and export.

### TRAP CHORDS

Proposed price: **\$19--\$29**

Features: - Dark minor progressions. - Emotional trap voicings. - Piano
and bell-friendly chord structures. - Suspended chords. -
Gospel-inspired options. - One-click MIDI generation.

### PHONK SEQUENCER

Proposed price: To be validated.

Features: - Loop-oriented melodic sequencing. - Minor-key motifs. -
Repetition and variation. - Step and probability controls. - MIDI
export.

## 6.2 FX products

### 808 SATURATOR

Proposed introductory price: **\$19**\
Proposed regular price: **\$39**

Features: - Tube-style saturation model. - Soft clipping. - Hard
clipping. - Harmonic generation. - Sub preservation. - Parallel mix. -
Input/output compensation. - Oversampling options where justified.

### PHONK TAPE

Proposed price: **\$29--\$39**

Features: - Tape/cassette-style saturation. - Wow and flutter. - Noise
and hiss. - Frequency degradation. - Stereo instability. - Optional
compression and filtering.

### DRUM CRUSH

Proposed price: **\$29--\$39**

Features: - Drum-bus saturation. - Transient emphasis. - Parallel
compression. - Low-end drive. - High-frequency crunch. - Punch and
density controls.

### MEMPHIS GRIT

Proposed price: **\$19--\$39**

Features: - Aggressive overdrive. - Lo-fi distortion. - Sample-rate
reduction. - Bit crushing. - Midrange aggression. -
Vintage-sampler-inspired degradation.

### DRIFT DESTROYER

Proposed price: **\$39--\$49**

Features: - Multiband distortion. - Aggressive clipping. -
Frequency-band separation. - Frequency-specific drive. - Stereo
destruction. - Extreme sound design.

### SAMPLE DAMAGE

Proposed price: **\$19--\$29**

Features: - Bit-depth reduction. - Sample-rate reduction. - Digital
aliasing. - Pitch degradation. - Filtering. - Randomized glitches.

## 6.3 Bundle hypotheses

  Bundle                       Introductory target   Regular target
  -------------------------- --------------------- ----------------
  Three-plugin bundle                   \$69--\$89         Validate
  Complete MIDI bundle                        \$49             \$79
  Complete producer bundle                    \$99            \$149

Bundle contents and pricing should be tested with customers. Avoid
promising products that have not shipped.

------------------------------------------------------------------------

# 7. Sound Quality and DSP Design Principles

The FX family must be built around sound quality, controllability, and
measurable behavior---not just dramatic presets.

## 7.1 808 SATURATOR

Priorities:

-   Preserve fundamental/sub energy.
-   Generate useful upper harmonics.
-   Avoid uncontrolled low-frequency clipping.
-   Provide input/output gain compensation.
-   Include wet/dry control.
-   Compare at loudness-matched levels.
-   Provide oversampling where it materially improves nonlinear
    processing.
-   Avoid aliasing artifacts unless intentionally selected.

## 7.2 MEMPHIS GRIT and SAMPLE DAMAGE

Potential processing stages:

-   Drive/overdrive.
-   Soft or hard clipping.
-   Bit-depth reduction.
-   Sample-rate reduction.
-   Filtering.
-   Noise.
-   Optional modulation.
-   Wet/dry mix.

Provide safe gain staging and output control. Do not normalize presets
in ways that create unexpected dangerous output levels.

## 7.3 PHONK TAPE

Potential stages:

-   Saturation.
-   Frequency response shaping.
-   Wow/flutter modulation.
-   Noise.
-   Stereo instability.
-   Optional compression.

Use original algorithms and appropriately licensed reference material
only. Do not claim exact hardware emulation unless it has been measured
and validated.

## 7.4 DSP engineering rules

-   Real-time audio thread must not block.
-   Avoid heap allocation in the audio callback.
-   Avoid file, network, or license-server operations in the audio
    callback.
-   Smooth parameters to avoid zipper noise.
-   Test at common sample rates and buffer sizes.
-   Handle mono, stereo, and supported channel layouts.
-   Profile CPU usage.
-   Provide bypass and output gain controls.
-   Test for denormals and numerical instability.
-   Document latency and oversampling behavior.

------------------------------------------------------------------------

# 8. Visual Identity and Product UX

## 8.1 Brand direction

DiskLordz Audio should have a consistent, recognizable
underground-production identity.

Suggested visual direction:

-   Black.
-   Gunmetal.
-   Charcoal.
-   White and cool gray.
-   Restrained accent colors.
-   Industrial, premium, hardware-inspired visual language.
-   Strong readability and contrast.
-   Avoid unnecessary visual clutter.

## 8.2 Shared UI system

Build reusable components for:

-   Knobs.
-   Sliders.
-   Switches.
-   Buttons.
-   Meters.
-   Displays.
-   Preset browser.
-   Tooltips.
-   Tabs.
-   Product logo and version display.

The shared UI system should provide consistency without forcing all
plugins into an identical layout.

## 8.3 UX principles

-   A new user should understand the main task quickly.
-   Important controls should be visible.
-   Every control must have a defined function.
-   Provide useful defaults.
-   Avoid excessive modal dialogs.
-   Support keyboard and mouse interaction.
-   Ensure text remains readable at supported window sizes.
-   Make preset loading and saving obvious.
-   Make installation and updates straightforward.

------------------------------------------------------------------------

# 9. Development Roadmap

The following is a planning framework for one primary developer using
AI-assisted development. It is not a guaranteed delivery schedule.
Adjust based on actual available development hours, build quality, and
testing results.

## Month 1 --- Company and framework foundation

Tasks: - Establish DiskLordz Audio naming and visual direction. - Create
a private GitHub organization/repository. - Install and verify the
development environment. - Review JUCE and VST3 SDK licensing. - Build
the shared plugin skeleton. - Define code conventions and testing
standards. - Create build documentation.

Deliverable: Compiling plugin foundation.

## Months 2--3 --- CHORD ENGINE alpha

Tasks: - Build music theory engine. - Create chord library. - Develop
progression generator. - Implement voicing engine. - Implement MIDI
output. - Build initial user interface. - Create first genre profiles. -
Add basic preset support.

Deliverable: Functional alpha.

## Month 4 --- Beta testing

Tasks: - Test DAW compatibility. - Fix MIDI timing and routing. - Add
and validate presets. - Test state persistence. - Recruit 15--25
independent producers. - Collect bug reports and workflow feedback. -
Prioritize fixes based on severity.

Deliverable: Public beta candidate.

## Month 5 --- Commercial launch preparation

Tasks: - Build product website. - Create installation package. -
Implement initial license management. - Write documentation. - Record
demo videos. - Confirm pricing. - Prepare support and refund policies. -
Complete release testing.

Deliverable: Release candidate.

## Month 6 --- CHORD ENGINE launch

Tasks: - Launch the sales website. - Release first commercial version. -
Collect customer feedback. - Track support requests. - Publish tutorials
and demonstrations. - Fix high-priority bugs.

Deliverable: First commercial product.

## Months 7--8 --- ARP MACHINE

Tasks: - Reuse MIDI framework. - Create arpeggiator engine. - Add rhythm
patterns. - Add probability and ratchets. - Test host synchronization. -
Introduce product bundles.

Deliverable: Second plugin.

## Months 9--10 --- 808 SATURATOR

Tasks: - Develop DSP foundation. - Implement saturation models. - Add
harmonic generation. - Add sub preservation. - Perform loudness-matched
testing. - Profile CPU and stability.

Deliverable: First audio effect.

## Months 11--12 --- MEMPHIS GRIT

Tasks: - Develop distortion engine. - Add bit crushing and sample-rate
reduction. - Design preset library. - Prepare effects launch. - Evaluate
macOS development readiness.

Deliverable: Fourth product milestone.

------------------------------------------------------------------------

# 10. Repository and Source-Code Organization

Establish a private repository with this proposed structure. Adjust only
when there is a documented engineering reason.

``` text
DiskLordz-Audio/
│
├── README.md
├── LICENSES/
├── DOCUMENTATION/
│
├── DPF-Core/
│   ├── PluginFramework/
│   ├── UserInterface/
│   ├── PresetManager/
│   ├── ParameterSystem/
│   ├── StateManager/
│   ├── Licensing/
│   └── Utilities/
│
├── DPF-MIDI/
│   ├── MusicTheory/
│   ├── ChordEngine/
│   ├── VoicingEngine/
│   ├── ProgressionEngine/
│   ├── GenreEngine/
│   └── MIDIProcessor/
│
├── DPF-DSP/
│   ├── Saturation/
│   ├── Distortion/
│   ├── Filtering/
│   ├── Dynamics/
│   └── Modulation/
│
├── Products/
│   ├── ChordEngine/
│   ├── ArpMachine/
│   ├── TrapChords/
│   ├── PhonkSequencer/
│   ├── 808Saturator/
│   ├── PhonkTape/
│   ├── DrumCrush/
│   ├── MemphisGrit/
│   ├── DriftDestroyer/
│   └── SampleDamage/
│
├── Tests/
│   ├── MusicTheoryTests/
│   ├── MIDI_Tests/
│   ├── DSP_Tests/
│   └── PluginTests/
│
└── Build/
    ├── Windows/
    └── macOS/
```

Repository rules:

-   Keep source code private.
-   Use meaningful commits.
-   Use branches for substantial features.
-   Tag release versions.
-   Document build steps.
-   Keep generated build artifacts out of source control.
-   Never commit credentials, private keys, license secrets, or customer
    data.
-   Keep dependencies pinned and documented.
-   Add CI builds and tests when practical.
-   Every product has its own version number, presets, documentation,
    and release package.
-   The shared framework has its own versioning and compatibility
    policy.

------------------------------------------------------------------------

# 11. Testing and Compatibility Strategy

## 11.1 Initial DAW test priorities

  DAW             Priority
  --------------- -----------
  FL Studio       Essential
  REAPER          Essential
  Ableton Live    High
  Studio One      High
  Cubase          High
  Bitwig Studio   Later

Test actual MIDI routing and host behavior. A VST3 that generates MIDI
does not necessarily behave identically in every DAW. Verify the
workflow in each target host before advertising compatibility.

## 11.2 Test categories

### Music theory tests

-   Note and pitch-class conversion.
-   Interval calculations.
-   Scale construction.
-   Chord formulas.
-   Enharmonic spelling.
-   Inversions.
-   Slash chords.
-   Voice-leading constraints.

### MIDI tests

-   Note-on/note-off pairing.
-   Event ordering.
-   Timing and block offsets.
-   Transport start and stop.
-   Seek and loop behavior.
-   No stuck notes.
-   MIDI file export and import round trips.

### Plugin tests

-   Plugin scanning.
-   Plugin validation.
-   State save and restore.
-   Preset loading.
-   Parameter automation.
-   Multiple instances.
-   Buffer-size variation.
-   Sample-rate variation.
-   Crash and stability testing.

### DSP tests

-   Frequency response.
-   Harmonic behavior.
-   Clipping behavior.
-   Output gain.
-   Parameter smoothing.
-   CPU usage.
-   Numerical stability.
-   Mono/stereo behavior.

------------------------------------------------------------------------

# 12. Business Model and Sales Infrastructure

## 12.1 Revenue model

Use perpetual licenses for individual products. Consider paid
major-version upgrades rather than mandatory monthly subscriptions.

Initial pricing hypotheses:

  Product                      Launch price   Regular target
  -------------------------- -------------- ----------------
  CHORD ENGINE                         \$29             \$49
  ARP MACHINE                          \$19             \$39
  808 SATURATOR                        \$19             \$39
  MEMPHIS GRIT                         \$19             \$39
  Complete MIDI bundle                 \$49             \$79
  Complete producer bundle             \$99            \$149

These are test prices, not forecasts or validated market facts.

## 12.2 Online store

The DiskLordz Audio website should eventually include:

-   Company homepage.
-   Product catalog.
-   Individual plugin landing pages.
-   Audio demonstrations.
-   Video tutorials.
-   Secure checkout.
-   Digital delivery.
-   License keys.
-   Customer accounts.
-   Download and update portal.
-   Knowledge base.
-   Support center.
-   Bundle discounts.
-   Expansion packs.

Keep the first store implementation simple. Do not build a complex
custom e-commerce backend before the first product has proven demand.
Evaluate established commerce and digital-delivery services against
fees, license integration, tax handling, and customer experience.

## 12.3 Customer acquisition

Prioritize organic marketing initially.

  Channel           Content
  ----------------- ----------------------------------------------------
  YouTube           Demonstrate plugins in actual beat-making sessions
  TikTok            Short before/after sound demonstrations
  Instagram         Plugin demonstrations and product updates
  Reddit            Participate helpfully in producer communities
  Producer forums   Beta testing and technical discussions
  Email             Product launches, updates, and preset expansions

Marketing principle:

**Show a producer a problem. Load DiskLordz. Solve the problem in 30
seconds.**

Examples: - CHORD ENGINE: turn a single minor chord into a four-chord
progression and export MIDI. - 808 SATURATOR: compare clean and
processed 808s at matched loudness. - MEMPHIS GRIT: demonstrate an
ordinary sample transformed with gritty, degraded processing.

Do not spam communities or misrepresent product capabilities.

## 12.4 Free product strategy

Eventually release CHORD ENGINE LITE.

Potential limitations: - Limited chord library. - Four-chord
progressions. - Basic key and scale selection. - Small genre preset
selection. - No advanced voicing tools.

The paid edition unlocks the complete feature set.

------------------------------------------------------------------------

# 13. Licensing, Ownership, and Legal Controls

DiskLordz Audio should own or have appropriate commercial rights to its
intellectual property.

Company assets include:

-   Original source code.
-   Original DSP algorithms.
-   Music-theory and progression-generation logic.
-   Original user-interface assets.
-   Original preset libraries.
-   Product names and branding, subject to trademark clearance.
-   Documentation and product-specific content.

Policies:

1.  Maintain a record of third-party dependencies and licenses.
2.  Do not copy competitor code, proprietary presets, or protected
    interfaces.
3.  Include required open-source notices.
4.  Keep source code private.
5.  Use commercial licenses for generated assets and fonts where
    required.
6.  Establish an end-user license agreement before commercial release.
7.  Review trademark availability before investing heavily in product
    branding.
8.  Do not promise protection against piracy that the system cannot
    provide.

Start with a straightforward license-key system, reasonable activation
limits, and a customer-friendly offline fallback. Do not build elaborate
copy protection before sales justify it.

License checks and network operations must never block or run inside the
real-time audio callback.

------------------------------------------------------------------------

# 14. Startup Budget

Initial objective: build the first product without hiring developers or
purchasing expensive development subscriptions.

  Item                                     Initial cost target
  ---------------------------- -------------------------------
  C++                                                      \$0
  JUCE Starter                   \$0, subject to license terms
  VST3 SDK                                                 \$0
  CMake                                                    \$0
  Visual Studio Community          \$0, subject to eligibility
  GitHub                                              \$0 tier
  Python                                                   \$0
  Initial preset development                               \$0
  Local testing                                            \$0
  Domain name                                         Optional
  **Target initial spend**                        **Near \$0**

Future expenses to plan for:

-   JUCE license upgrade when required.
-   Code signing and installer distribution.
-   Domain and web hosting.
-   Payment processing.
-   License server hosting.
-   macOS testing and signing.
-   Support and customer-service infrastructure.

Financial control: review the applicable JUCE Starter revenue ceiling
and licensing terms before commercial release and before approaching any
threshold. Budget for a license upgrade if needed.

------------------------------------------------------------------------

# 15. Business Performance Targets

These are internal planning targets, not guarantees.

  Metric                        Initial target
  --------------------------- ----------------
  First commercial product                   1
  Initial beta testers                  15--25
  First 90-day sales target                 50
  Customer rating target                 4.5/5
  Critical launch bugs                       0
  Products in year one                    3--4
  Email subscribers                        250
  Refund rate target                  Under 5%

Also measure:

-   Product-page conversion rate.
-   Demo-to-purchase conversion.
-   Support requests per sale.
-   Repeat purchase rate.
-   Bundle attachment rate.
-   Refund reasons.
-   Crash and stability reports.
-   Compatibility issues by DAW.
-   Time required to resolve support issues.

Use these metrics to make product and pricing decisions rather than
relying on vanity metrics.

------------------------------------------------------------------------

# 16. First 30-Day Startup Sprint

The first sprint should produce a working plugin foundation and an
initial CHORD ENGINE prototype.

## Startup checklist

-   [ ] Establish DiskLordz Audio naming, logo direction, and visual
    system.
-   [ ] Install and verify C++, JUCE, CMake, and Visual Studio.
-   [ ] Create the private company repository and folder structure.
-   [ ] Build a blank VST3 plugin that loads in a test host.
-   [ ] Verify that generated MIDI notes reach a DAW instrument.
-   [ ] Implement notes, intervals, keys, and scales.
-   [ ] Implement triads, sevenths, and extended chords.
-   [ ] Generate and validate four-chord progressions.
-   [ ] Build the key selector, chord browser, and progression grid.
-   [ ] Package the first working CHORD ENGINE prototype.

## Milestone 001: DiskLordz Audio Plugin Foundation

The first coding project should:

1.  Set up the JUCE project.
2.  Compile a Windows VST3.
3.  Create the DiskLordz interface shell.
4.  Implement the shared parameter system.
5.  Implement preset save/load.
6.  Implement MIDI event generation.
7.  Verify output in REAPER or another compatible test host.
8.  Establish automated build and test procedures.

Once this works, the company has the foundation for its first commercial
product.

------------------------------------------------------------------------

# 17. Required Strategic Documents

Maintain three governing documents in the repository.

## `DISKLORDZ_MASTER_PLAN.md`

Contains: - Business strategy. - Product catalog. - Positioning. -
Pricing hypotheses. - Sales strategy. - Marketing strategy. - Roadmap. -
Business targets.

## `DPF_TECHNICAL_ARCHITECTURE.md`

Contains: - Shared framework architecture. - Technology decisions. -
MIDI and DSP architecture. - Build system. - Coding standards. -
Dependency and licensing inventory. - Testing strategy. - Release and
compatibility policy.

## `CHORD_ENGINE_PRD.md`

Contains: - Complete product requirements. - User stories. - UI
requirements. - Music-theory behavior. - Genre Intelligence rules. -
MIDI behavior. - Preset and state requirements. - Acceptance criteria. -
Test cases.

Keep these documents synchronized. When implementation reveals a
necessary change, update the relevant specification rather than silently
diverging from it.

------------------------------------------------------------------------

# 18. Instructions for Cursor and AI Coding Agents

This document is the product and business source of truth. Cursor should
use it to plan and implement the project incrementally.

## 18.1 Operating rules

1.  Do not attempt to build all products at once.
2.  Begin with the shared plugin foundation and CHORD ENGINE.
3.  Inspect the existing repository before creating or changing files.
4.  If no repository exists, propose and create the structure in this
    document.
5.  Prefer small, buildable implementation steps.
6.  Do not claim a feature is complete unless it compiles and its tests
    pass.
7.  Do not create fake UI controls or nonfunctional placeholders while
    claiming completion.
8.  Do not add unnecessary dependencies.
9.  Check licenses before introducing third-party code or assets.
10. Keep MIDI and DSP engines modular.
11. Keep real-time audio processing safe.
12. Do not perform network operations in the audio callback.
13. Write unit tests for music-theory and MIDI behavior.
14. Build and test after each meaningful implementation stage.
15. Record known limitations and unresolved compatibility issues.
16. Do not silently remove requirements.
17. Do not add paid services without first explaining why they are
    needed and identifying a free alternative.
18. Do not expose credentials, signing keys, or license secrets.
19. Do not implement licensing in a way that can interrupt audio
    processing.
20. Update documentation as architecture and behavior evolve.

## 18.2 Required implementation sequence

### Stage A --- Repository and environment audit

-   Inspect installed compilers, CMake, JUCE, and Git.
-   Verify operating system and architecture.
-   Identify existing source files.
-   Check dependency versions and licenses.
-   Document missing prerequisites.
-   Do not overwrite existing work without inspection.

### Stage B --- Build foundation

-   Create the smallest valid JUCE VST3 project.
-   Configure CMake.
-   Compile a Windows 64-bit VST3.
-   Confirm the artifact exists at the expected build path.
-   Load it in a compatible DAW.
-   Document exact build and installation steps.

### Stage C --- Shared framework

-   Create reusable parameter definitions.
-   Implement state serialization.
-   Implement preset save/load.
-   Establish common UI components.
-   Establish versioning and product configuration.
-   Add automated tests.

### Stage D --- Music theory engine

-   Implement note and interval operations.
-   Implement scale construction.
-   Implement chord formulas.
-   Implement inversions.
-   Add comprehensive unit tests.
-   Verify enharmonic and edge cases.

### Stage E --- CHORD ENGINE functionality

-   Implement progression generation.
-   Implement voicing and voice-leading controls.
-   Implement genre profiles.
-   Implement MIDI generation and host synchronization.
-   Implement MIDI export.
-   Add undo/redo and editing.
-   Add factory presets.

### Stage F --- UI and product integration

-   Implement the five primary interface areas.
-   Connect every UI control to actual functionality.
-   Add error and status feedback.
-   Verify state restoration.
-   Test usability at supported window sizes.

### Stage G --- Validation and release candidate

-   Run unit and integration tests.
-   Run plugin validation.
-   Test target DAWs.
-   Test transport stop, seek, and loop.
-   Test for stuck notes.
-   Test preset and state persistence.
-   Test installer and uninstaller.
-   Prepare documentation and known-issues list.

## 18.3 Cursor task reporting format

At the end of each implementation task, report:

1.  **Completed:** What was actually implemented.
2.  **Files changed:** Exact file paths.
3.  **Build result:** Build command and result.
4.  **Tests:** Tests run and results.
5.  **Limitations:** Known gaps or compatibility concerns.
6.  **Next task:** The single highest-priority next implementation step.

Never report success based only on generated code. Verify the build and
behavior.

## 18.4 Definition of done

A task is complete only when:

-   Its implementation exists.
-   The code compiles where applicable.
-   Relevant tests pass.
-   The behavior has been verified or its verification limitation is
    explicitly documented.
-   Documentation is updated.
-   No unrelated functionality has been broken.

------------------------------------------------------------------------

# 19. Long-Term Vision

DiskLordz Audio should become a recognizable underground
music-production software brand.

Long-term opportunities include:

-   A complete MIDI plugin family.
-   A complete saturation and distortion family.
-   Original preset and progression libraries.
-   Standalone sound-design tools.
-   MPC utilities and workflows.
-   Sample and sound-generation tools.
-   macOS versions.
-   Additional DAW formats if justified.
-   Educational production content.

Keep DSP engines and music-theory engines independent so they can
eventually support standalone applications, MPC-related utilities, and
other music-production software.

The company's guiding principle:

**Build the foundation once. Sell individual products repeatedly. Grow
DiskLordz Audio into a complete underground music-production software
brand.**

------------------------------------------------------------------------

# 20. Immediate Cursor Assignment

**Start with Milestone 001: DiskLordz Audio Plugin Foundation.**

Do not attempt the full product catalog yet.

First inspect the development environment and repository. Then produce a
concise environment report and an implementation plan for the smallest
working Windows VST3 using the selected framework.

After approval or when operating autonomously within the repository's
existing conventions, implement the build foundation in small steps.
Verify that the plugin compiles and loads in a compatible DAW before
proceeding to the music-theory engine.

The first tangible result must be a working, testable plugin
artifact---not merely a mockup, architecture diagram, or generated
source tree.
