"""ZEN-Core Patch Forge desktop UI."""

from __future__ import annotations

import random
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox

import customtkinter as ctk

from synth_forge.bin_diff import compare_bytes, suggest_fields_from_diff
from synth_forge.synths.zenology_svz import ZenologySvzEngine
from synth_forge.zen_forge_presets import PROFILE_LABELS, ZEN_PROFILES, apply_zen_profile, procedural_name

ACCENT = "#FF5500"
BG = "#121212"


class ZenPatchForgeApp(ctk.CTk):
    def __init__(self) -> None:
        super().__init__()
        self.title("ZEN-Core Patch Forge")
        self.geometry("720x640")
        ctk.set_appearance_mode("dark")
        self.configure(fg_color=BG)
        self.engine = ZenologySvzEngine()
        self.profile_var = tk.StringVar(value="memphis_lead")
        self.count_var = tk.IntVar(value=8)
        self.mutation_var = tk.DoubleVar(value=0.15)
        self.export_dir = Path.home() / "zen_forge_exports"
        self._build()

    def _build(self) -> None:
        header = ctk.CTkLabel(self, text="ZEN-Core Patch Forge", font=ctk.CTkFont(size=22, weight="bold"))
        header.pack(pady=(12, 4))
        ctk.CTkLabel(
            self,
            text="Experimental — verify imports in ZENOLOGY Pro",
            text_color=ACCENT,
        ).pack()

        profile_frame = ctk.CTkFrame(self)
        profile_frame.pack(fill="x", padx=16, pady=12)
        for key, label in PROFILE_LABELS.items():
            btn = ctk.CTkRadioButton(
                profile_frame,
                text=label,
                variable=self.profile_var,
                value=key,
                fg_color=ACCENT,
            )
            btn.pack(anchor="w", padx=12, pady=4)

        controls = ctk.CTkFrame(self)
        controls.pack(fill="x", padx=16, pady=8)
        ctk.CTkLabel(controls, text="Batch count").grid(row=0, column=0, sticky="w", padx=8)
        ctk.CTkEntry(controls, textvariable=self.count_var, width=80).grid(row=0, column=1)
        ctk.CTkLabel(controls, text="Mutation %").grid(row=1, column=0, sticky="w", padx=8)
        ctk.CTkSlider(controls, from_=0.05, to=0.5, variable=self.mutation_var, progress_color=ACCENT).grid(
            row=1, column=1, sticky="ew", padx=8
        )
        controls.grid_columnconfigure(1, weight=1)

        ctk.CTkButton(self, text="Forge patches (ZIP bank)", fg_color=ACCENT, command=self._forge).pack(pady=12)
        ctk.CTkButton(self, text="Pick export folder", command=self._pick_dir).pack()

        diff_frame = ctk.CTkFrame(self)
        diff_frame.pack(fill="both", expand=True, padx=16, pady=12)
        ctk.CTkLabel(diff_frame, text="Binary diff inspector").pack(anchor="w", padx=8, pady=4)
        ctk.CTkButton(diff_frame, text="Compare two .svz files", command=self._diff).pack(padx=8, pady=4, anchor="w")
        self.diff_box = ctk.CTkTextbox(diff_frame, height=180)
        self.diff_box.pack(fill="both", expand=True, padx=8, pady=8)

    def _pick_dir(self) -> None:
        chosen = filedialog.askdirectory()
        if chosen:
            self.export_dir = Path(chosen)

    def _load_seed(self) -> bytes | None:
        seed_path = ZenologySvzEngine.default_fixtures_dir() / "seed_init.svz"
        return seed_path.read_bytes() if seed_path.is_file() else None

    def _forge(self) -> None:
        profile = self.profile_var.get()
        if profile not in ZEN_PROFILES:
            messagebox.showerror("Profile", f"Unknown profile {profile}")
            return
        count = max(1, min(64, int(self.count_var.get())))
        rng = random.Random()
        sigma = float(self.mutation_var.get())
        seed = self._load_seed()
        pairs = []
        for i in range(count):
            base = apply_zen_profile(profile, rng)
            data = base.model_dump()
            for k in base.model_fields:
                if k == "category":
                    continue
                data[k] = float(__import__("numpy").clip(data[k] + rng.gauss(0, sigma), 0.0, 1.0))
            from synth_forge.models import SynthParameters
            from synth_forge.safety import clamp_for_hardware

            params = clamp_for_hardware(SynthParameters.model_validate({**data, "category": profile}))
            pairs.append((params, procedural_name(profile, i + 1)))
        payload = self.engine.pack_svz_bank(pairs, seed=seed)
        self.export_dir.mkdir(parents=True, exist_ok=True)
        out = self.export_dir / f"zen_forge_{profile}_{count}.zip"
        out.write_bytes(payload)
        messagebox.showinfo("Exported", f"Wrote {out}")

    def _diff(self) -> None:
        a = filedialog.askopenfilename(filetypes=[("SVZ", "*.svz"), ("All", "*.*")])
        if not a:
            return
        b = filedialog.askopenfilename(filetypes=[("SVZ", "*.svz"), ("All", "*.*")])
        if not b:
            return
        left = Path(a).read_bytes()
        right = Path(b).read_bytes()
        changes = compare_bytes(left, right)
        suggestions = suggest_fields_from_diff(changes)
        lines = [f"{len(changes)} byte changes", ""]
        for s in suggestions[:40]:
            lines.append(f"0x{s.offset:04x} {s.scale} ({s.id})")
        self.diff_box.delete("1.0", "end")
        self.diff_box.insert("1.0", "\n".join(lines))


def main() -> None:
    app = ZenPatchForgeApp()
    app.mainloop()


if __name__ == "__main__":
    main()
