"""Simple checks for deterministic rendering and generated WAV consistency."""
import numpy as np
import forge_core_v2 as core
import drum_forge_v4 as forge

def test_notes():
    assert forge.parse_midi('C1')==24
    assert forge.parse_midi('Bb0')==22
    assert forge.parse_midi('F#1')==30

def test_reproducibility():
    p=core.PRESETS['memphis']
    a,_=core.gen_808(44100,np.random.default_rng(123),p,24)
    b,_=core.gen_808(44100,np.random.default_rng(123),p,24)
    assert np.array_equal(a,b)

def test_audio():
    x,r=core.gen_808(44100,np.random.default_rng(42),core.PRESETS['drift_phonk'],24)
    assert np.all(np.isfinite(x))
    y=core.master(forge.final_trim(x,44100),44100,-1.12)
    assert core.true_peak_db(y)<=-1.10
    detected,confidence=core.estimate_808_pitch(y,44100,r)
    assert detected is not None and abs(12*np.log2(detected/r))<.5 and confidence>.48

def test_seven_genres():
    expected={'atl_trap','memphis','drift_phonk','space_trap','southern_trap','cloud_trap','raw_underground'}
    assert expected<=set(core.PRESETS)

def test_pitch_holds_on_every_genre():
    for name,preset in core.PRESETS.items():
        x,root=core.gen_808(44100,np.random.default_rng(7),preset,24)
        y=core.master(forge.final_trim(x,44100),44100,-1.12)
        detected,confidence=core.estimate_808_pitch(y,44100,root)
        cents=abs(12*np.log2(detected/root))
        assert detected is not None and cents<.5 and confidence>.48, (name,detected,cents,confidence)
        assert -1.30<=core.true_peak_db(y)<=-1.05

def test_clap_variations_are_not_copies():
    import drum_forge_pro as pro
    vectors=[]
    for index,spread in enumerate((0.0,1.0)):
        rng=np.random.default_rng(80+index)
        preset,_=pro.mutate_preset(core.PRESETS['memphis'],rng,'clap',0.9,spread)
        audio,_=core.gen_clap(44100,rng,preset)
        audio=core.master(audio,44100,-1.12)
        vector,_,_=pro.descriptors(audio,44100)
        vectors.append(vector)
    assert float(np.dot(vectors[0],vectors[1]))<0.995

def test_hat_partials_stay_below_nyquist():
    audio,_=core.gen_hat(48000,np.random.default_rng(3),core.PRESETS['drift_phonk'],False)
    window=min(len(audio),4096)
    spectrum=np.abs(np.fft.rfft(audio[:window]*np.hanning(window)))**2
    freqs=np.fft.rfftfreq(window,1/48000)
    peak=float(spectrum[(freqs>2000)&(freqs<10000)].max())
    floor=float(np.median(spectrum[freqs>18000]))+1e-12
    assert peak>floor*20

if __name__=='__main__':
    test_notes();test_reproducibility();test_audio();test_seven_genres()
    test_pitch_holds_on_every_genre();test_clap_variations_are_not_copies()
    test_hat_partials_stay_below_nyquist();print('All tests passed')
