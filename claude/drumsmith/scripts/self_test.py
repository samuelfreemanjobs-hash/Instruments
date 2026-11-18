"""Deterministic executable checks. Run from any working directory."""
import hashlib
import json
import math
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).parent))
from dsp_math import amplitude_from_db, cents_difference, dbfs, estimate_fundamental_autocorrelation, hz_to_midi, midi_to_hz, rms
from scaffold_kit import scaffold
from render_reference import render_kit
from verify_kit import measure, safe_relative, verify
from pitch_check import inspect_pitch, windowed_spectral_peak


class MathTests(unittest.TestCase):
    def test_db_zero(self): self.assertEqual(dbfs(1), 0)
    def test_db_minus_six(self): self.assertAlmostEqual(dbfs(0.5), -6.020599913, places=5)
    def test_silence(self): self.assertEqual(dbfs(0), -math.inf)
    def test_amplitude(self): self.assertAlmostEqual(amplitude_from_db(-6.020599913), 0.5, places=6)
    def test_a440(self): self.assertEqual(hz_to_midi(440), 69)
    def test_a440_inverse(self): self.assertEqual(midi_to_hz(69), 440)
    def test_middle_c(self): self.assertAlmostEqual(midi_to_hz(60), 261.625565, places=4)
    def test_cents(self): self.assertAlmostEqual(cents_difference(880, 440), 1200)
    def test_rms(self): self.assertAlmostEqual(rms([1, -1, 1, -1]), 1)
    def test_pitch(self):
        wave = [math.sin(2*math.pi*100*i/4000) for i in range(800)]
        result = estimate_fundamental_autocorrelation(wave, 4000, 80, 140)
        self.assertAlmostEqual(result['hz'], 100, delta=3)
    def test_pitch_invalid(self): self.assertIsNone(estimate_fundamental_autocorrelation([], 44100))


class SecurityTests(unittest.TestCase):
    def test_safe(self): self.assertTrue(safe_relative('masters/SOUND.wav'))
    def test_traversal(self): self.assertFalse(safe_relative('../evil.wav'))
    def test_absolute(self): self.assertFalse(safe_relative('/tmp/evil.wav'))
    def test_windows(self): self.assertFalse(safe_relative('..\\evil.wav'))


class IntegrationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.kit = Path(self.temp.name) / 'kit'
        scaffold(self.kit, 'TLS', 3)
    def tearDown(self): self.temp.cleanup()
    def test_unrendered_fails(self): self.assertEqual(verify(self.kit, self.kit/'manifest.json')['status'], 'FAIL')
    def test_render_works(self): self.assertEqual(render_kit(self.kit), 3)
    def test_render_passes_qa(self):
        render_kit(self.kit)
        r = verify(self.kit, self.kit/'manifest.json')
        self.assertEqual(r['status'], 'PASS', r['errors'])
        self.assertFalse(r['release_ready'])
    def test_wav_properties(self):
        render_kit(self.kit)
        m = measure(next((self.kit/'masters').glob('*.wav')))
        self.assertEqual(m['sample_rate'], 44100)
        self.assertEqual(m['bit_depth'], 16)
    def test_hash_tamper(self):
        render_kit(self.kit)
        p = next((self.kit/'masters').glob('*.wav'))
        with p.open('ab') as f: f.write(b'changed')
        self.assertEqual(verify(self.kit, self.kit/'manifest.json')['status'], 'FAIL')
    def test_extra_file(self):
        render_kit(self.kit)
        p = next((self.kit/'masters').glob('*.wav'))
        (self.kit/'masters'/'extra.wav').write_bytes(p.read_bytes())
        self.assertIn('unlisted WAV', ' '.join(verify(self.kit, self.kit/'manifest.json')['errors']))
    def test_manifest_duplicate(self):
        render_kit(self.kit)
        m = json.loads((self.kit/'manifest.json').read_text())
        m['samples'].append(m['samples'][0])
        (self.kit/'manifest.json').write_text(json.dumps(m))
        self.assertEqual(verify(self.kit, self.kit/'manifest.json')['status'], 'FAIL')

    def test_evidence_required_for_release(self):
        render_kit(self.kit)
        m = json.loads((self.kit/'manifest.json').read_text())
        m['human_listening_approved'] = True
        m['release_approved'] = True
        (self.kit/'manifest.json').write_text(json.dumps(m))
        r = verify(self.kit, self.kit/'manifest.json')
        self.assertFalse(r['release_ready'])
        m['approvals'] = {'listening': {'reviewer':'Human','date':'2026-11-17','evidence':'audition-log'},
                          'release': {'reviewer':'Human','date':'2026-11-17','evidence':'release-record'}}
        (self.kit/'manifest.json').write_text(json.dumps(m))
        self.assertTrue(verify(self.kit, self.kit/'manifest.json')['release_ready'])
        m['required_device_profiles']=['mpc500']
        (self.kit/'manifest.json').write_text(json.dumps(m))
        self.assertFalse(verify(self.kit, self.kit/'manifest.json')['release_ready'])

    def test_pitch_reference_pass(self):
        render_kit(self.kit)
        m = json.loads((self.kit/'manifest.json').read_text())
        pitch = inspect_pitch(self.kit/m['samples'][0]['path'], 52)
        self.assertEqual(pitch['status'], 'PASS', pitch)

    def test_pitch_wrong_target_fails(self):
        render_kit(self.kit)
        m = json.loads((self.kit/'manifest.json').read_text())
        m['samples'][0]['pitch_target_hz'] = 90
        (self.kit/'manifest.json').write_text(json.dumps(m))
        result = verify(self.kit,self.kit/'manifest.json')
        self.assertEqual(result['technical_status'],'FAIL')
        self.assertIn('pitch target mismatch',' '.join(result['errors']))

    def test_preflight_blocks_path_traversal(self):
        m = json.loads((self.kit/'manifest.json').read_text())
        m['samples'][0]['path'] = '../escape.wav'
        (self.kit/'manifest.json').write_text(json.dumps(m))
        with self.assertRaises(ValueError):
            render_kit(self.kit)

    def test_64_hit_plan(self):
        target = Path(self.temp.name)/'plan64'
        m = scaffold(target,'TLS',1,'tail-light-static')
        from collections import Counter
        expected = {'808':8,'KICK':8,'SNARE':8,'CLAP':6,'HAT':12,'COWBELL':8,'PERC':8,'FX':6}
        self.assertEqual(dict(Counter(x['kind'] for x in m['samples'])), expected)

    def test_existing_manifest_no_overwrite(self):
        with self.assertRaises(FileExistsError): scaffold(self.kit, 'TLS', 3)


if __name__ == '__main__':
    unittest.main(verbosity=2)
