#define _GNU_SOURCE
/*
 *
 * cool-s - Draw the legendary Cool S in your terminal
 *
 * Copyright (C) 2026 Jeremy Bell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * "Find what you love and let it kill you. Let it drain you of your all.
 *  Let it cling onto your back and weigh you down into eventual nothingness.
 *  Let it kill you and let it devour your remains. 
 *  For all things will kill you, both slowly and fastly, but it's much better to be killed by a lover."
 *  - Bukowski
 *
 * Inspired by `sl` (steam locomotive).
 * Animates the step-by-step construction of the Cool S.
 *
 * Geometry: all segments are defined in "square-pixel" coordinates
 * where x∈[0,4], y∈[0,10].  At render time each sq-pixel is scaled
 * by `scale`, then x is doubled (×2) to compensate for the ~2:1
 * terminal character aspect ratio, giving clean 45-degree diagonals.
 *
 *   sq-pixel -> terminal:  col = sq_x * scale * 2
 *                          row = sq_y * scale
 *
 * Build:   gcc -O2 -o cool-s src/cool-s.c -lm
 * Usage:   cool-s [-f] [-d USECS] [-s SCALE] [-r] [-o] [--no-sparks] [--plain] [-o] [--oppenheimer] [-h]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>

/* ── ANSI ──────────────────────────────────────────────────────────────────── */
#define CLEAR   "\033[2J"
#define HOME    "\033[H"
#define HIDE    "\033[?25l"
#define SHOW    "\033[?25h"
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

static const char *STAGE_COLOR[] = {
    "\033[97m",   /* 1  top bars      — bright white   */
    "\033[97m",   /* 2  bottom bars   — bright white   */
    "\033[96m",   /* 3  diagonals     — bright cyan    */
    "\033[93m",   /* 4  top V         — bright yellow  */
    "\033[92m",   /* 5  bottom V      — bright green   */
    "\033[95m",   /* 6  close left    — bright magenta */
    "\033[91m",   /* 7  close right   — bright red     */
    "\033[97m",   /* 8  final         — bright white   */
};
#define N_COLORS 8

/* ── Terminal / canvas ─────────────────────────────────────────────────────── */
#define MAX_W 512
#define MAX_H 256

typedef struct { char ch; int stage; } Cell;
static Cell canvas[MAX_H][MAX_W];
static int cw, ch_;   /* actual canvas size = terminal size */

static int g_tcols = 80, g_trows = 24;

static void get_term(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        g_tcols = ws.ws_col;
        g_trows = ws.ws_row;
    }
    cw  = g_tcols < MAX_W ? g_tcols : MAX_W;
    ch_ = g_trows < MAX_H ? g_trows : MAX_H;
}

static void canvas_clear(void) {
    for (int y = 0; y < ch_; y++)
        for (int x = 0; x < cw; x++) {
            canvas[y][x].ch    = ' ';
            canvas[y][x].stage = 0;
        }
}

static void canvas_put(int col, int row, char ch, int stage) {
    if (col < 0 || col >= cw || row < 0 || row >= ch_) return;
    canvas[row][col].ch    = ch;
    canvas[row][col].stage = stage;
}

static void canvas_render(int plain) {
    printf(HOME);
    int last = -1;
    for (int y = 0; y < ch_ - 1; y++) {
        for (int x = 0; x < cw - 1; x++) {
            Cell *c = &canvas[y][x];
            if (!plain && c->stage != last) {
                printf(RESET);
                if (c->stage > 0 && c->stage <= N_COLORS)
                    printf(BOLD "%s", STAGE_COLOR[c->stage - 1]);
                last = c->stage;
            }
            putchar(c->ch);
        }
        putchar('\n');
    }
    printf(RESET);
    fflush(stdout);
}

/* ── Bresenham in sq-pixel space ───────────────────────────────────────────── */
/*
 * x0,y0,x1,y1  are sq-pixel unit coords (e.g. 0..4 × 0..10).
 * scale multiplies them; then col = sq_x*2, row = sq_y.
 * ox, oy are terminal-cell offsets for centering.
 */
static void draw_seg(int x0, int y0, int x1, int y1,
                     int scale, int ox, int oy,
                     char ch, int stage,
                     int delay_us, int plain, int skip_last) {
    int X0 = x0 * scale, Y0 = y0 * scale;
    int X1 = x1 * scale, Y1 = y1 * scale;

    int dx =  abs(X1 - X0), sx = X0 < X1 ? 1 : -1;
    int dy = -abs(Y1 - Y0), sy = Y0 < Y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        int at_end = (X0 == X1 && Y0 == Y1);
        if (!skip_last || !at_end) {
            int col = ox + X0 * 2;   /* x2 aspect correction */
            int row = oy + Y0;
            canvas_put(col,   row, ch, stage);
            canvas_put(col+1, row, ch, stage); /* 1-col thickness */
        }
        if (delay_us > 0) { canvas_render(plain); usleep(delay_us); }
        if (X0 == X1 && Y0 == Y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; X0 += sx; }
        if (e2 <= dx) { err += dx; Y0 += sy; }
    }
}

/* ── Sparks ────────────────────────────────────────────────────────────────── */
#define MAX_SPARKS 120
typedef struct { int x, y, vx, vy, life, stage; char ch; } Spark;
static Spark sparks[MAX_SPARKS];
static int   n_sparks = 0;
static const char SCHARS[] = "*+.'`";

static void spark_emit(int x, int y, int stage, int count) {
    for (int k = 0; k < count && n_sparks < MAX_SPARKS; k++, n_sparks++) {
        sparks[n_sparks].x     = x + (rand() % 7) - 3;
        sparks[n_sparks].y     = y + (rand() % 5) - 2;
        sparks[n_sparks].vx    = (rand() % 3) - 1;
        sparks[n_sparks].vy    = (rand() % 3) - 2;
        sparks[n_sparks].life  = 3 + rand() % 5;
        sparks[n_sparks].stage = stage > 0 ? stage : 1;
        sparks[n_sparks].ch    = SCHARS[rand() % (sizeof(SCHARS) - 1)];
    }
}

static void sparks_tick(void) {
    for (int i = 0; i < n_sparks; i++) {
        if (sparks[i].life <= 0) continue;
        canvas_put(sparks[i].x, sparks[i].y, ' ', 0);
        sparks[i].x += sparks[i].vx;
        sparks[i].y += sparks[i].vy;
        sparks[i].life--;
        if (sparks[i].life > 0)
            canvas_put(sparks[i].x, sparks[i].y, sparks[i].ch, sparks[i].stage);
    }
}

/* ── Signal / cleanup ──────────────────────────────────────────────────────── */
static volatile int g_quit = 0;
static void on_sigint(int s) { (void)s; g_quit = 1; }
static void cleanup(void)    { printf(SHOW RESET "\n"); fflush(stdout); }

/* ── Cool S segments (sq-pixel unit coords) ────────────────────────────────── */
/*
 * sq-pixel space: x∈[0,4], y∈[0,10]
 * Sections:
 *   y=2..4  top bars
 *   y=4..6  diagonal crossing zone
 *   y=6..8  bottom bars
 *   y=0..2  top inverted-V
 *   y=8..10 bottom V
 *   closing connectors: short diagonals at the bar/diagonal junctions
 */
typedef struct { int x0,y0,x1,y1; char ch; int stage; } Seg;

static const Seg SEGS[] = {
    /* stage 1 — top three vertical bars */
    { 0,2, 0,4, '|', 1 },
    { 2,2, 2,4, '|', 1 },
    { 4,2, 4,4, '|', 1 },
    /* stage 2 — bottom three vertical bars */
    { 0,6, 0,8, '|', 2 },
    { 2,6, 2,8, '|', 2 },
    { 4,6, 4,8, '|', 2 },
    /* stage 3 — two diagonal connectors */
    { 0,4, 2,6, '\\', 3 },
    { 2,4, 4,6, '\\', 3 },
    /* stage 4 — top inverted-V */
    { 0,2, 2,0, '/',  4 },
    { 4,2, 2,0, '\\', 4 },
    /* stage 5 — bottom V */
    { 0,8, 2,10,'\\', 5 },
    { 4,8, 2,10,'/',  5 },
    /* stage 6 — left closing diagonal: / bridging left bar to diagonal */
    { 0,6, 1,5, '/', 6 },
    /* stage 7 — right closing diagonal: / bridging diagonal to right bar */
    { 3,5, 4,4, '/', 7 },
};
#define N_SEGS (int)(sizeof(SEGS)/sizeof(SEGS[0]))

/* ── Oppenheimer explosion ─────────────────────────────────────────────────── */
/*
 * Trinity test, July 16, 1945.
 * The S shatters outward in a shockwave of particles, then the screen
 * flashes white, then dims to an ash-grey fallout.
 */
#define MAX_DEBRIS 512
typedef struct {
    float x, y, vx, vy;
    int   life, maxlife, stage;
    char  ch;
} Debris;
static Debris debris[MAX_DEBRIS];
static int    n_debris = 0;

static const char DEBRIS_CHARS[] = "*+.'`|/\\-~^";
static const char FLASH_CHARS[]  = "#@%$&*O0";

static void oppenheimer(int ox, int oy, int scale, int plain, int delay_us) {
    /* --- Phase 0: collect all lit pixels as debris seeds --- */
    n_debris = 0;
    for (int y = 0; y < ch_; y++) {
        for (int x = 0; x < cw; x++) {
            if (canvas[y][x].ch != ' ' && n_debris < MAX_DEBRIS) {
                float cx = (float)(cw) / 2.0f;
                float cy = (float)(ch_) / 2.0f;
                float dx = (float)x - cx;
                float dy = (float)y - cy;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 0.1f) dist = 0.1f;
                float speed = 0.3f + ((float)rand() / RAND_MAX) * 1.2f;
                debris[n_debris].x      = (float)x;
                debris[n_debris].y      = (float)y;
                debris[n_debris].vx     = (dx / dist) * speed + ((float)rand()/RAND_MAX - 0.5f) * 0.5f;
                debris[n_debris].vy     = (dy / dist) * speed * 0.5f + ((float)rand()/RAND_MAX - 0.5f) * 0.3f;
                debris[n_debris].life   = 18 + rand() % 20;
                debris[n_debris].maxlife= debris[n_debris].life;
                debris[n_debris].stage  = canvas[y][x].stage;
                debris[n_debris].ch     = DEBRIS_CHARS[rand() % (sizeof(DEBRIS_CHARS)-1)];
                n_debris++;
            }
        }
    }

    (void)ox; (void)oy; (void)scale;

    /* --- Phase 1: FLASH — rapidly fill screen bright --- */
    if (!plain) {
        for (int flash = 0; flash < 4 && !g_quit; flash++) {
            canvas_clear();
            int col = (flash % 2 == 0) ? 8 : N_COLORS;
            for (int y = 0; y < ch_-1; y++) {
                for (int x = 0; x < cw-1; x++) {
                    if ((x + y + flash) % (2 + flash) == 0) {
                        canvas[y][x].ch    = FLASH_CHARS[rand() % (sizeof(FLASH_CHARS)-1)];
                        canvas[y][x].stage = col;
                    }
                }
            }
            canvas_render(plain);
            usleep(40000);
        }
    }

    /* --- Phase 2: shockwave expansion --- */
    int frames = 40;
    for (int f = 0; f < frames && !g_quit; f++) {
        canvas_clear();

        /* shockwave ring */
        if (!plain) {
            float radius = (float)f * 1.5f;
            float cx2 = (float)cw / 2.0f;
            float cy2 = (float)ch_ / 2.0f;
            for (int theta = 0; theta < 360; theta += 2) {
                float rad = (float)theta * 3.14159f / 180.0f;
                int rx = (int)(cx2 + cosf(rad) * radius * 2.0f);
                int ry = (int)(cy2 + sinf(rad) * radius);
                int sc = (f < 8) ? 4 : (f < 16) ? 3 : 2;
                canvas_put(rx, ry, '*', sc);
            }
        }

        /* update and draw debris */
        for (int i = 0; i < n_debris; i++) {
            if (debris[i].life <= 0) continue;
            debris[i].x  += debris[i].vx;
            debris[i].y  += debris[i].vy;
            debris[i].vy += 0.04f;  /* gravity */
            debris[i].life--;

            /* fade: change char as life drains */
            int li = debris[i].life;
            char dc;
            if (li > debris[i].maxlife * 2 / 3) dc = debris[i].ch;
            else if (li > debris[i].maxlife / 3) dc = '.';
            else                                  dc = '`';

            /* color: starts at own stage, fades toward dim */
            int sc = (li > debris[i].maxlife / 2) ? debris[i].stage : 1;

            canvas_put((int)debris[i].x, (int)debris[i].y, dc, sc);
        }

        canvas_render(plain);
        usleep(delay_us > 0 ? (delay_us < 30000 ? 30000 : delay_us) : 50000);
    }

    /* --- Phase 3: fallout — sparse drifting ash --- */
    for (int f = 0; f < 20 && !g_quit; f++) {
        /* fade all remaining debris */
        for (int i = 0; i < n_debris; i++) {
            if (debris[i].life <= 0) continue;
            canvas_put((int)debris[i].x, (int)debris[i].y, ' ', 0);
            debris[i].y  += 0.15f;
            debris[i].x  += (float)(rand()%3 - 1) * 0.3f;
            debris[i].life -= 2;
            if (debris[i].life > 0)
                canvas_put((int)debris[i].x, (int)debris[i].y, '.', 1);
        }
        canvas_render(plain);
        usleep(80000);
    }

    canvas_clear();
    canvas_render(plain);
}

/* ── Options ───────────────────────────────────────────────────────────────── */
typedef struct {
    int fast, delay_us, scale, rainbow, oppenheimer, no_sparks, plain;
} Opts;

static void usage(const char *prog) {
    printf(
        "Usage: %s [OPTIONS]\n\n"
        "Draw the legendary Cool S in your terminal.\n\n"
        "Options:\n"
        "  -f              Fast mode (no animation)\n"
        "  -d USECS        Per-pixel delay in microseconds (default: 25000)\n"
        "  -s SCALE        Scale factor 1-8 (default: 3)\n"
        "  -r              Rainbow finale\n"
        "  -o, --oppenheimer  Detonate the S at the end\n"
        "  --no-sparks     Disable spark particles\n"
        "  --plain         No color output\n"
        "  -h, --help      Show this help\n\n"
        "Examples:\n"
        "  cool-s                    Animated Cool S\n"
        "  cool-s -f -s 5            Instant, large\n"
        "  cool-s -o                 Now I am become death\n"
        "  cool-s -d 60000 -r        Slow with rainbow\n",
        prog);
}

/* ── Recolor ───────────────────────────────────────────────────────────────── */
static void recolor(int stage) {
    for (int y = 0; y < ch_; y++)
        for (int x = 0; x < cw; x++)
            if (canvas[y][x].ch != ' ')
                canvas[y][x].stage = stage;
}

/* ── Draw the Cool S ───────────────────────────────────────────────────────── */
static void draw_cool_s(const Opts *o) {
    get_term();
    canvas_clear();

    /*
     * Bounding box in terminal cells:
     *   width  = 4 * scale * 2  (sq-pixel x: 0..4, each ×2 for aspect)
     *   height = 10 * scale     (sq-pixel y: 0..10)
     */
    /* auto-reduce scale if S doesn't fit the terminal */
    int scale = o->scale;
    while (scale > 1 && (10 * scale > ch_ - 3 || 4 * scale * 2 > cw - 4))
        scale--;

    int s_w = 4 * scale * 2;
    int s_h = 10 * scale;
    int ox  = (cw  - s_w) / 2;
    int oy  = (ch_ - s_h) / 2;
    if (ox < 1) ox = 1;
    if (oy < 1) oy = 1;

    int delay = o->fast ? 0 : o->delay_us;

    for (int i = 0; i < N_SEGS && !g_quit; i++) {
        const Seg *sg = &SEGS[i];
        int skip_last = (sg->stage == 6 || sg->stage == 7) ? 1 : 0;
        draw_seg(sg->x0, sg->y0, sg->x1, sg->y1,
                 scale, ox, oy,
                 sg->ch, sg->stage,
                 delay, o->plain, skip_last);

        if (!o->no_sparks && delay > 0 && !g_quit) {
            int ex = ox + sg->x1 * scale * 2;
            int ey = oy + sg->y1 * scale;
            spark_emit(ex, ey, sg->stage, 5);
            sparks_tick();
            canvas_render(o->plain);
            usleep(delay * 3);
            sparks_tick();
            canvas_render(o->plain);
            usleep(delay * 2);
        }
    }

    if (g_quit) return;

    /* final colour wash */
    if (!o->fast && delay > 0) {
        usleep(300000);
        /* flash to solid white */
        recolor(N_COLORS);
        canvas_render(o->plain);
        usleep(400000);

        if (o->rainbow && !g_quit) {
            /* rainbow finale: cycle all lit pixels through stage colours */
            for (int p = 0; p < 20 && !g_quit; p++) {
                for (int y = 0; y < ch_; y++)
                    for (int x = 0; x < cw; x++)
                        if (canvas[y][x].ch != ' ')
                            canvas[y][x].stage = ((p + x/3 + y) % (N_COLORS-1)) + 1;
                canvas_render(o->plain);
                usleep(150000);
            }
            /* settle back to white */
            recolor(N_COLORS);
            canvas_render(o->plain);
            usleep(300000);
        }
    } else {
        canvas_render(o->plain);
    }

    /* tagline */
    if (!g_quit && !o->oppenheimer) {
        const char *tag = "~ the Cool S ~";
        int tx = ox + (s_w - (int)strlen(tag)) / 2; (void)scale;
        int ty = oy + s_h + 2;
        for (int i = 0; tag[i] && tx+i < cw-1; i++)
            canvas_put(tx+i, ty, tag[i], 4);
        canvas_render(o->plain);
    }

    /* Oppenheimer */
    if (o->oppenheimer && !g_quit) {
        if (!o->fast) usleep(500000);
        oppenheimer(ox, oy, scale, o->plain, delay);
    }
}

/* ── main ──────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    Opts o = { .fast=0, .delay_us=40000, .scale=3,
               .rainbow=0, .oppenheimer=0, .no_sparks=0, .plain=0 };

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "-f"))                    o.fast = 1;
        else if (!strcmp(argv[i], "-r"))                    o.rainbow = 1;
        else if (!strcmp(argv[i], "-o") ||
                 !strcmp(argv[i], "--oppenheimer"))         o.oppenheimer = 1;
        else if (!strcmp(argv[i], "--no-sparks"))           o.no_sparks = 1;
        else if (!strcmp(argv[i], "--plain"))               o.plain = 1;
        else if (!strcmp(argv[i], "-d") && i+1 < argc) {
            o.delay_us = atoi(argv[++i]);
            if (o.delay_us < 0) o.delay_us = 0;
        } else if (!strcmp(argv[i], "-s") && i+1 < argc) {
            o.scale = atoi(argv[++i]);
            if (o.scale < 1) o.scale = 1;
            if (o.scale > 8) o.scale = 8;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage(argv[0]); return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            usage(argv[0]); return 1;
        }
    }

    srand((unsigned)time(NULL));
    signal(SIGINT, on_sigint);
    atexit(cleanup);

    if (!o.fast) {
        printf(CLEAR HIDE);
        fflush(stdout);
    }

    draw_cool_s(&o);

    if (!o.fast && !g_quit && !o.oppenheimer) {
        printf("\033[%d;1H", g_trows);
        const char *msg = "[ press any key to exit ]";
        int pad = (g_tcols - (int)strlen(msg)) / 2;
        for (int i = 0; i < pad; i++) putchar(' ');
        printf(BOLD "\033[97m%s" RESET, msg);
        fflush(stdout);

        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    printf(CLEAR HOME SHOW RESET);
    fflush(stdout);
    return 0;
}
