# cool-s
> *The most universally drawn doodle in human history. Now in your terminal.*

`cool-s` draws the legendary **Cool S** stroke by stroke - in glorious ANSI color - *right in your terminal*. Like `sl` but gnarlier, cooler, sicker, and certainly more dope.

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
    /    \\     //
  ||      \\      //
  ||      ||      ||
  ||      ||      ||
  ||      ||      ||
  \\      ||      //
    \\          //
      \\      //
        \\  //
          \/
```

---

## Install in 30 seconds
*Just like the blue-box macaroni that's probably still clogging our arteries. . .while you wait for DOOM to load.*

**From source (Linux / macOS / WSL):**

```bash
git clone https://github.com/fourzerosix/cool-s.git
cd cool-s
make
sudo make install
```

**Via pip (Python wrapper):**

```bash
pip install cool-s
cool-s
```

**Via Docker:**

```bash
docker run --rm -it fourzerosix/cool-s
```

---

## Usage

```
cool-s [OPTIONS]

  -f                 Fast mode - instant render/no animation
  -d USECS           Per-pixel delay in microseconds (default: 40000)
  -s SCALE           Scale factor 1-8 (default: 3, auto-fits your terminal)
  -r                 Rainbow finale (i just want to dance)
  -o, --oppenheimer  Detonate the S at the end
  --no-sparks        Disable spark particles (we should have also probably disabled the drink in the early 2000's) 
  --plain            No color output (you'll enjoy plain or choose your own switch from the willow tree - this hurts me more than it hurts you)
  -h, --help         Show help and exit (go ahead and scream - nobody is listening)
```

```bash
cool-s                    # buy the ticket, take the ride
cool-s -f -s 5            # Instant/Large (MURRRICA MODE)
cool-s -o                 # "Now I am become death, the destroyer of S's."
cool-s -d 60000 -r        # Slow-draw with rainbow finale (like Clint Eastwood doing drag)
cool-s -s 2 --plain       # Small - monochrome - great for pipes (HEY! it's not *that* small . . .)
```

---

## The Lore (a 90s kid deep-dive)

If you grew up in the 80s or 90s, you know the S. You didn't learn it in class. You didn't read it in a book. Some kid - maybe on the bus, maybe at lunch, maybe during a math test you were definitely failing - leaned over and showed you six lines, then connected them. Then added a pointy top and a pointy bottom. Suddenly, you had the coolest thing your pen had ever made.

You covered your Trapper Keeper in them. Your desk. The margins of your Lisa Frank notebook. The back of your hand. Time itself had no meaning while the S was being drawn.

But where did it come from? Nobody knew. Nobody *knows*. That's kind of the thing.

### What even is it?

The Cool S - also known as the Universal S, the Super S, the Pointy S, and the Graffiti S - is a graffiti sign in popular culture and childlore, typically doodled on children's notebooks or graffitied on walls. It consists of exactly 14 line segments forming a stylized, pointed S-shape. It has also been compared to the infinity symbol - the S appears to have depth, where the overlap in the center and the appearance of a potential altitude change at the top and bottom make it look like it connects back to itself, whilst making the world wround you connect in a sort of ethereal way (as you listen the the self-titled Sublime album - love is, it's what we've got - remember that).

Mathematically speaking, the Cool S has no reflection symmetry but has 2-fold rotational symmetry. Spin it 180 degrees and it looks identical (which is deeply satisfying and also completely useless information that you will now think about every time you draw one - see you in your dreams)

### The construction (the sacred recipe)

This is the part that made it viral before "viral" was a word:

1. Draw three short vertical bars - evenly spaced (or it's detention and verbal abuse)
2. Draw three more identical bars directly below - with a matching gap (a gap bigger than yo momma's)
3. Connect them with two diagonal lines - crossing left to right (Allen Iverson style - breakin' ankles)
4. Cap the top with an inverted V (for all the hAters)
5. Cap the bottom with a V (for Vendetta)
6. Add two short closing strokes at the middle junctions (cuz' who don't love closing strokes at the middle junction, ammirite?)

That's it. That's the whole secret. Once you knew it, you couldn't stop. A professor of language and media at Middlesex University put it well when interviewed by VICE:

"*It's probably a Moebius strip - it can't be drawn continuously, but it does have a perpetual flow. Most nine-year-olds can't draw, so when someone hands them a magical recipe to create something fairly cool on demand, that'll go viral.*"

### Origin (O.G.)

The exact origin of the Cool S is unknown, but it became prevalent around the early 1980s as a part of graffiti culture. Some people report seeing it as early as the 1960s. Similar designs for the S have also been identified as early as 1890. One art history professor suggested it may have originated as a "decorated initial" in medieval scripts. So qiute possibly the Cool S may have been invented by medieval monks (NOW, that's what i call RAD).

Jon Naar's photographs of graffiti in New York City, taken in 1973 and published in *The Faith of Graffiti* in 1974, frequently contain the symbol. Jean-Michel Basquiat - one of the most influential artists of the 20th century - occasionally featured it in his artworks, including *Charles the First*, and in *Untitled (Olive Oyl)* it is labelled "CLASSIC S OF GRAFF."

### Childlore (sweet dreams are made of these)

The most compelling explanation isn't really an explanation at all - it's a classification. The Cool S is generally considered to be an artifact of childlore, meaning that it is taught by children to children over the course of generations. It spreads the same way jump-rope rhymes spread (just how your mom's legs spread 9 months before your ugly ass popped out). The same way that thing where you hold someone's wrists and spin them spread. Kid sees it, kid learns it, kid teaches it, repeat.

A 2022 Atlantic article on childlore noted that kids have been passing down games, rhymes, and legends this way since forever - and that the Cool S is one of the purest modern examples of the form. No adult invented it (no such thing as adults). No corporation owns it (*well...*). No one put it in a textbook. It just *exists*, flowing from kid to kid like a slightly more geometric version of cooties or alcoholism.

### The terminal dimension

This is where `cool-s` enters the picture. The S has been drawn on notebooks, walls, desks, arms, textbook covers, bathroom stalls, and the margins of history exams. It has been photographed by documentary photographers in 1973,
painted by Basquiat, worn on streetwear, and apparently trademarked by a former coffee van entrepreneur.

It had never been drawn, stroke by stroke, in 7-stage animated ANSI color with optional spark particles and an atomic bomb finale, in a Linux terminal.

*Until now*.

---

## How it works

### The geometry

The Cool S is 14 line segments across 7 stages:

| Stage | What is drawn                  | Color   |
|------:|--------------------------------|---------|
|     1 | Three top vertical bars        | White   |
|     2 | Three bottom vertical bars     | White   |
|     3 | Two diagonal connectors        | Cyan    |
|     4 | Top inverted-V cap             | Yellow  |
|     5 | Bottom V base                  | Green   |
|     6 | Left closing diagonal          | Magenta |
|     7 | Right closing diagonal         | Red     |

All segments are defined in a square-pixel coordinate grid (x: 0-4, y: 0-10). At render time, each unit is multiplied by `scale`, and the x-coordinate is doubled (`col = sq_x * scale * 2`) to compensate for terminal character aspect
ratio - giving true 45-degree diagonals that actually look like 45 degrees.

### The animation

Each segment is drawn pixel by pixel using Bresenham's line algorithm. Between segments, spark particles emit from the endpoint and drift upward. After all 14 strokes, the shape flashes to solid white. With `-r`, it then cycles through
all 7 stage colors in a rolling rainbow wave before settling.

### The Oppenheimer mode (`-o`)

After the S finishes drawing, there is a brief pause. Then:

1. **Flash** -- rapid white strobe fills the entire screen
2. **Shockwave** -- an expanding ring pulses outward from center
3. **Debris** -- every lit pixel becomes a particle flying away from center with velocity and gravity
4. **Fallout** -- particles drift down and fade to ash

*"Now I am become death, the destroyer of S's."*
-- J. Robert Oppenheimer (paraphrased; he was talking about a different kind of S)

---

## Installation

### From source

```bash
git clone https://github.com/fourzerosix/cool-s.git
cd cool-s
make
sudo make install          # installs to /usr/local/bin
sudo make uninstall        # removes it
```

Custom prefix:

```bash
sudo make install PREFIX=/usr
```

One-liner (no make):

```bash
gcc -O2 -o cool-s src/cool-s.c -lm && sudo cp cool-s /usr/local/bin/
```

### Via pip

```bash
pip install cool-s
cool-s          # runs the binary
```

The pip package bundles the compiled binary for Linux x86-64 and macOS. On other platforms it compiles from source at install time (requires gcc).

### Via Docker

```bash
# Interactive (recommended -- needs a TTY for ANSI animation)
docker run --rm -it fourzerosix/cool-s

# With flags
docker run --rm -it fourzerosix/cool-s cool-s -o

# Fast mode (works without TTY)
docker run --rm fourzerosix/cool-s cool-s -f --plain
```

---

## Platform support

| Platform            | Status        |
|---------------------|---------------|
| Linux (x86-64)      | Full support  |
| macOS (Intel/Apple) | Full support  |
| WSL / Windows Terminal | Full support |
| FreeBSD / other POSIX | Should work |
| Windows (native cmd) | No           |

Requires a VT100-compatible terminal with ANSI color support.
Most modern terminals qualify. If your terminal was made after 1990, you're fine.

---

## Development

```bash
make check      # smoke tests
make dist       # create release tarball
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## Credits & further reading

- [Cool S -- Wikipedia](https://en.wikipedia.org/wiki/Cool_S) -- the canonical reference
- [Mobius Strip](https://en.wikipedia.org/wiki/M%C3%B6bius_strip) -- in mathematics
- [What the Hell Was That 'S' Thing Everyone Drew in School?](https://www.vice.com/en/article/that-s-thing-everyone-drew-in-school-what-is-it/) -- VICE, 2016
- [Meet the Guy Who Just Trademarked 'The S Thing'](https://www.vice.com/en/article/meet-the-guy-who-just-trademarked-the-s-thing-cool-s-pointy-s-stussy-s/) -- VICE, 2020
- [Investigating the Origins of the S, Again](https://www.vice.com/en/article/the-internet-tried-hard-to-solve-the-mystery-of-the-s/) -- VICE, 2017
- [The Faith of Graffiti](https://en.wikipedia.org/wiki/The_Faith_of_Graffiti) -- Norman Mailer & Jon Naar, 1974
- `sl` -- the original terminal toy this was inspired by
