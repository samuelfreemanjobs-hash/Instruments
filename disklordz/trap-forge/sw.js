const CACHE = "trap-forge-v1";
const ASSETS = [
  "./",
  "./index.html",
  "./manifest.webmanifest",
  "./js/studio-app.js",
  "./js/dsp-core.js",
  "./js/synth-trap.js",
  "./js/trap-presets.js",
  "./js/hat-roll.js",
  "./js/export-dnd.js",
  "./js/reverb-cardo.js",
  "./js/ai-vibe-forge.js",
  "./js/mpc-pad-grid.js",
];

self.addEventListener("install", (event) => {
  event.waitUntil(caches.open(CACHE).then((c) => c.addAll(ASSETS)).then(() => self.skipWaiting()));
});

self.addEventListener("activate", (event) => {
  event.waitUntil(
    caches.keys().then((keys) => Promise.all(keys.filter((k) => k !== CACHE).map((k) => caches.delete(k)))).then(() => self.clients.claim())
  );
});

self.addEventListener("fetch", (event) => {
  if (event.request.method !== "GET") return;
  event.respondWith(
    caches.match(event.request).then((cached) => cached || fetch(event.request).then((res) => {
      const copy = res.clone();
      caches.open(CACHE).then((c) => c.put(event.request, copy));
      return res;
    }).catch(() => cached))
  );
});
