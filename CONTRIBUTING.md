# Contributing to cool-s

Thank you for wanting to make the Cool S even cooler!

## Development Setup

```bash
git clone https://github.com/YOUR_USERNAME/cool-s.git
cd cool-s
make
./cool-s
```

## Running Tests

```bash
make check
```

## Code Style

- C99, no external dependencies beyond libc
- Keep the single-file spirit of `src/cool-s.c`
- All terminal output must degrade gracefully with `--plain`

## Submitting Changes

1. Fork the repo
2. Create a feature branch: `git checkout -b my-feature`
3. Commit with a clear message
4. Open a Pull Request

## Ideas for Contributions

- macOS Homebrew formula
- Snap / Flatpak packaging
- `--fps` flag for smoother animation
- Color themes (`--theme matrix`, `--theme fire`, etc.)
- Multiple S's scrolling across the screen (the true `sl` homage)
- Windows native (cmd.exe / PowerShell) support
