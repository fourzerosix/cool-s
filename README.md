# cool-s

> *Draw the legendary Cool S in your terminal.*

`cool-s` is a terminal toy in the spirit of [`sl`](https://github.com/mtoyoda/sl) --
it animates the step-by-step construction of the **Cool S** (also known as the
Universal S, Super S, Pointy S, or Graffiti S): the graffiti symbol every
90s kid drew on their notebook.

```
          /\
        //  \\
      //      \\
    //          \\
  //      ||      \\
  ||      ||      ||
  ||      ||      ||
  ||      ||      ||
  \\      \\      ||
    \\      \\   /
      \\      //
    /   \\      //
  ||      \\      //
  ||      ||      ||
  ||      ||      ||
  ||      ||      ||
  \\      ||      //
    \\          //
      \\      //
        \\  //
          //
```

The Cool S is constructed in **7 stages**, each in a distinct color, with
spark particles flying from the tip of each stroke as it is drawn.

---

## Features

- 7-stage animated construction with per-stage colors
- Spark particle effects during drawing
- `-r` rainbow finale
- `-o` / `--oppenheimer` -- detonate the S in an atomic explosion at the end
- `-s 1-8` scale factor (auto-reduces if the S is too big for your terminal)
- `-f` fast mode (instant render)
- `--plain` colorless output
- Zero dependencies beyond libc

---

## Quick Start

```bash
git clone https://github.com/YOUR_USERNAME/cool-s.git
cd cool-s
make
./cool-s
```

---

## Installation

```bash
make
sudo make install          # installs to /usr/local/bin
sudo make uninstall        # removes it
```

Custom prefix:

```bash
sudo make install PREFIX=/usr
```

Manual (no make):

```bash
gcc -O2 -o cool-s src/cool-s.c -lm
```

---

## Usage

```
cool-s [OPTIONS]

Options:
  -f                 Fast mode -- skip animation, instant render
  -d USECS           Per-pixel delay in microseconds (default: 25000)
  -s SCALE           Scale factor 1-8 (default: 3, auto-fits terminal)
  -r                 Rainbow finale
  -o, --oppenheimer  Detonate the S at the end
  --no-sparks        Disable spark particles
  --plain            No color output
  -h, --help         Show help and exit
```

### Examples

```bash
cool-s                    # Animated Cool S
cool-s -f -s 5            # Instant, large
cool-s -o                 # Now I am become death
cool-s -d 60000 -r        # Slow with rainbow
cool-s -s 2 --plain       # Small, monochrome
```

---

## How the Cool S is Drawn

14 line segments across 7 steps:

| Stage | What is drawn                  | Color   |
|------:|--------------------------------|---------|
|     1 | Three top vertical bars        | White   |
|     2 | Three bottom vertical bars     | White   |
|     3 | Two diagonal connectors        | Cyan    |
|     4 | Top inverted-V cap             | Yellow  |
|     5 | Bottom V base                  | Green   |
|     6 | Left closing diagonal          | Magenta |
|     7 | Right closing diagonal         | Red     |

---

## The Oppenheimer Flag

`cool-s -o`

After the S finishes drawing, it detonates: a screen-flash, a shockwave ring,
and all the pixels scatter outward as debris before fading to ash.

*"Now I am become death, the destroyer of S's."*

---

## Platform Support

- **Linux** -- full support
- **macOS** -- full support
- **WSL / Windows Terminal** -- works great
- **Other POSIX** -- should work; needs a VT100-compatible terminal

---

## Background

The [Cool S](https://en.wikipedia.org/wiki/Cool_S) is a piece of childlore
whose origin is unknown but dates to at least the early 1980s. It is one of
the most universally recognized hand-drawn symbols in the world -- yet no one
knows exactly where it came from.

`cool-s` was written as a tribute to that tiny, pointy act of creativity.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Run `make check` for smoke tests.

---

## License

MIT -- see [LICENSE](LICENSE).
