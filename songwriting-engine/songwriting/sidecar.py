"""PyInstaller entry point for the self-contained Windows engine executable."""

from songwriting.cli import main


if __name__ == "__main__":
    raise SystemExit(main())
