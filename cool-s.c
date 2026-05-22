#define _POSIX_C_SOURCE 200809L
/*
 * cool-s — Draw the legendary "Cool S" (Universal S / Graffiti S) in your terminal.
 *
 * Inspired by `sl` (steam locomotive), this program animates the step-by-step
 * construction of the Cool S — the graffiti symbol every 90s kid drew in notebooks.
 *
 * The Cool S consists of 14 line segments built in 7 steps:
 *   Step 1: Three short vertical bars (top group)
 *   Step 2: Three short vertical bars (bottom group)
 *   Step 3: Two diagonal connectors crossing the middle
 *   Step 4: Inverted-V cap at the top
 *   Step 5: V base at the bottom
 *   Step 6: Left closing connector
 *   Step 7: Right closing connector
 *
 * Build:   gcc -O2 -o cool-s src/cool-s.c -lm
 * Usage:   cool-s [-f] [-d DELAY] [-s SCALE] [-r] [--no-sparks] [--plain] [-h]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* usleep, STDOUT_FILENO */
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <termios.h>

/* ── ANSI helpers ──────────────────────────────────────────────────────────── */
#define CSI             "\033["
#define CLEAR_SCREEN    "\033[2J"
#define CURSOR_HOME     "\033[H"
#define CURSOR_HIDE     "\033[?25l"
#define CURSOR_SHOW     "\033[?25h"
#define RESET           "\033[0m"
#define BOLD            "\033[1m"

/* Stage colors — one per drawing step */
static const char *STAGE_COLOR[] = {
    "\033[97m",   /* 1  top bars      — bright white  */
    "\033[97m",   /* 2  bottom bars   — bright white  */
    "\033[96m",   /* 3  diagonals     — bright cyan   */
    "\033[93m",   /* 4  top V         — bright yellow */
    "\033[92m",   /* 5  bottom V      — bright green  */
    "\033[95m",   /* 6  close left    — bright magenta*/
    "\033[91m",   /* 7  close right   — bright red    */
    "\033[97m",   /* 8  final wash    — bright white  */
};
#define N_COLORS 8

/* ── Canvas ────────────────────────────────────────────────────────────────── */
#define CANVAS_W 120
#define CANVAS_H  48

typedef struct { char ch; int stage; } Cell;
static Cell canvas[CANVAS_H][CANVAS_W];

static void canvas_clear(void) {
    memset(canvas, 0, sizeof canvas);
    for (int y=0;y<CANVAS_H;y++)
        for (int x=0;x<CANVAS_W;x++)
            canvas[y][x].ch = ' ';
}

static void canvas_put(int x, int y, char ch, int stage) {
    if (x<0||x>=CANVAS_W||y<0||y>=CANVAS_H) return;
    canvas[y][x].ch    = ch;
    canvas[y][x].stage = stage;
}

static void canvas_render(int plain) {
    printf(CURSOR_HOME);
    int last = -1;
    for (int y=0; y<CANVAS_H; y++) {
        for (int x=0; x<CANVAS_W; x++) {
            Cell *c = &canvas[y][x];
            if (!plain && c->stage != last) {
                printf(RESET);
                if (c->stage > 0 && c->stage <= N_COLORS) {
                    printf(BOLD "%s", STAGE_COLOR[c->stage-1]);
                }
                last = c->stage;
            }
            putchar(c->ch);
        }
        putchar('\n');
    }
    printf(RESET);
    fflush(stdout);
}

/* ── Bresenham line ────────────────────────────────────────────────────────── */
/* Coordinates are in "S-units"; each unit = (2*scale) cols × scale rows      */
static void draw_segment(int x0,int y0,int x1,int y1,
                         int scale, int ox, int oy,
                         char ch, int stage,
                         int delay_us, int plain) {
    /* map to terminal cells; x gets ×2 to compensate for char aspect ratio   */
    int tx0 = ox + x0*2*scale;
    int ty0 = oy + y0*scale;
    int tx1 = ox + x1*2*scale;
    int ty1 = oy + y1*scale;

    int dx =  abs(tx1-tx0), sx = tx0<tx1 ? 1:-1;
    int dy = -abs(ty1-ty0), sy = ty0<ty1 ? 1:-1;
    int err = dx+dy, e2;

    for(;;) {
        canvas_put(tx0, ty0, ch, stage);
        if (delay_us>0) { canvas_render(plain); usleep(delay_us); }
        if (tx0==tx1 && ty0==ty1) break;
        e2 = 2*err;
        if (e2 >= dy) { err+=dy; tx0+=sx; }
        if (e2 <= dx) { err+=dx; ty0+=sy; }
    }
}

/* Fill a thick line by drawing multiple offset copies for visual weight */
static void draw_thick(int x0,int y0,int x1,int y1,
                       int scale, int ox, int oy,
                       char ch, int stage,
                       int delay_us, int plain) {
    draw_segment(x0,y0,x1,y1, scale,ox,oy,ch,stage,delay_us,plain);
    /* draw one extra copy shifted by 1 terminal col for thickness */
    if (scale >= 2) {
        ox++;
        draw_segment(x0,y0,x1,y1, scale,ox,oy,ch,stage,0,plain);
        ox--;
    }
}

/* ── Spark particles ───────────────────────────────────────────────────────── */
#define MAX_SPARKS 80
typedef struct { int x,y,life,stage; char ch; } Spark;
static Spark sparks[MAX_SPARKS];
static int   n_sparks = 0;

static const char SPARK_CHARS[] = "*·+°.`";

static void spark_emit(int x, int y, int stage) {
    for (int k=0; k<4 && n_sparks<MAX_SPARKS; k++, n_sparks++) {
        sparks[n_sparks].x     = x + (rand()%7)-3;
        sparks[n_sparks].y     = y + (rand()%5)-2;
        sparks[n_sparks].life  = 2 + rand()%5;
        sparks[n_sparks].stage = stage > 0 ? stage : 1;
        sparks[n_sparks].ch    = SPARK_CHARS[rand()%(sizeof(SPARK_CHARS)-1)];
    }
}

static void sparks_tick(void) {
    for (int i=0; i<n_sparks; i++) {
        if (sparks[i].life <= 0) continue;
        /* erase old position */
        canvas_put(sparks[i].x, sparks[i].y, ' ', 0);
        sparks[i].life--;
        sparks[i].y += (rand()%3)-2;
        sparks[i].x += (rand()%3)-1;
        if (sparks[i].life > 0)
            canvas_put(sparks[i].x, sparks[i].y, sparks[i].ch, sparks[i].stage);
    }
}

/* ── Terminal size ─────────────────────────────────────────────────────────── */
static void term_size(int *cols, int *rows) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws)==0 && ws.ws_col>0) {
        *cols = ws.ws_col; *rows = ws.ws_row;
    } else { *cols=80; *rows=24; }
}

/* ── Signal ────────────────────────────────────────────────────────────────── */
static volatile int g_quit = 0;
static void on_sigint(int s){(void)s; g_quit=1;}

static void cleanup(void) { printf(CURSOR_SHOW RESET "\n"); fflush(stdout); }

/* ── Cool S geometry ───────────────────────────────────────────────────────── */
/*
 * Canonical Wikipedia coordinates (Cartesian, y-up, range x∈[0,2] y∈[0,5]):
 *
 *  Top bars:      (0,4)→(0,3)  (1,4)→(1,3)  (2,4)→(2,3)
 *  Bottom bars:   (0,2)→(0,1)  (1,2)→(1,1)  (2,2)→(2,1)
 *  Diagonals:     (0,3)→(1,2)  (1,3)→(2,2)
 *  Top inv-V:     (0,4)→(1,5)  (2,4)→(1,5)
 *  Bottom V:      (0,1)→(1,0)  (2,1)→(1,0)
 *  Close left:    (0,2)→(0,3)  [seals gap on left side]
 *  Close right:   (2,2)→(2,3)  [seals gap on right side]
 *
 * Terminal: y is flipped (y_term = 5 - y_wiki), x scaled ×2 for aspect ratio.
 */
typedef struct { int x0,y0,x1,y1; char ch; int stage; } Seg;

#define FLIP(y)  (5-(y))

static const Seg SEGS[] = {
    /* Stage 1 — top three vertical bars */
    {0,FLIP(3), 0,FLIP(4), '|', 1},
    {1,FLIP(3), 1,FLIP(4), '|', 1},
    {2,FLIP(3), 2,FLIP(4), '|', 1},
    /* Stage 2 — bottom three vertical bars */
    {0,FLIP(1), 0,FLIP(2), '|', 2},
    {1,FLIP(1), 1,FLIP(2), '|', 2},
    {2,FLIP(1), 2,FLIP(2), '|', 2},
    /* Stage 3 — two diagonal connectors */
    {0,FLIP(3), 1,FLIP(2), '\\', 3},
    {1,FLIP(3), 2,FLIP(2), '\\', 3},
    /* Stage 4 — top inverted-V */
    {0,FLIP(4), 1,FLIP(5), '/', 4},
    {2,FLIP(4), 1,FLIP(5), '\\', 4},
    /* Stage 5 — bottom V */
    {0,FLIP(1), 1,FLIP(0), '\\', 5},
    {2,FLIP(1), 1,FLIP(0), '/', 5},
    /* Stage 6 — left closing connector */
    {0,FLIP(2), 0,FLIP(3), '#', 6},
    /* Stage 7 — right closing connector */
    {2,FLIP(2), 2,FLIP(3), '#', 7},
};
#define N_SEGS (int)(sizeof(SEGS)/sizeof(SEGS[0]))

/* ── Options ───────────────────────────────────────────────────────────────── */
typedef struct {
    int fast, delay_us, scale, rainbow, no_sparks, plain;
} Opts;

static void usage(const char *prog) {
    printf(
        "Usage: %s [OPTIONS]\n\n"
        "Draw the legendary Cool S (Universal S / Graffiti S) in your terminal.\n\n"
        "Options:\n"
        "  -f            Fast mode — skip animation, instant render\n"
        "  -d USECS      Per-pixel delay in microseconds (default 25000)\n"
        "  -s SCALE      Scale factor 1–8 (default 4)\n"
        "  -r            Rainbow finale mode\n"
        "  --no-sparks   Disable spark particles\n"
        "  --plain       No color output\n"
        "  -h, --help    Show this help and exit\n\n"
        "Examples:\n"
        "  cool-s                 Animate at default size\n"
        "  cool-s -f -s 6         Instant big Cool S\n"
        "  cool-s -d 60000 -r     Slow with rainbow finale\n"
        "  cool-s -s 2 --plain    Small monochrome\n",
        prog);
}

/* ── Final color wash helper ───────────────────────────────────────────────── */
static void recolor(int stage) {
    for (int y=0;y<CANVAS_H;y++)
        for (int x=0;x<CANVAS_W;x++)
            if (canvas[y][x].ch != ' ')
                canvas[y][x].stage = stage;
}

/* ── Main draw ─────────────────────────────────────────────────────────────── */
static void draw_cool_s(const Opts *o) {
    int tcols, trows;
    term_size(&tcols, &trows);

    /* S bounding box: x ∈ [0,4*scale], y ∈ [0,5*scale] */
    int sw = 4*o->scale + 2;
    int sh = 6*o->scale + 4;
    int ox = (tcols - sw) / 2;
    int oy = (trows - sh) / 2;
    if (ox < 1) ox = 1;
    if (oy < 1) oy = 1;

    canvas_clear();

    int delay = o->fast ? 0 : o->delay_us;

    for (int i=0; i<N_SEGS && !g_quit; i++) {
        const Seg *s = &SEGS[i];
        draw_thick(s->x0, s->y0, s->x1, s->y1,
                   o->scale, ox, oy,
                   s->ch, s->stage,
                   delay, o->plain);

        if (!o->no_sparks && delay>0 && !g_quit) {
            int ex = ox + s->x1*2*o->scale;
            int ey = oy + s->y1*o->scale;
            spark_emit(ex, ey, s->stage);
            sparks_tick();
            canvas_render(o->plain);
            usleep(delay * 4);
            sparks_tick();
            canvas_render(o->plain);
            usleep(delay * 2);
        }
    }

    if (g_quit) return;

    if (!o->fast && delay>0) {
        usleep(250000);

        if (o->rainbow) {
            for (int pass=0; pass<8 && !g_quit; pass++) {
                for (int y=0;y<CANVAS_H;y++)
                    for (int x=0;x<CANVAS_W;x++)
                        if (canvas[y][x].ch != ' ')
                            canvas[y][x].stage = ((pass*2 + x/3 + y) % (N_COLORS-1)) + 1;
                canvas_render(o->plain);
                usleep(120000);
            }
        }
        /* final: wash to white */
        recolor(N_COLORS);
        canvas_render(o->plain);
        usleep(200000);
    } else {
        canvas_render(o->plain);
    }

    /* tagline */
    if (!g_quit) {
        int ty = oy + 6*o->scale + 2;
        int tx = ox + o->scale*2 - 7;
        const char *tag = "~ the Cool S ~";
        for (int i=0; tag[i] && tx+i < CANVAS_W; i++)
            canvas_put(tx+i, ty, tag[i], 4);
        canvas_render(o->plain);
    }
}

/* ── main ──────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    Opts o = { .fast=0, .delay_us=25000, .scale=4,
               .rainbow=0, .no_sparks=0, .plain=0 };

    for (int i=1; i<argc; i++) {
        if      (!strcmp(argv[i],"-f"))           o.fast=1;
        else if (!strcmp(argv[i],"-r"))           o.rainbow=1;
        else if (!strcmp(argv[i],"--no-sparks"))  o.no_sparks=1;
        else if (!strcmp(argv[i],"--plain"))      o.plain=1;
        else if (!strcmp(argv[i],"-d") && i+1<argc) {
            o.delay_us = atoi(argv[++i]);
            if (o.delay_us<0) o.delay_us=0;
        } else if (!strcmp(argv[i],"-s") && i+1<argc) {
            o.scale = atoi(argv[++i]);
            if (o.scale<1) o.scale=1;
            if (o.scale>8) o.scale=8;
        } else if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"--help")) {
            usage(argv[0]); return 0;
        } else {
            fprintf(stderr,"Unknown option: %s\n",argv[i]);
            usage(argv[0]); return 1;
        }
    }

    srand((unsigned)time(NULL));
    signal(SIGINT, on_sigint);
    atexit(cleanup);

    if (!o.fast) {
        printf(CLEAR_SCREEN CURSOR_HIDE);
        fflush(stdout);
    }

    draw_cool_s(&o);

    if (!o.fast && !g_quit) {
        int tcols, trows;
        term_size(&tcols, &trows);
        printf("\033[%d;%dH", trows-1, (tcols-26)/2);
        printf(BOLD "\033[97m" "[ press any key to exit ]" RESET);
        fflush(stdout);

        struct termios oldt, newt;
        tcgetattr(0, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(0, TCSANOW, &newt);
        getchar();
        tcsetattr(0, TCSANOW, &oldt);

        printf(CLEAR_SCREEN CURSOR_HOME CURSOR_SHOW RESET);
        fflush(stdout);
    }

    return 0;
}
