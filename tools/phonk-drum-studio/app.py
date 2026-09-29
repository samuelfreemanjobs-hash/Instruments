#!/usr/bin/env python3
"""Phonk Drum Studio — desktop GUI for training and generating phonk drum samples."""

from __future__ import annotations

import logging
import sys
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox
from typing import Optional

import customtkinter as ctk
import numpy as np
import soundfile as sf
import torch

from dataset import load_and_preprocess
from model import AUDIO_LENGTH, LATENT_DIM, SAMPLE_RATE, peak_normalize
from trainer import (
    DEFAULT_CHECKPOINT,
    TrainConfig,
    TrainingWorker,
    device_label,
    load_model_from_checkpoint,
    select_device,
)

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(name)s: %(message)s")
logger = logging.getLogger("phonk_drum_studio")

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class PhonkDrumStudioApp(ctk.CTk):
    def __init__(self) -> None:
        super().__init__()
        self.title("Phonk Drum Studio")
        self.geometry("980x720")
        self.minsize(880, 640)

        self.dataset_path: Optional[Path] = None
        self.file_count = 0
        self.model: Optional[torch.nn.Module] = None
        self.model_device = select_device()
        self.checkpoint_path = Path(DEFAULT_CHECKPOINT)

        self._sample_lock = threading.Lock()
        self._current_sample: Optional[np.ndarray] = None

        self._trainer: Optional[TrainingWorker] = None
        self._mutation_variance = tk.DoubleVar(value=0.8)

        self._build_ui()
        self._show_device_notice()
        self._try_load_default_checkpoint()

    def _build_ui(self) -> None:
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)

        container = ctk.CTkScrollableFrame(self)
        container.grid(row=0, column=0, sticky="nsew", padx=16, pady=16)
        container.grid_columnconfigure(0, weight=1)

        self._build_dataset_panel(container)
        self._build_generation_panel(container)
        self._build_audition_panel(container)
        self._build_export_panel(container)

        self.status_bar = ctk.CTkLabel(self, text="Ready", anchor="w")
        self.status_bar.grid(row=1, column=0, sticky="ew", padx=16, pady=(0, 8))

    def _section(self, parent: ctk.CTkBaseClass, title: str) -> ctk.CTkFrame:
        frame = ctk.CTkFrame(parent)
        frame.grid(sticky="ew", pady=(0, 12))
        frame.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(frame, text=title, font=ctk.CTkFont(size=16, weight="bold")).grid(
            row=0, column=0, sticky="w", padx=12, pady=(10, 6)
        )
        body = ctk.CTkFrame(frame, fg_color="transparent")
        body.grid(row=1, column=0, sticky="ew", padx=12, pady=(0, 12))
        body.grid_columnconfigure(0, weight=1)
        return body

    def _build_dataset_panel(self, parent: ctk.CTkBaseClass) -> None:
        body = self._section(parent, "Dataset & Training")

        row = ctk.CTkFrame(body, fg_color="transparent")
        row.grid(row=0, column=0, sticky="ew")
        row.grid_columnconfigure(1, weight=1)

        ctk.CTkButton(row, text="Browse Folder", width=140, command=self._browse_folder).grid(
            row=0, column=0, padx=(0, 8)
        )
        self.folder_label = ctk.CTkLabel(row, text="No folder selected", anchor="w")
        self.folder_label.grid(row=0, column=1, sticky="ew")

        btn_row = ctk.CTkFrame(body, fg_color="transparent")
        btn_row.grid(row=1, column=0, sticky="ew", pady=(8, 0))
        self.train_btn = ctk.CTkButton(btn_row, text="Train Model", command=self._start_training)
        self.train_btn.grid(row=0, column=0, padx=(0, 8))
        self.stop_btn = ctk.CTkButton(
            btn_row, text="Stop", fg_color="#8b2942", hover_color="#6b1f33", command=self._stop_training
        )
        self.stop_btn.grid(row=0, column=1)
        self.stop_btn.configure(state="disabled")

        self.progress = ctk.CTkProgressBar(body)
        self.progress.grid(row=2, column=0, sticky="ew", pady=(12, 4))
        self.progress.set(0)

        self.train_status = ctk.CTkLabel(body, text="Epoch — / —  |  Loss —", anchor="w")
        self.train_status.grid(row=3, column=0, sticky="ew")

    def _build_generation_panel(self, parent: ctk.CTkBaseClass) -> None:
        body = self._section(parent, "Generation & Variation")

        slider_row = ctk.CTkFrame(body, fg_color="transparent")
        slider_row.grid(row=0, column=0, sticky="ew")
        slider_row.grid_columnconfigure(1, weight=1)
        ctk.CTkLabel(slider_row, text="Mutation Variance").grid(row=0, column=0, padx=(0, 8))
        self.variance_slider = ctk.CTkSlider(
            slider_row,
            from_=0.1,
            to=2.5,
            number_of_steps=240,
            variable=self._mutation_variance,
            command=self._on_variance_changed,
        )
        self.variance_slider.grid(row=0, column=1, sticky="ew")
        self.variance_value_label = ctk.CTkLabel(slider_row, text="0.80", width=48)
        self.variance_value_label.grid(row=0, column=2, padx=(8, 0))

        gen_row = ctk.CTkFrame(body, fg_color="transparent")
        gen_row.grid(row=1, column=0, sticky="ew", pady=(10, 0))
        ctk.CTkButton(gen_row, text="Generate New Sample", command=self._generate_sample).grid(
            row=0, column=0, padx=(0, 8)
        )
        ctk.CTkButton(gen_row, text="Mutate Sample", command=self._mutate_sample).grid(row=0, column=1)

    def _build_audition_panel(self, parent: ctk.CTkBaseClass) -> None:
        body = self._section(parent, "Audition & Waveform Preview")

        ctk.CTkButton(body, text="Play / Audition", command=self._play_sample).grid(
            row=0, column=0, sticky="w", pady=(0, 8)
        )

        self.wave_canvas = tk.Canvas(
            body,
            height=140,
            bg="#1a1a1a",
            highlightthickness=1,
            highlightbackground="#333333",
        )
        self.wave_canvas.grid(row=1, column=0, sticky="ew")
        self._draw_placeholder_waveform()

    def _build_export_panel(self, parent: ctk.CTkBaseClass) -> None:
        body = self._section(parent, "Export")
        ctk.CTkButton(body, text="Export to WAV", command=self._export_wav).grid(row=0, column=0, sticky="w")

    def _set_status(self, text: str) -> None:
        self.status_bar.configure(text=text)

    def _on_variance_changed(self, _value: float) -> None:
        self.variance_value_label.configure(text=f"{self._mutation_variance.get():.2f}")

    def _show_device_notice(self) -> None:
        label = device_label(self.model_device)
        self._set_status(f"Compute device: {label}")
        if self.model_device.type == "cpu":
            messagebox.showwarning(
                "CUDA / GPU",
                "CUDA and MPS are not available. Training and inference will run on CPU, "
                "which is slower but fully supported.",
            )

    def _try_load_default_checkpoint(self) -> None:
        if self.checkpoint_path.is_file():
            threading.Thread(target=self._load_checkpoint_thread, args=(self.checkpoint_path,), daemon=True).start()

    def _browse_folder(self) -> None:
        directory = filedialog.askdirectory(title="Select phonk drum sample folder")
        if not directory:
            return
        from dataset import list_audio_files

        self.dataset_path = Path(directory)
        paths = list_audio_files(self.dataset_path)
        self.file_count = len(paths)
        self.folder_label.configure(text=f"{self.dataset_path}  ({self.file_count} files)")
        if self.file_count == 0:
            messagebox.showwarning("Empty folder", "No .wav or .aif files were found in that folder.")

    def _require_model(self) -> bool:
        if self.model is None:
            messagebox.showerror(
                "No model loaded",
                "Train a model or place phonk_vae.pth in the application directory first.",
            )
            return False
        return True

    def _start_training(self) -> None:
        if self._trainer and self._trainer.is_running:
            return
        if self.dataset_path is None or self.file_count == 0:
            messagebox.showwarning("Dataset", "Choose a folder with at least one audio file before training.")
            return

        ckpt = self.dataset_path / DEFAULT_CHECKPOINT
        self.checkpoint_path = ckpt
        config = TrainConfig(
            data_dir=self.dataset_path,
            epochs=100,
            batch_size=16,
            checkpoint_path=ckpt,
        )

        self.train_btn.configure(state="disabled")
        self.stop_btn.configure(state="normal")
        self.progress.set(0)
        self.train_status.configure(text="Starting training…")

        self._trainer = TrainingWorker(
            config,
            progress_callback=self._on_train_progress,
            complete_callback=self._on_train_complete,
            error_callback=self._on_train_error,
        )
        self._trainer.start()

    def _stop_training(self) -> None:
        if self._trainer:
            self._trainer.request_stop()
            self._set_status("Stopping training…")

    def _on_train_progress(self, epoch: int, total: int, loss: float) -> None:
        def ui() -> None:
            self.progress.set(epoch / max(total, 1))
            self.train_status.configure(text=f"Epoch {epoch} / {total}  |  Loss {loss:.6f}")
            self._set_status(f"Training epoch {epoch}/{total}")

        self.after(0, ui)

    def _on_train_complete(self, path: Path) -> None:
        def ui() -> None:
            self.train_btn.configure(state="normal")
            self.stop_btn.configure(state="disabled")
            self.progress.set(1.0)
            self.train_status.configure(text="Training complete")
            self._set_status(f"Saved model to {path}")
            threading.Thread(target=self._load_checkpoint_thread, args=(path,), daemon=True).start()

        self.after(0, ui)

    def _on_train_error(self, message: str) -> None:
        def ui() -> None:
            self.train_btn.configure(state="normal")
            self.stop_btn.configure(state="disabled")
            messagebox.showerror("Training error", message)
            self._set_status(message)

        self.after(0, ui)

    def _load_checkpoint_thread(self, path: Path) -> None:
        try:
            model = load_model_from_checkpoint(path, self.model_device)
            self.model = model
            self.checkpoint_path = path

            def ui() -> None:
                self._set_status(f"Model loaded from {path.name}")

            self.after(0, ui)
        except Exception as exc:
            self.after(0, lambda: messagebox.showerror("Load error", str(exc)))

    def _generate_sample(self) -> None:
        if not self._require_model():
            return
        threading.Thread(target=self._generate_worker, daemon=True).start()

    def _generate_worker(self) -> None:
        try:
            assert self.model is not None
            device = self.model_device
            with torch.no_grad():
                z = torch.randn(1, LATENT_DIM, device=device)
                out = self.model.decode(z)
                out = peak_normalize(out)
            audio = out.squeeze().cpu().numpy()
            self._store_sample(audio)
            self.after(0, lambda: self._set_status("Generated new sample"))
        except Exception as exc:
            self.after(0, lambda: messagebox.showerror("Generation error", str(exc)))

    def _mutate_sample(self) -> None:
        if not self._require_model():
            return
        path = filedialog.askopenfilename(
            title="Select phonk sample to mutate",
            filetypes=[("Audio", "*.wav *.aif *.aiff"), ("All files", "*.*")],
        )
        if not path:
            return
        threading.Thread(target=self._mutate_worker, args=(Path(path),), daemon=True).start()

    def _mutate_worker(self, path: Path) -> None:
        try:
            assert self.model is not None
            wave = load_and_preprocess(path)
            if wave is None:
                self.after(0, lambda: messagebox.showerror("Mutate error", f"Could not read {path.name}"))
                return
            device = self.model_device
            variance = float(self._mutation_variance.get())
            with torch.no_grad():
                x = wave.unsqueeze(0).unsqueeze(0).to(device)
                mu, _ = self.model.encode(x)
                noise = torch.randn_like(mu) * variance
                z = mu + noise
                out = self.model.decode(z)
                out = peak_normalize(out)
            audio = out.squeeze().cpu().numpy()
            self._store_sample(audio)
            self.after(0, lambda: self._set_status(f"Mutated {path.name}"))
        except Exception as exc:
            self.after(0, lambda: messagebox.showerror("Mutate error", str(exc)))

    def _store_sample(self, audio: np.ndarray) -> None:
        with self._sample_lock:
            self._current_sample = np.asarray(audio, dtype=np.float32)
        self.after(0, self._draw_waveform)

    def _get_sample(self) -> Optional[np.ndarray]:
        with self._sample_lock:
            if self._current_sample is None:
                return None
            return self._current_sample.copy()

    def _draw_placeholder_waveform(self) -> None:
        self.wave_canvas.delete("all")
        w = max(self.wave_canvas.winfo_width(), 400)
        h = 140
        self.wave_canvas.create_line(0, h // 2, w, h // 2, fill="#444444")

    def _draw_waveform(self) -> None:
        sample = self._get_sample()
        self.wave_canvas.delete("all")
        w = max(self.wave_canvas.winfo_width(), 400)
        h = 140
        mid = h // 2
        if sample is None or sample.size == 0:
            self._draw_placeholder_waveform()
            return
        step = max(1, len(sample) // w)
        pts = []
        for i in range(w):
            start = i * step
            chunk = sample[start : start + step]
            if chunk.size == 0:
                val = 0.0
            else:
                val = float(chunk.max()) if chunk.max() >= abs(chunk.min()) else float(chunk.min())
            y = mid - int(val * (mid - 4))
            pts.extend([i, y])
        if len(pts) >= 4:
            self.wave_canvas.create_line(*pts, fill="#3b8ed0")
        self.wave_canvas.create_line(0, mid, w, mid, fill="#333333")

    def _play_sample(self) -> None:
        sample = self._get_sample()
        if sample is None:
            messagebox.showwarning("Audition", "Generate or mutate a sample first.")
            return
        threading.Thread(target=self._play_worker, args=(sample,), daemon=True).start()

    def _play_worker(self, sample: np.ndarray) -> None:
        try:
            import sounddevice as sd

            sd.play(sample, SAMPLE_RATE, blocking=True)
        except Exception as exc:
            self.after(0, lambda: messagebox.showerror("Playback error", str(exc)))

    def _export_wav(self) -> None:
        sample = self._get_sample()
        if sample is None:
            messagebox.showwarning("Export", "Generate or mutate a sample before exporting.")
            return
        path = filedialog.asksaveasfilename(
            title="Export WAV",
            defaultextension=".wav",
            filetypes=[
                ("32-bit float WAV", "*.wav"),
                ("16-bit PCM WAV", "*.wav"),
            ],
        )
        if not path:
            return
        use_float = messagebox.askyesno(
            "Bit depth",
            "Use 32-bit float WAV?\n\nChoose No for 16-bit PCM.",
        )
        threading.Thread(
            target=self._export_worker,
            args=(path, sample, use_float),
            daemon=True,
        ).start()

    def _export_worker(self, path: str, sample: np.ndarray, use_float: bool) -> None:
        try:
            subtype = "FLOAT" if use_float else "PCM_16"
            sf.write(path, sample, SAMPLE_RATE, subtype=subtype)
            self.after(0, lambda: self._set_status(f"Exported {Path(path).name}"))
        except Exception as exc:
            self.after(0, lambda: messagebox.showerror("Export error", str(exc)))


def main() -> None:
    app = PhonkDrumStudioApp()
    app.mainloop()


if __name__ == "__main__":
    main()
