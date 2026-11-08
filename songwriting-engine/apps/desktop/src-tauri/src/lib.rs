use std::{
    env,
    fs,
    path::{Path, PathBuf},
    process::Command,
};
use tauri::{AppHandle, Manager};

fn engine_dir() -> Result<PathBuf, String> {
    if let Some(path) = env::var_os("SONGWRITER_ENGINE_DIR") {
        return Ok(PathBuf::from(path));
    }
    Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../..")
        .canonicalize()
        .map_err(|error| format!("cannot locate the songwriting engine: {error}"))
}

fn run_generator(
    app: &AppHandle,
    output_dir: &Path,
    operation: &[String],
    formats: Vec<u8>,
) -> Result<serde_json::Value, String> {
    if formats.is_empty() || formats.iter().any(|format| *format > 1) {
        return Err("choose MIDI Type 0, Type 1, or both".into());
    }
    let packaged = !cfg!(debug_assertions);
    let mut command = if packaged {
        let executable = app
            .path()
            .resource_dir()
            .map_err(|error| format!("cannot locate packaged engine resources: {error}"))?
            .join("songwriting-engine.exe");
        if !executable.is_file() {
            return Err("packaged songwriting engine is missing from app resources".into());
        }
        Command::new(executable)
    } else {
        let mut command = Command::new(
            env::var_os("SONGWRITER_PYTHON").unwrap_or_else(|| "python".into()),
        );
        command
            .current_dir(engine_dir()?)
            .args(["-m", "songwriting.cli"]);
        command
    };
    command.args(operation).arg("--output-dir").arg(output_dir).arg("--formats");
    for format in formats {
        command.arg(format.to_string());
    }
    let output = command
        .output()
        .map_err(|error| format!("could not start the songwriting engine: {error}. Set SONGWRITER_PYTHON if needed."))?;
    if !output.status.success() {
        return Err(String::from_utf8_lossy(&output.stderr).trim().to_owned());
    }
    serde_json::from_slice(&output.stdout)
        .map_err(|error| format!("engine returned invalid JSON: {error}"))
}

#[tauri::command]
fn generate_song(
    app: AppHandle,
    config: serde_json::Value,
    formats: Vec<u8>,
) -> Result<serde_json::Value, String> {
    let output_dir = app
        .path()
        .app_local_data_dir()
        .map_err(|error| format!("cannot locate the app data directory: {error}"))?
        .join("Generated");
    fs::create_dir_all(&output_dir)
        .map_err(|error| format!("cannot create output directory: {error}"))?;
    let config_path = output_dir.join("active-config.json");
    fs::write(&config_path, serde_json::to_vec_pretty(&config).map_err(|error| error.to_string())?)
        .map_err(|error| format!("cannot save generation settings: {error}"))?;
    run_generator(
        &app,
        &output_dir,
        &["generate".into(), "--config".into(), config_path.to_string_lossy().into_owned()],
        formats,
    )
}

#[tauri::command]
fn generate_song_from_prompt(
    app: AppHandle,
    prompt: String,
    seed: i64,
    formats: Vec<u8>,
) -> Result<serde_json::Value, String> {
    let output_dir = app
        .path()
        .app_local_data_dir()
        .map_err(|error| format!("cannot locate the app data directory: {error}"))?
        .join("Generated");
    fs::create_dir_all(&output_dir)
        .map_err(|error| format!("cannot create output directory: {error}"))?;
    run_generator(
        &app,
        &output_dir,
        &[
            "generate-prompt".into(),
            "--prompt".into(),
            prompt,
            "--seed".into(),
            seed.to_string(),
        ],
        formats,
    )
}

#[tauri::command]
fn reveal_artifact(app: AppHandle, path: String) -> Result<(), String> {
    let output_root = app
        .path()
        .app_local_data_dir()
        .map_err(|error| format!("cannot locate the app data directory: {error}"))?
        .join("Generated")
        .canonicalize()
        .map_err(|error| format!("generated output directory is unavailable: {error}"))?;
    let artifact = PathBuf::from(path)
        .canonicalize()
        .map_err(|error| format!("generated artifact is unavailable: {error}"))?;
    if !artifact.starts_with(&output_root) {
        return Err("only files in the generated output directory can be opened".into());
    }
    #[cfg(target_os = "windows")]
    Command::new("explorer.exe")
        .arg(format!("/select,{}", artifact.display()))
        .spawn()
        .map_err(|error| format!("could not reveal generated file: {error}"))?;
    #[cfg(not(target_os = "windows"))]
    Command::new("open")
        .arg(&artifact)
        .spawn()
        .map_err(|error| format!("could not open generated file: {error}"))?;
    Ok(())
}

pub fn run() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![generate_song, generate_song_from_prompt, reveal_artifact])
        .run(tauri::generate_context!())
        .expect("error while running the songwriting desktop app");
}
