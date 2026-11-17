import json
from pathlib import Path
import sys
import tempfile
import unittest
sys.path.insert(0,str(Path(__file__).parent))
from build_recipe_kit import build
from render_recipes import render_kit
from recipe_engine import recipe_for,render_recipe,recipe_hash
from diversity_audit import audit
from export_forge import export
from verify_kit import verify

class UpgradeTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.kit=Path(self.tmp.name)/'kit'
        build(self.kit,count=8,plan=None)
    def tearDown(self):self.tmp.cleanup()
    def test_deterministic(self):
        r=recipe_for('808',0)
        self.assertEqual(render_recipe(r,8000,4),render_recipe(r,8000,4))
    def test_invalid_recipe(self):
        r=recipe_for('808');r['drive']=float('nan')
        with self.assertRaises(ValueError):render_recipe(r,8000,5)
    def test_render_and_verify(self):
        self.assertEqual(render_kit(self.kit),8)
        result=verify(self.kit,self.kit/'manifest.json')
        self.assertEqual(result['technical_status'],'PASS',result['errors'])
    def test_no_overwrite(self):
        render_kit(self.kit)
        with self.assertRaises(FileExistsError):render_kit(self.kit)
    def test_malicious_recipe_preflight(self):
        m=json.loads((self.kit/'manifest.json').read_text())
        m['samples'][-1]['recipe_params']['drive']=999
        (self.kit/'manifest.json').write_text(json.dumps(m))
        with self.assertRaises(ValueError):render_kit(self.kit)
        self.assertEqual(list((self.kit/'masters').glob('*.wav')),[])
    def test_recipe_tamper(self):
        m=json.loads((self.kit/'manifest.json').read_text())
        m['samples'][0]['recipe_params']['drive']=2.4
        (self.kit/'manifest.json').write_text(json.dumps(m))
        with self.assertRaises(ValueError):render_kit(self.kit)
    def test_diversity(self):
        render_kit(self.kit)
        self.assertEqual(audit(self.kit)['status'],'PASS')
    def test_duplicate_detection(self):
        render_kit(self.kit)
        m=json.loads((self.kit/'manifest.json').read_text())
        p=self.kit/m['samples'][0]['path'];q=self.kit/m['samples'][1]['path']
        q.write_bytes(p.read_bytes())
        self.assertEqual(audit(self.kit)['status'],'FAIL')
    def test_handoff(self):
        render_kit(self.kit)
        p=Path(self.tmp.name)/'forge.json'
        self.assertEqual(export(self.kit,p),8)
        self.assertEqual(json.loads(p.read_text())['status'],'SPECIFICATION_ONLY_NOT_COMPILED')
        with self.assertRaises(FileExistsError):export(self.kit,p)
    def test_distinct_recipe_params(self):
        self.assertNotEqual(recipe_hash(recipe_for('HAT',0)),recipe_hash(recipe_for('HAT',1)))
if __name__=='__main__':unittest.main(verbosity=2)
