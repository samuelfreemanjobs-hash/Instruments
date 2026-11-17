"""Read-only local kit browser. Loopback-only; no uploads, no filesystem traversal."""
import argparse
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, unquote
from pathlib import Path
import json
import html

STYLE='''body{font:15px system-ui;background:#10131a;color:#f4f5fa;margin:40px auto;max-width:900px;padding:0 18px}h1{letter-spacing:.04em}p{color:#aab1c2}.sample{border:1px solid #343947;background:#1c202a;border-radius:9px;padding:12px;margin:10px 0;display:flex;align-items:center;gap:14px;flex-wrap:wrap}audio{width:min(100%,310px)}.kind{color:#9dd8f7;font-weight:700}small{color:#bdc6d5}a{color:#abcbff}'''

def make_handler(kit):
    kit=Path(kit).resolve(strict=True)
    m=json.loads((kit/'manifest.json').read_text(encoding='utf8'))
    # Only samples explicitly declared in manifest can be served, irrespective of request URL.
    from verify_kit import safe_relative
    paths={e['path'] for e in m['samples'] if safe_relative(e.get('path')) and e['path'].startswith('masters/')}
    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            url=urlparse(self.path)
            if url.path=='/':
                entries=[]
                for e in m['samples']:
                    path=e.get('path','')
                    if path not in paths:continue
                    from urllib.parse import quote
                    uri='/audio/'+quote(path,safe='/')
                    entries.append('<div class="sample"><span class="kind">'+html.escape(e.get('kind',''))+'</span><small>'+html.escape(e.get('id',''))+'</small><audio controls preload="none" src="'+uri+'"></audio></div>')
                page='<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>DRUMSMITH Browser</title><style>'+STYLE+'</style><h1>DRUMSMITH / '+html.escape(m.get('sku','kit'))+'</h1><p>Read-only audition browser · '+html.escape(m.get('genre','unassigned'))+' · NOT RELEASE APPROVED</p>'+''.join(entries)+'</html>'
                payload=page.encode('utf8')
                self.send_response(200);self.send_header('Content-Type','text/html; charset=utf-8');self.send_header('Content-Length',str(len(payload)));self.send_header('X-Content-Type-Options','nosniff');self.send_header('Content-Security-Policy',"default-src 'none'; style-src 'unsafe-inline'; media-src 'self'");self.end_headers();self.wfile.write(payload)
            elif url.path.startswith('/audio/'):
                rel=unquote(url.path[len('/audio/'):])
                if rel not in paths:
                    self.send_error(404);return
                f=(kit/rel).resolve()
                if not f.is_relative_to(kit) or not f.is_file():
                    self.send_error(404);return
                data=f.read_bytes()
                self.send_response(200);self.send_header('Content-Type','audio/wav');self.send_header('Content-Length',str(len(data)));self.send_header('X-Content-Type-Options','nosniff');self.end_headers();self.wfile.write(data)
            else:self.send_error(404)
        def log_message(self,format,*args):pass
    return Handler

def serve(kit,port=8765):
    if not 1024<=port<=65535: raise ValueError('port must be 1024..65535')
    server=HTTPServer(('127.0.0.1',port),make_handler(kit))
    print(f'Open http://127.0.0.1:{port} to audition. Ctrl+C to stop.',flush=True)
    try:server.serve_forever()
    except KeyboardInterrupt:pass
    finally:server.server_close()
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--kit',required=True);p.add_argument('--port',type=int,default=8765)
    a=p.parse_args();serve(a.kit,a.port)
