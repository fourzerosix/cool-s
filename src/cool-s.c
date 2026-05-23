#define _GNU_SOURCE
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
 *   Step 6: Left closing connector  (vertical, same column as left bars)
 *   Step 7: Right closing connector (vertical, same column as right bars)
 *
 * Build:   gcc -O2 -o cool-s src/cool-s.c -lm
 * Usage:   cool-s [-f] [-d DELAY] [-s SCALE] [-r] [--no-sparks] [--plain] [-h]
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

/* ── ANSI helpers ──────────────────────────────────────────────────────────── */
#define CLEAR_SCREEN  "\033[2J"
#define CURSOR_HOME   "\033[H"
#define CURSOR_HIDE   "\033[?25l"
#define CURSOR_SHOW   "\033[?25h"
#define RESET         "\033[0m"
#define BOLD          "\033[1m"

/* One color per drawing stage */
static const char *STAGE_COLOR[] = {
    "\033[97m",   /* 1  top bars       — bright white   */
    "\033[97m",   /* 2  bottom bars    — bright white   */
    "\033[96m",   /* 3  diagonals      — bright cyan    */
    "\033[93m",   /* 4  top inv-V      — bright yellow  */
    "\033[92m",   /* 5  bottom V       — bright green   */
    "\033[95m",   /* 6  close left     — bright magenta */
    "\033[91m",   /* 7  close right    — bright red     */
    "\033[97m",   /* 8  final wash     — bright white   */
};
#define N_COLORS 8

/* ── Terminal size ─────────────────────────────────────────────────────────── */
static int g_cols = 80, g_rows = 24;

static void refresh_term_size(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        g_cols = ws.ws_col;
        g_rows = ws.ws_row;
    } else {
        g_cols = 80;
        g_rows = 24;
    }
}

/* ── Canvas — allocated to match terminal size at startup ─────────────────── */
#define MAX_W 512
#define MAX_H 256

typedef struct { char ch; int stage; } Cell;
static Cell canvas[MAX_H][MAX_W];
static int  cw, ch_; /* actual canvas dimensions = terminal size */

static void canvas_init(void) {
    refresh_term_size();
    cw  = g_cols;
    ch_ = g_rows;
    if (cw  > MAX_W) cw  = MAX_W;
    if (ch_ > MAX_H) ch_ = MAX_H;
}

static void canvas_clear(void) {
    for (int y = 0; y < ch_; y++)
        for (int x = 0; x < cw; x++) {
            canvas[y][x].ch    = ' ';
            canvas[y][x].stage = 0;
        }
}

static void canvas_put(int x, int y, char c, int stage) {
    if (x < 0 || x >= cw || y < 0 || y >= ch_) return;
    canvas[y][x].ch    = c;
    canvas[y][x].stage = stage;
}

/*
 * Render the canvas to stdout.
 * We print exactly cw-1 characters per row (leave last col to avoid
 * forced line-wrap on terminals that don't have auto-margin off), then \n.
 * We render ch_-1 rows so the last row stays free for the "press key" prompt.
 */
static void canvas_render(int plain) {
    printf(CURSOR_HOME);
    int last_stage = -1;
    int render_rows = ch_ - 1;   /* keep last row for prompt */
    int render_cols = cw  - 1;   /* keep last col to avoid wrap */

    for (int y = 0; y < render_rows; y++) {
        for (int x = 0; x < render_cols; x++) {
            Cell *c = &canvas[y][x];
            if (!plain && c->stage != last_stage) {
                printf(RESET);
                if (c->stage > 0 && c->stage <= N_COLORS)
                    printf(BOLD "%s", STAGE_COLOR[c->stage - 1]);
                last_stage = c->stage;
            }
            putchar(c->ch);
        }
        putchar('\n');
    }
    printf(RESET);
    fflush(stdout);
}

/* ── Bresenham line ────────────────────────────────────────────────────────── */
/*
 * Coordinates are in "S-units" (canonical 0-2 x, 0-5 y).
 * Terminal mapping:
 *   tx = ox + x_unit * 2 * scale   (×2 compensates for char aspect ratio)
 *   ty = oy + y_unit * scale
 */
static void draw_line(int x0, int y0, int x1, int y1,
                      int scale, int ox, int oy,
                      char ch, int stage,
                      int delay_us, int plain) {
    int tx0 = ox + x0 * 2 * scale;
    int ty0 = oy + y0 * scale;
    int tx1 = ox + x1 * 2 * scale;
    int ty1 = oy + y1 * scale;

    int dx =  abs(tx1 - tx0), sx = tx0 < tx1 ? 1 : -1;
    int dy = -abs(ty1 - ty0), sy = ty0 < ty1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        canvas_put(tx0, ty0, ch, stage);
        if (delay_us > 0) { canvas_render(plain); usleep(delay_us); }
        if (tx0 == tx1 && ty0 == ty1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; tx0 += sx; }
        if (e2 <= dx) { err += dx; ty0 += sy; }
    }
    /* always do one final thickness pass — one col to the right for diagonals */
    if (scale >= 2 && tx0 != tx1) {
        /* already at endpoint; re-walk just for fill */
    }
    /* extra thickness: nudge ox+1 and redraw silently */
    if (scale >= 2) {
        int ax0 = ox + x0*2*scale + 1, ay0 = oy + y0*scale;
        int ax1 = ox + x1*2*scale + 1, ay1 = oy + y1*scale;
        int adx =  abs(ax1-ax0), asx = ax0<ax1?1:-1;
        int ady = -abs(ay1-ay0), asy = ay0<ay1?1:-1;
        int aerr = adx+ady, ae2;
        for(;;){
            canvas_put(ax0,ay0,ch,stage);
            if(ax0==ax1&&ay0==ay1) break;
            ae2=2*aerr;
            if(ae2>=ady){aerr+=ady;ax0+=asx;}
            if(ae2<=adx){aerr+=adx;ay0+=asy;}
        }
    }
}

/* ── Spark particles ───────────────────────────────────────────────────────── */
#define MAX_SPARKS 80
typedef struct { int x, y, life, stage; char ch; } Spark;
static Spark sparks[MAX_SPARKS];
static int   n_sparks = 0;
static const char SPARK_CHARS[] = "*+.`'";

static void spark_emit(int x, int y, int stage) {
    for (int k = 0; k < 5 && n_sparks < MAX_SPARKS; k++, n_sparks++) {
        sparks[n_sparks].x     = x + (rand() % 7) - 3;
        sparks[n_sparks].y     = y + (rand() % 5) - 2;
        sparks[n_sparks].life  = 2 + rand() % 5;
        sparks[n_sparks].stage = stage > 0 ? stage : 1;
        sparks[n_sparks].ch    = SPARK_CHARS[rand() % (sizeof(SPARK_CHARS)-1)];
    }
}

static void sparks_tick(void) {
    for (int i = 0; i < n_sparks; i++) {
        if (sparks[i].life <= 0) continue;
        canvas_put(sparks[i].x, sparks[i].y, ' ', 0);
        sparks[i].life--;
        sparks[i].y += (rand() % 3) - 2;
        sparks[i].x += (rand() % 3) - 1;
        if (sparks[i].life > 0)
            canvas_put(sparks[i].x, sparks[i].y, sparks[i].ch, sparks[i].stage);
    }
}

/* ── Signal / cleanup ──────────────────────────────────────────────────────── */
static volatile int g_quit = 0;
static void on_sigint(int s) { (void)s; g_quit = 1; }
static void cleanup(void)    { printf(CURSOR_SHOW RESET "\n"); fflush(stdout); }


/* ── Direct terminal-pixel Bresenham (no S-unit scaling) ──────────────────── */
static void draw_px(int tx0, int ty0, int tx1, int ty1,
                    char ch, int stage, int delay_us, int plain) {
    int dx =  abs(tx1-tx0), sx = tx0<tx1 ? 1:-1;
    int dy = -abs(ty1-ty0), sy = ty0<ty1 ? 1:-1;
    int err = dx+dy, e2;
    for (;;) {
        canvas_put(tx0, ty0, ch, stage);
        if (delay_us > 0) { canvas_render(plain); usleep(delay_us); }
        if (tx0==tx1 && ty0==ty1) break;
        e2 = 2*err;
        if (e2 >= dy) { err+=dy; tx0+=sx; }
        if (e2 <= dx) { err+=dx; ty0+=sy; }
    }
}

/* ── Cool S geometry ───────────────────────────────────────────────────────── */
/*
 * Canonical Wikipedia coordinates (y-up, x∈[0,2], y∈[0,5]).
 * We flip to terminal y-down: ty = 5 - cy.
 *
 * The 14 segments:
 *   Top bars:      (0,4)→(0,3)   (1,4)→(1,3)   (2,4)→(2,3)
 *   Bottom bars:   (0,2)→(0,1)   (1,2)→(1,1)   (2,2)→(2,1)
 *   Diagonals:     (0,3)→(1,2)   (1,3)→(2,2)
 *   Top inv-V:     (0,4)→(1,5)   (2,4)→(1,5)
 *   Bottom V:      (0,1)→(1,0)   (2,1)→(1,0)
 *   Close left:    (0,2)→(0,3)   ← vertical, fills gap on left column
 *   Close right:   (2,2)→(2,3)   ← vertical, fills gap on right column
 */
#define FLIP(y)  (5-(y))

typedef struct { int x0,y0, x1,y1; char ch; int stage; } Seg;

static const Seg SEGS[] = {
    /* Stage 1 — top vertical bars */
    { 0,FLIP(3), 0,FLIP(4), '|', 1 },
    { 1,FLIP(3), 1,FLIP(4), '|', 1 },
    { 2,FLIP(3), 2,FLIP(4), '|', 1 },
    /* Stage 2 — bottom vertical bars */
    { 0,FLIP(1), 0,FLIP(2), '|', 2 },
    { 1,FLIP(1), 1,FLIP(2), '|', 2 },
    { 2,FLIP(1), 2,FLIP(2), '|', 2 },
    /* Stage 3 — diagonal connectors */
    { 0,FLIP(3), 1,FLIP(2), '\\', 3 },
    { 1,FLIP(3), 2,FLIP(2), '\\', 3 },
    /* Stage 4 — top inverted-V */
    { 0,FLIP(4), 1,FLIP(5), '/',  4 },
    { 2,FLIP(4), 1,FLIP(5), '\\', 4 },
    /* Stage 5 — bottom V */
    { 0,FLIP(1), 1,FLIP(0), '\\', 5 },
    { 2,FLIP(1), 1,FLIP(0), '/',  5 },
    /* Stages 6 & 7 are drawn separately in draw_cool_s() using
     * terminal pixel coordinates, because they span fractional S-units. */
};
#define N_SEGS (int)(sizeof(SEGS)/sizeof(SEGS[0]))

/* ── Options ───────────────────────────────────────────────────────────────── */
typedef struct {
    int fast, delay_us, scale, rainbow, no_sparks, plain;
} Opts;

static void usage(const char *prog) {
    printf(
        "Usage: %s [OPTIONS]\n\n"
        "Draw the legendary Cool S in your terminal.\n\n"
        "Options:\n"
        "  -f            Fast mode  (no animation)\n"
        "  -d USECS      Per-pixel delay in microseconds (default 25000)\n"
        "  -s SCALE      Scale factor 1-8 (default 4)\n"
        "  -r            Rainbow finale mode\n"
        "  --no-sparks   Disable spark particles\n"
        "  --plain       No color output\n"
        "  -h, --help    Show this help and exit\n\n"
        "Examples:\n"
        "  cool-s                 Animated at default size\n"
        "  cool-s -f -s 6         Instant, big\n"
        "  cool-s -d 60000 -r     Slow with rainbow finale\n"
        "  cool-s -s 2 --plain    Small monochrome\n",
        prog);
}

/* ── Recolor entire canvas ─────────────────────────────────────────────────── */
static void recolor(int stage) {
    for (int y = 0; y < ch_; y++)
        for (int x = 0; x < cw; x++)
            if (canvas[y][x].ch != ' ')
                canvas[y][x].stage = stage;
}

/* ── Main draw ─────────────────────────────────────────────────────────────── */
static void draw_cool_s(const Opts *o) {
    canvas_init();
    canvas_clear();

    /*
     * S bounding box in terminal cells:
     *   width  = 2 * 2 * scale  (x goes 0..2, each unit = 2*scale cols)
     *   height = 5 * scale      (y goes 0..5, each unit = scale rows)
     * Add a small margin.
     */
    int s_w = 2 * 2 * o->scale;   /* = 4*scale terminal columns */
    int s_h = 5 * o->scale;        /* terminal rows */

    /* Center origin: top-left corner of bounding box */
    int ox = (cw  - s_w) / 2;
    int oy = (ch_ - s_h) / 2;
    if (ox < 2) ox = 2;
    if (oy < 1) oy = 1;

    int delay = o->fast ? 0 : o->delay_us;

    for (int i = 0; i < N_SEGS && !g_quit; i++) {
        const Seg *s = &SEGS[i];
        draw_line(s->x0, s->y0, s->x1, s->y1,
                  o->scale, ox, oy,
                  s->ch, s->stage,
                  delay, o->plain);

        if (!o->no_sparks && delay > 0 && !g_quit) {
            int ex = ox + s->x1 * 2 * o->scale;
            int ey = oy + s->y1 * o->scale;
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

    /* ── Stages 6 & 7: closing diagonal connectors ─────────────────────────────
     * Wikipedia: close-left  = (0,2)→(0.5,2.5) in Cartesian S-units
     *            close-right = (2,3)→(1.5,2.5) in Cartesian S-units
     * In terminal coords (y-down, each S-unit = 2*scale cols × scale rows):
     *   close-left  '/' : from (ox,            oy+3*scale) → (ox+  scale, oy+5*scale/2)
     *   close-right '\' : from (ox+4*scale,    oy+2*scale) → (ox+3*scale, oy+5*scale/2)
     */
    if (!g_quit) {
        int s = o->scale;
        /* left connector — '\' seals the left gap, going right+down from left bar bottom */
        draw_px(ox,         oy + 3*s,
                ox + s,     oy + 3*s + s/2,
                '\\', 6, delay, o->plain);
        if (!o->no_sparks && delay > 0 && !g_quit) {
            spark_emit(ox + s, oy + 3*s + s/2, 6);
            sparks_tick(); canvas_render(o->plain); usleep(delay * 3);
        }
        /* right connector — '/' seals the right gap, going left+down from right bar bottom */
        draw_px(ox + 4*s,   oy + 3*s + 1,
                ox + 3*s,   oy + 3*s + s/2,
                '/', 7, delay, o->plain);
        if (!o->no_sparks && delay > 0 && !g_quit) {
            spark_emit(ox + 3*s, oy + 3*s + s/2, 7);
            sparks_tick(); canvas_render(o->plain); usleep(delay * 3);
        }
    }

    if (g_quit) return;

    if (!o->fast && delay > 0) {
        usleep(250000);

        if (o->rainbow) {
            for (int pass = 0; pass < 8 && !g_quit; pass++) {
                for (int y = 0; y < ch_; y++)
                    for (int x = 0; x < cw; x++)
                        if (canvas[y][x].ch != ' ')
                            canvas[y][x].stage =
                                ((pass*2 + x/4 + y/2) % (N_COLORS-1)) + 1;
                canvas_render(o->plain);
                usleep(120000);
            }
        }
        recolor(N_COLORS);
        canvas_render(o->plain);
        usleep(200000);
    } else {
        canvas_render(o->plain);
    }

    /* tagline below the S */
    if (!g_quit) {
        int ty = oy + s_h + 2;
        const char *tag = "~ the Cool S ~";
        int tx = ox + (s_w - (int)strlen(tag)) / 2;
        for (int i = 0; tag[i] && tx+i < cw-1; i++)
            canvas_put(tx+i, ty, tag[i], 4);
        canvas_render(o->plain);
    }
}

/* ── main ──────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    Opts o = { .fast=0, .delay_us=25000, .scale=4,
               .rainbow=0, .no_sparks=0, .plain=0 };

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "-f"))           o.fast = 1;
        else if (!strcmp(argv[i], "-r"))           o.rainbow = 1;
        else if (!strcmp(argv[i], "--no-sparks"))  o.no_sparks = 1;
        else if (!strcmp(argv[i], "--plain"))      o.plain = 1;
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
        printf(CLEAR_SCREEN CURSOR_HIDE);
        fflush(stdout);
    }

    draw_cool_s(&o);

    if (!o.fast && !g_quit) {
        /* position prompt on the last terminal row */
        printf("\033[%d;1H", g_rows);
        const char *msg = "[ press any key to exit ]";
        int pad = (g_cols - (int)strlen(msg)) / 2;
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

        printf(CLEAR_SCREEN CURSOR_HOME CURSOR_SHOW RESET);
        fflush(stdout);
    }

    return 0;
}
