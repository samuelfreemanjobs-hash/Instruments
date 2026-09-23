import { useCallback, useEffect, useMemo, useRef, useState } from "react";
import {
  PARAMETER_SECTIONS,
  ULTRANOVA_PARAMETERS,
  defaultParameterState,
  type ParamDef,
} from "./data/parameters";
import { sendCc, sendNrpn } from "./midi/nrpn";
import {
  buildEditBufferDumpRequest,
  buildProgramDumpRequest,
  guessPatchNameFromSysex,
  isUltraNovaSysex,
  parseSyxFile,
  sysexToHex,
} from "./midi/sysex";
import { findOutputByName, listPorts, requestMidiAccess } from "./midi/webMidi";

type Tab = "edit" | "sysex";

export default function App() {
  const [tab, setTab] = useState<Tab>("edit");
  const [midiReady, setMidiReady] = useState(false);
  const [midiError, setMidiError] = useState<string | null>(null);
  const [outputs, setOutputs] = useState<MIDIOutput[]>([]);
  const [inputs, setInputs] = useState<MIDIInput[]>([]);
  const [outputId, setOutputId] = useState("");
  const [inputId, setInputId] = useState("");
  const [channel, setChannel] = useState(1);
  const [liveSend, setLiveSend] = useState(true);
  const [values, setValues] = useState(defaultParameterState);
  const [sysexLog, setSysexLog] = useState<string>("No SysEx received yet.");
  const [patchName, setPatchName] = useState<string | null>(null);
  const [bank, setBank] = useState(0);
  const [program, setProgram] = useState(0);
  const accessRef = useRef<MIDIAccess | null>(null);

  const selectedOutput = useMemo(
    () => outputs.find((o) => o.id === outputId) ?? outputs[0],
    [outputs, outputId],
  );

  const refreshPorts = useCallback((access: MIDIAccess) => {
    const { inputs: ins, outputs: outs } = listPorts(access);
    setInputs(ins);
    setOutputs(outs);
    if (!outputId && outs.length) {
      const ultra = findOutputByName(outs, "ultranova") ?? findOutputByName(outs, "nova") ?? outs[0];
      setOutputId(ultra.id);
    }
    if (!inputId && ins.length) {
      const ultraIn =
        ins.find((i) => i.name?.toLowerCase().includes("ultranova")) ??
        ins.find((i) => i.name?.toLowerCase().includes("nova")) ??
        ins[0];
      setInputId(ultraIn.id);
    }
  }, [inputId, outputId]);

  useEffect(() => {
    let cancelled = false;
    (async () => {
      try {
        const access = await requestMidiAccess();
        if (cancelled) return;
        accessRef.current = access;
        refreshPorts(access);
        access.onstatechange = () => refreshPorts(access);
        setMidiReady(true);
        setMidiError(null);
      } catch (e) {
        setMidiError(e instanceof Error ? e.message : String(e));
      }
    })();
    return () => {
      cancelled = true;
    };
  }, [refreshPorts]);

  useEffect(() => {
    const input = inputs.find((i) => i.id === inputId);
    if (!input) return;

    const onMessage = (event: MIDIMessageEvent) => {
      const data = event.data;
      if (!data || data[0] !== 0xf0) return;
      const bytes = new Uint8Array(data);
      setSysexLog(sysexToHex(bytes, 512));
      if (isUltraNovaSysex(bytes)) {
        setPatchName(guessPatchNameFromSysex(bytes));
      }
    };

    input.onmidimessage = onMessage;
    return () => {
      input.onmidimessage = null;
    };
  }, [inputs, inputId]);

  const applyParam = useCallback(
    (param: ParamDef, value: number) => {
      setValues((prev) => ({ ...prev, [param.id]: value }));
      if (!liveSend || !selectedOutput) return;
      const ch = channel;
      if (param.nrpnLsb !== undefined) {
        sendNrpn(selectedOutput, ch, param.nrpnMsb ?? 0, param.nrpnLsb, value);
      } else if (param.cc !== undefined) {
        sendCc(selectedOutput, ch, param.cc, value);
      }
    },
    [channel, liveSend, selectedOutput],
  );

  const sendAllParameters = () => {
    if (!selectedOutput) return;
    for (const p of ULTRANOVA_PARAMETERS) {
      const v = values[p.id] ?? p.defaultValue;
      if (p.nrpnLsb !== undefined) sendNrpn(selectedOutput, channel, p.nrpnMsb ?? 0, p.nrpnLsb, v);
      else if (p.cc !== undefined) sendCc(selectedOutput, channel, p.cc, v);
    }
  };

  const sendSysex = (payload: Uint8Array) => {
    if (!selectedOutput) return;
    selectedOutput.send(payload);
  };

  const onSyxFile = async (file: File) => {
    const buf = await file.arrayBuffer();
    const messages = parseSyxFile(buf);
    if (!messages.length) {
      setSysexLog("No SysEx messages found in file.");
      return;
    }
    const first = messages[0];
    setSysexLog(`Loaded ${messages.length} message(s).\n${sysexToHex(first, 512)}`);
    if (isUltraNovaSysex(first)) setPatchName(guessPatchNameFromSysex(first));
  };

  return (
    <div className="app">
      <header>
        <h1>UltraNova Web Editor</h1>
        <p>
          Browser patch editor for Novation UltraNova — live NRPN/CC via Web MIDI, SysEx import/send.
          Connect USB, enable SysEx on the synth, write protect off. Full program binary edit is phased;
          use Novation Librarian for bank management until SysEx decode lands.
        </p>
      </header>

      <section className="panel">
        <h2>MIDI</h2>
        <div className="midi-row">
          <label>
            MIDI output
            <select value={outputId} onChange={(e) => setOutputId(e.target.value)} disabled={!outputs.length}>
              {outputs.map((o) => (
                <option key={o.id} value={o.id}>
                  {o.name ?? o.id}
                </option>
              ))}
            </select>
          </label>
          <label>
            MIDI input (SysEx in)
            <select value={inputId} onChange={(e) => setInputId(e.target.value)} disabled={!inputs.length}>
              {inputs.map((i) => (
                <option key={i.id} value={i.id}>
                  {i.name ?? i.id}
                </option>
              ))}
            </select>
          </label>
          <label>
            Channel (1–16)
            <select value={channel} onChange={(e) => setChannel(Number(e.target.value))}>
              {Array.from({ length: 16 }, (_, i) => i + 1).map((n) => (
                <option key={n} value={n}>
                  {n}
                </option>
              ))}
            </select>
          </label>
          <label style={{ flexDirection: "row", alignItems: "center", gap: "0.5rem", minWidth: "auto" }}>
            <input type="checkbox" checked={liveSend} onChange={(e) => setLiveSend(e.target.checked)} />
            Live send on slider move
          </label>
        </div>
        <p className={`status ${midiReady ? "ok" : "warn"}`} style={{ marginTop: "0.75rem" }}>
          {midiError ?? (midiReady ? "Web MIDI ready (sysex enabled)." : "Requesting MIDI access…")}
        </p>
        {patchName && <p className="status ok">Detected patch name (guess): {patchName}</p>}
      </section>

      <div className="tabs">
        <button type="button" className={`tab ${tab === "edit" ? "active" : ""}`} onClick={() => setTab("edit")}>
          Parameters
        </button>
        <button type="button" className={`tab ${tab === "sysex" ? "active" : ""}`} onClick={() => setTab("sysex")}>
          SysEx / files
        </button>
      </div>

      {tab === "edit" && (
        <>
          <section className="panel">
            <button type="button" className="btn" onClick={sendAllParameters} disabled={!selectedOutput}>
              Send all parameters to synth
            </button>
          </section>
          {PARAMETER_SECTIONS.map((section) => {
            const params = ULTRANOVA_PARAMETERS.filter((p) => p.section === section);
            if (!params.length) return null;
            return (
              <section key={section} className="panel">
                <h2>{section}</h2>
                <div className="param-grid">
                  {params.map((p) => (
                    <div key={p.id} className="param">
                      <label htmlFor={p.id}>{p.label}</label>
                      <output htmlFor={p.id}>{values[p.id]}</output>
                      <input
                        id={p.id}
                        type="range"
                        min={p.min}
                        max={p.max}
                        value={values[p.id]}
                        onChange={(e) => applyParam(p, Number(e.target.value))}
                      />
                    </div>
                  ))}
                </div>
              </section>
            );
          })}
        </>
      )}

      {tab === "sysex" && (
        <section className="panel">
          <h2>SysEx</h2>
          <div className="midi-row" style={{ marginBottom: "0.75rem" }}>
            <label>
              Bank (0-based)
              <input
                type="number"
                min={0}
                max={3}
                value={bank}
                onChange={(e) => setBank(Number(e.target.value))}
                style={{ padding: "0.45rem", borderRadius: 6, border: "1px solid var(--border)", background: "#0c0e12", color: "inherit" }}
              />
            </label>
            <label>
              Program (0-based)
              <input
                type="number"
                min={0}
                max={127}
                value={program}
                onChange={(e) => setProgram(Number(e.target.value))}
                style={{ padding: "0.45rem", borderRadius: 6, border: "1px solid var(--border)", background: "#0c0e12", color: "inherit" }}
              />
            </label>
            <button
              type="button"
              className="btn secondary"
              disabled={!selectedOutput}
              onClick={() => sendSysex(buildProgramDumpRequest(bank, program))}
            >
              Request program dump
            </button>
            <button
              type="button"
              className="btn secondary"
              disabled={!selectedOutput}
              onClick={() => sendSysex(buildEditBufferDumpRequest())}
            >
              Request edit buffer dump
            </button>
          </div>
          <label style={{ display: "block", marginBottom: "0.5rem", fontSize: "0.85rem", color: "var(--muted)" }}>
            Import .syx (preview + optional send first message)
            <input
              type="file"
              accept=".syx,audio/midi"
              style={{ display: "block", marginTop: "0.35rem" }}
              onChange={(e) => {
                const f = e.target.files?.[0];
                if (f) void onSyxFile(f);
              }}
            />
          </label>
          <button
            type="button"
            className="btn"
            style={{ marginBottom: "0.75rem" }}
            disabled={!selectedOutput}
            onClick={async () => {
              const input = document.querySelector<HTMLInputElement>('input[type="file"]');
              const f = input?.files?.[0];
              if (!f) return;
              const messages = parseSyxFile(await f.arrayBuffer());
              if (messages[0]) sendSysex(messages[0]);
            }}
          >
            Send first .syx message to synth
          </button>
          <div className="sysex-log">{sysexLog}</div>
        </section>
      )}
    </div>
  );
}
