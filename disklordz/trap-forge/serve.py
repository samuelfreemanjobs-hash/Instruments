#!/usr/bin/env python3
"""Static TRAP-FORGE + Gemini proxy (avoids browser CORS on AI Vibe Forge)."""

from __future__ import annotations

import json
import os
import urllib.error
import urllib.request
from pathlib import Path

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field

ROOT = Path(__file__).resolve().parent
app = FastAPI(title="MPC TRAP-FORGE")
app.add_middleware(CORSMiddleware, allow_origins=["*"], allow_methods=["*"], allow_headers=["*"])


class VibeRequest(BaseModel):
    prompt: str = Field(..., min_length=3, max_length=500)
    drumId: str = Field(..., max_length=32)
    model: str = Field(default="gemini-2.0-flash", max_length=64)
    schema: str = Field(default="studio", max_length=32)


MPC_ELITE_SYSTEM = """You are a legendary Trap Drum DSP Engineer for Akai MPC hardware.
Convert the user's natural language drum aesthetic into precise procedural DSP parameters in JSON.
The output JSON schema MUST match:
{
  "name": "SHORT_UPPERCASE_NAME",
  "cat": "808" | "KICK" | "SNARE" | "HIHAT" | "PERC",
  "desc": "Short description",
  "amp": { "attack": number, "decay": number, "sustain": number, "release": number },
  "filter": { "type": "lowpass"|"highpass"|"bandpass"|"notch", "cutoff": number, "reso": number, "env": number, "decay": number },
  "transient": { "attack": number, "sustain": number, "pitchStart": number, "pitchDecay": number, "baseFreq": number },
  "tx": { "drive": number, "type": "hard"|"tape"|"tube"|"fold", "subHarm": number, "noise": number, "ceiling": number }
}"""


@app.post("/api/ai/vibe")
def ai_vibe(body: VibeRequest):
    key = os.environ.get("GEMINI_API_KEY") or os.environ.get("GOOGLE_API_KEY")
    if not key:
        raise HTTPException(status_code=503, detail="Set GEMINI_API_KEY to enable AI Vibe Forge")
    url = f"https://generativelanguage.googleapis.com/v1beta/models/{body.model}:generateContent?key={key}"
    if body.schema == "mpc_elite":
        user_text = f'Design an elite trap drum for drum category "{body.drumId}" with this vibe: {body.prompt}'
        payload = {
            "contents": [{"role": "user", "parts": [{"text": user_text}]}],
            "systemInstruction": {"parts": [{"text": MPC_ELITE_SYSTEM}]},
            "generationConfig": {"temperature": 0.35, "responseMimeType": "application/json"},
        }
    else:
        payload = {
            "contents": [
                {
                    "role": "user",
                    "parts": [
                        {
                            "text": (
                                f"Trap drum sound designer. Drum: {body.drumId}. Vibe: {body.prompt}. "
                                "Return JSON only with DSP params: rootHz, pitchMod, drive, fCut, transAttackDb, "
                                "transSustainDb, distType (tape|tube|foldback|spinz|fl_clip), snap, reverbMix, bits, lofiSr."
                            )
                        }
                    ],
                }
            ],
            "generationConfig": {"temperature": 0.35, "responseMimeType": "application/json"},
        }
    req = urllib.request.Request(
        url, data=json.dumps(payload).encode("utf-8"), headers={"Content-Type": "application/json"}, method="POST"
    )
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            data = json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        raise HTTPException(status_code=exc.code, detail="Gemini request failed") from exc
    text = data.get("candidates", [{}])[0].get("content", {}).get("parts", [{}])[0].get("text", "{}")
    try:
        parameters = json.loads(text)
    except json.JSONDecodeError:
        parameters = {}
    if body.schema == "mpc_elite":
        return {"sound": parameters, "parameters": parameters}
    return {"parameters": parameters}


app.mount("/", StaticFiles(directory=str(ROOT), html=True), name="static")


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="127.0.0.1", port=8765)
