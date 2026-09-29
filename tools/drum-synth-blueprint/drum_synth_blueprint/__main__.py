from drum_synth_blueprint.trap_kick_808 import normalize_peak, render_trap_808, render_trap_kick


def main() -> None:
    k = normalize_peak(render_trap_kick())
    b = normalize_peak(render_trap_808())
    print("drum-synth-blueprint: ok", len(k), len(b))


if __name__ == "__main__":
    main()
