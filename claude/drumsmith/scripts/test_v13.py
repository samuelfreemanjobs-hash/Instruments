import json
import sys
import tempfile
import unittest
from pathlib import Path
from urllib.request import urlopen
from threading import Thread
from http.server import HTTPServer
sys.path.insert(0,str(Path(__file__).parent))
from genre_profiles import PROFILES,genre_recipe
from recipe_engine import recipe_hash
from factory import run
from build_genre_kit import build
from kit_browser import make_handler

class V13Tests(unittest.TestCase):
    def test_profiles_valid(self):
        for genre in PROFILES:
            for kind in ('808','KICK','SNARE','CLAP','HAT','COWBELL','PERC','FX'):
                self.assertTrue(recipe_hash(genre_recipe(kind,0,genre)))
    def test_deterministic(self):
        for genre in PROFILES:
            self.assertEqual(genre_recipe('808',3,genre),genre_recipe('808',3,genre))
    def test_distinct_genres(self):
        self.assertNotEqual(recipe_hash(genre_recipe('KICK',0,'drift-phonk')),recipe_hash(genre_recipe('KICK',0,'cloud-rap')))
    def test_unknown_genre(self):
        with self.assertRaises(ValueError):genre_recipe('KICK',0,'other')
    def test_full_factory_and_browser_security(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/'new'
            result=run(p,'memphis-tape',count=8)
            self.assertEqual(result['technical_qa'],'PASS')
            self.assertFalse(result['release_ready'])
            self.assertEqual(result['samples'],8)
            m=json.loads((p/'manifest.json').read_text())
            self.assertEqual(m['genre'],'memphis-tape')
            self.assertTrue((p/'forge-handoff.json').exists())
            with self.assertRaises(FileExistsError):run(p,'memphis-tape',count=8)
            server=HTTPServer(('127.0.0.1',0),make_handler(p))
            t=Thread(target=server.serve_forever,daemon=True);t.start()
            try:
                base='http://127.0.0.1:'+str(server.server_port)
                with urlopen(base+'/') as resp:
                    body=resp.read().decode()
                self.assertIn('memphis-tape',body)
                self.assertIn('<audio',body)
                with urlopen(base+'/audio/'+m['samples'][0]['path']) as resp:
                    self.assertEqual(resp.read(4),b'RIFF')
                from urllib.error import HTTPError
                for bad in ('/audio/../manifest.json','/audio/masters/not-listed.wav'):
                    with self.assertRaises(HTTPError):urlopen(base+bad)
            finally: server.shutdown();server.server_close();t.join()
    def test_no_manifest_overwrite(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/'kit';build(p,'detroit-trap',count=2)
            with self.assertRaises(FileExistsError):build(p,'detroit-trap',count=2)

if __name__=='__main__':unittest.main(verbosity=2)
