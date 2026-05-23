# cool-s

> *Draw the legendary Cool S in your terminal.*

`cool-s` is a terminal toy in the spirit of [`sl`](https://github.com/mtoyoda/sl) —
it animates the step-by-step construction of the **Cool S** (also called the
Universal S, Super S, Pointy S, or Graffiti S), the graffiti symbol every
90s kid drew on their notebook.
# cool-s

> *Draw the legendary Cool S in your terminal.*

`cool-s` is a terminal toy in the spirit of [`sl`](https://github.com/mtoyoda/sl) --
it animates the step-by-step construction of the **Cool S** (also called the
Universal S, Super S, Pointy S, or Graffiti S), the graffiti symbol every
90s kid drew on their notebook.

```
          /\
        ///  \\\
      ///      \\\
    ///          \\\
   //      ||      \\
   ||      ||      ||
   ||      ||      ||
   ||      \\      ||
   ||\\      \\\   ||
   ||  \\\    \\\ ||
   ||    \\\   \\ ||
   ||      ||      ||
   ||      ||      ||
   \\      ||      //
     \\\          ///
       \\\      ///
         \\\  ///
           \///
```

The Cool S is constructed in 7 stages, each rendered in a distinct color,
with spark particles flying from the tip of each line as it is drawn.

---

## Features

- 7-stage animated construction with color-coded segments
- Spark particle effect at each drawing step
- Rainbow finale mode (`-r`)
- Scalable -- render it tiny or enormous (`-s 1` through `-s 8`)
- Fast mode -- instant render, no animation (`-f`)
- Plain/no-color mode for redirecting or piping output
- Auto-centers in your terminal regardless of window size
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

### From source

```bash
git clone https://github.com/YOUR_USERNAME/cool-s.git
cd cool-s
make
sudo make install          # installs to /usr/local/bin
```

To install to a custom prefix:

```bash
sudo make install PREFIX=/usr
```

### Uninstall

```bash
sudo make uninstall
```

### Manual (no make)

```bash
gcc -O2 -o cool-s src/cool-s.c -lm
```

---

## Usage

```
cool-s [OPTIONS]

Options:
  -f            Fast mode -- skip animation, instant render
  -d USECS      Per-pixel delay in microseconds (default: 25000)
  -s SCALE      Scale factor 1-8 (default: 4)
  -r            Rainbow finale mode
  --no-sparks   Disable spark particles
  --plain       No color output
  -h, --help    Show help and exit
```

### Examples

```bash
cool-s                   # Animated Cool S at default size
cool-s -f -s 6           # Instant, big
cool-s -d 60000 -r       # Slow cinematic with rainbow finale
cool-s -s 2 --plain      # Small, monochrome
cool-s --no-sparks       # No particles
```

---

## How the Cool S is Drawn

The Cool S consists of **14 line segments** in **7 steps**, following the
canonical construction described on [Wikipedia](https://en.wikipedia.org/wiki/Cool_S):

| Stage | What is drawn                      | Color   |
|------:|------------------------------------|---------|
|     1 | Three top vertical bars            | White   |
|     2 | Three bottom vertical bars         | White   |
|     3 | Two diagonal connectors            | Cyan    |
|     4 | Top inverted-V cap                 | Yellow  |
|     5 | Bottom V base                      | Green   |
|     6 | Left closing diagonal              | Magenta |
|     7 | Right closing diagonal             | Red     |

---

## Platform Support

- **Linux** -- full support
- **macOS** -- full support (tested on macOS 13+)
- **WSL / Windows Terminal** -- works great
- **Other POSIX** -- should work; needs a VT100-compatible terminal

---

## Background

The Cool S is a piece of [childlore](https://en.wikipedia.org/wiki/Childlore)
whose origin is unknown but dates to at least the early 1980s. It is one of
the most universally recognized hand-drawn symbols in the world -- yet no one
knows exactly where it came from.

`cool-s` was written as a tribute to that tiny, pointy act of creativity.

---

## Contributing

Bug reports and pull requests are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

```bash
make check   # run smoke tests
```

---

## License

MIT -- see [LICENSE](LICENSE).

```
          /\
        ///  \\\
      ///      \\\
    ///          \\\
   //      ||      \\
   ||      ||      ||
   ||      ||      ||
   ||      \\      ||
   ||\\      \\\   ||
   ||  \\\    \\\ ||
   ||    \\\   \\ ||
   ||      ||      ||
   ||      ||      ||
   \\      ||      //
     \\\          ///
       \\\      ///
         \\\  ///
           \///
```

The Cool S is constructed in 7 stages, each rendered in a distinct color,
with spark particles flying from the tip of each line as it is drawn.

---

## Features

- 7-stage animated construction with color-coded segments
- Spark particle effect at each drawing step
- Rainbow finale mode (`-r`)
- Scalable — render it tiny or enormous (`-s 1` through `-s 8`)
- Fast mode — instant render, no animation (`-f`)
- Plain/no-color mode for redirecting or piping output
- Auto-centers in your terminal regardless of window size
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

### From source

```bash
git clone https://github.com/YOUR_USERNAME/cool-s.git
cd cool-s
make
sudo make install          # installs to /usr/local/bin
```

To install to a custom prefix:

```bash
sudo make install PREFIX=/usr
```

### Uninstall

```bash
sudo make uninstall
```

### Manual (no make)

```bash
gcc -O2 -o cool-s src/cool-s.c -lm
```

---

## Usage

```
cool-s [OPTIONS]

Options:
  -f            Fast mode -- skip animation, instant render
  -d USECS      Per-pixel delay in microseconds (default: 25000)
  -s SCALE      Scale factor 1-8 (default: 4)
  -r            Rainbow finale mode
  --no-sparks   Disable spark particles
  --plain       No color output
  -h, --help    Show help and exit
```

### Examples

```bash
cool-s                   # Animated Cool S at default size
cool-s -f -s 6           # Instant, big
cool-s -d 60000 -r       # Slow cinematic with rainbow finale
cool-s -s 2 --plain      # Small, monochrome
cool-s --no-sparks       # No particles
```

---

## How the Cool S is Drawn

The Cool S consists of **14 line segments** in **7 steps**, following the
canonical construction described on [Wikipedia](https://en.wikipedia.org/wiki/Cool_S):

| Stage | What is drawn              | Color   |
|------:|----------------------------|---------|
|     1 | Three top vertical bars    | White   |
|     2 | Three bottom vertical bars | White   |
|     3 | Two diagonal connectors    | Cyan    |
|     4 | Top inverted-V cap         | Yellow  |
|     5 | Bottom V base              | Green   |
|     6 | Left closing connector     | Magenta |
|     7 | Right closing connector    | Red     |

---

## Platform Support

- **Linux** -- full support
- **macOS** -- full support (tested on macOS 13+)
- **WSL / Windows Terminal** -- works great
- **Other POSIX** -- should work; needs a VT100-compatible terminal

---

## Background

The Cool S is a piece of [childlore](https://en.wikipedia.org/wiki/Childlore)
whose origin is unknown but dates to at least the early 1980s. It is one of
the most universally recognized hand-drawn symbols in the world -- yet no one
knows exactly where it came from.

`cool-s` was written as a tribute to that tiny, pointy act of creativity.

---

## Contributing

Bug reports and pull requests are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

```bash
make check   # run smoke tests
```

---

## License

MIT -- see [LICENSE](LICENSE).
