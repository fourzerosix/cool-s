# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.1] - 2026
Geometry rebuilt from scratch - the old approach used a flipped Cartesian coordinate system that accumulated rounding errors across multiple transformations. The new approach uses a simple square-pixel grid (x: 0-4, y: 0-10) where every segment is placed by hand - at render time, col = sq_x × scale × 2 (the ×2 compensates for terminal character aspect ratio so diagonals look like actual 45 degree angles) and row = sq_y × scale. The shape now matches the Wikipedia geometry cleanly - yeehaw.

Auto-fit. If your requested scale would make the S taller or wider than the terminal, it silently steps scale down until it fits - no more clipping.

-o / --oppenheimer flag. Three phases:

    Flash - rapid white strobe fills the screen
    Shockwave - an expanding ring pulses outward while every lit pixel becomes a debris particle flying away from center with physics
    Fallout - remaining particles drift down and fade to ash before the screen clears

"Now I am become death, the destroyer of S's."

## [1.0.0] - 2026

### Added
- Initial release
- Animated 7-stage construction of the Cool S
- ANSI color support with per-stage colors
- Spark particle effects
- `-f` fast mode
- `-d USECS` delay control
- `-s SCALE` scale factor (1–8)
- `-r` rainbow finale mode
- `--no-sparks` flag
- `--plain` flag for colorless output
- Auto-centering in terminal
- Waits for keypress before exiting (interactive mode)
- `make install` / `make uninstall` targets
- Man page (`man cool-s`)
