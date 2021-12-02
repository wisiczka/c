
// Some compilers don't know about these???
#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

#define MAX(a,b) ((a > b) ? (a) : (b))
#define MIN(a,b) ((a < b) ? (a) : (b))

// Function prototypes
void init(void);
void shutdown(void);
void draw_map(void);
void init_map(void);
void play(void);
void parse(void);
void bsp_split(int t, int l, int h, int w, bool vert);
bool move_player(int speed_y, int speed_x);
void fill_area(int t, int l, int h, int w);
void populate(int t, int l, int b, int r);
int new_animal(void);
int animal_terrain(int type);
void legend(void);
void parsebox(void);
void calc_fov(void);
void move_animals(void);
int blocking(int y, int x);
void make_entrance(void);
void recenter(void);
void fill_special(int t, int l, int h, int w);
void house(int t, int l, int h, int w);
int lazy(int type);
int gtile(int y, int x);
void stile(int y, int x, int tile);
void tilefill(int t, int l, int b, int r, int tile);

void parse_pinput();
void removeStringTrailingNewline(char *str) ;


void init_nouns();

// How much of the map we display at any time
#define BOARD_W 60
#define BOARD_H 24
#define DISP_W 60
#define DISP_H 10

// The full size of the map
#define MAP_W 120
#define MAP_H 88


#define FOV_IN_VIEW (1<<0)
#define FOV_SEEN    (1<<1)
#define FOV_RADIUS  10

// These are used in a bitmask by blocking()
// to describe the properties of tiles
#define CANT_MOVE     (1<<0) // Creatures can't move there
#define CANT_SEE      (1<<1) // Blocks FOV
#define OUTSIDE_MAP   (1<<2) // Invalid coordinate
#define SOMEONE_THERE (1<<3) // Occupied by mob

// The maximum size of BSP cells
#define BSP_MAX_W 40
#define BSP_MAX_H 32


// How many mobs (animals) the map can hold || will replace max_nouns
#define MAX_MOBS 250

// Used during generation; this is the index of the next mob_t we can
// use for placing animals. Index 0 is reserved for the player.
int next_mob = 1;


// The current offset of the viewport
int view_x = 0;
int view_y = 0;


int map[MAP_H][MAP_W];

// Field of view uses two layers composed as a bitmask
// FOV_IN_VIEW are things currently within view
// FOV_SEEN are things previously explored

int fov[MAP_H][MAP_W];

// These keys will be set to platform-specific codes
int key_left;
int key_right;
int key_up;
int key_down;

// curses windows
WINDOW * board;
WINDOW * disp;
WINDOW * disp2;

// curses color pair; note that the pair indices
// do not correspond with the COLOR_* constants
typedef enum
{
  pair_white,
  pair_red,
  pair_green,
  pair_cyan,
  pair_brown,
  pair_black,
  pair_magenta,
  pair_blue
} pair_t;



// Where the player enters the map. Set in init_map(),
// used in fill_area() and some other places.
int entrance;

#define ENT_NW 0
#define ENT_NE 1
#define ENT_SE 2
#define ENT_SW 3

#define DW_N 0
#define DW_E 1
#define DW_S 2
#define DW_W 3

//  Ok, we're done with declarations; on to the code!








/*
  sin table for 360 degrees (cos uses the same value offset by 90),
  included here to avoid the need to link with -lm (the math library;
  which is actually part of the standard library... I'm not sure what
  good this is really supposed to do, but I have wanted for some time
  to try using my own table). This table is used by the LOS algorithm;
  even provided this ridiculious resolution it STILL leaves artifacts
  near walls. I challenge you to write a less efficient LOS routine!

  I typed this out by hand.                             (not really)
*/

float sin_tbl[360] = {
  0.000000, 0.017452, 0.034899, 0.052336, 0.069756, 0.087156,
  0.104528, 0.121869, 0.139173, 0.156434, 0.173648, 0.190809,
  0.207912, 0.224951, 0.241922, 0.258819, 0.275637, 0.292372,
  0.309017, 0.325568, 0.342020, 0.358368, 0.374607, 0.390731,
  0.406737, 0.422618, 0.438371, 0.453991, 0.469472, 0.484810,
  0.500000, 0.515038, 0.529919, 0.544639, 0.559193, 0.573576,
  0.587785, 0.601815, 0.615662, 0.629320, 0.642788, 0.656059,
  0.669131, 0.681998, 0.694658, 0.707107, 0.719340, 0.731354,
  0.743145, 0.754710, 0.766044, 0.777146, 0.788011, 0.798636,
  0.809017, 0.819152, 0.829038, 0.838671, 0.848048, 0.857167,
  0.866025, 0.874620, 0.882948, 0.891007, 0.898794, 0.906308,
  0.913545, 0.920505, 0.927184, 0.933580, 0.939693, 0.945519,
  0.951057, 0.956305, 0.961262, 0.965926, 0.970296, 0.974370,
  0.978148, 0.981627, 0.984808, 0.987688, 0.990268, 0.992546,
  0.994522, 0.996195, 0.997564, 0.998630, 0.999391, 0.999848,
  1.000000, 0.999848, 0.999391, 0.998630, 0.997564, 0.996195,
  0.994522, 0.992546, 0.990268, 0.987688, 0.984808, 0.981627,
  0.978148, 0.974370, 0.970296, 0.965926, 0.961262, 0.956305,
  0.951056, 0.945519, 0.939693, 0.933580, 0.927184, 0.920505,
  0.913545, 0.906308, 0.898794, 0.891006, 0.882948, 0.874620,
  0.866025, 0.857167, 0.848048, 0.838671, 0.829038, 0.819152,
  0.809017, 0.798635, 0.788011, 0.777146, 0.766044, 0.754710,
  0.743145, 0.731354, 0.719340, 0.707107, 0.694658, 0.681998,
  0.669131, 0.656059, 0.642788, 0.629321, 0.615661, 0.601815,
  0.587785, 0.573576, 0.559193, 0.544639, 0.529919, 0.515038,
  0.500000, 0.484810, 0.469472, 0.453991, 0.438371, 0.422618,
  0.406737, 0.390731, 0.374606, 0.358368, 0.342020, 0.325568,
  0.309017, 0.292372, 0.275637, 0.258819, 0.241922, 0.224951,
  0.207912, 0.190809, 0.173648, 0.156434, 0.139173, 0.121869,
  0.104528, 0.087156, 0.069756, 0.052336, 0.034899, 0.017452,
  -0.000000, -0.017452, -0.034899, -0.052336, -0.069756, -0.087156,
  -0.104528, -0.121869, -0.139173, -0.156434, -0.173648, -0.190809,
  -0.207912, -0.224951, -0.241922, -0.258819, -0.275637, -0.292372,
  -0.309017, -0.325568, -0.342020, -0.358368, -0.374607, -0.390731,
  -0.406737, -0.422618, -0.438371, -0.453991, -0.469472, -0.484810,
  -0.500000, -0.515038, -0.529919, -0.544639, -0.559193, -0.573576,
  -0.587785, -0.601815, -0.615661, -0.629320, -0.642788, -0.656059,
  -0.669131, -0.681998, -0.694658, -0.707107, -0.719340, -0.731354,
  -0.743145, -0.754710, -0.766045, -0.777146, -0.788011, -0.798635,
  -0.809017, -0.819152, -0.829038, -0.838671, -0.848048, -0.857167,
  -0.866025, -0.874620, -0.882948, -0.891006, -0.898794, -0.906308,
  -0.913545, -0.920505, -0.927184, -0.933581, -0.939693, -0.945519,
  -0.951056, -0.956305, -0.961262, -0.965926, -0.970296, -0.974370,
  -0.978148, -0.981627, -0.984808, -0.987688, -0.990268, -0.992546,
  -0.994522, -0.996195, -0.997564, -0.998630, -0.999391, -0.999848,
  -1.000000, -0.999848, -0.999391, -0.998630, -0.997564, -0.996195,
  -0.994522, -0.992546, -0.990268, -0.987688, -0.984808, -0.981627,
  -0.978148, -0.974370, -0.970296, -0.965926, -0.961262, -0.956305,
  -0.951056, -0.945519, -0.939693, -0.933580, -0.927184, -0.920505,
  -0.913545, -0.906308, -0.898794, -0.891006, -0.882948, -0.874620,
  -0.866025, -0.857167, -0.848048, -0.838670, -0.829038, -0.819152,
  -0.809017, -0.798635, -0.788011, -0.777146, -0.766044, -0.754710,
  -0.743145, -0.731354, -0.719340, -0.707107, -0.694658, -0.681998,
  -0.669131, -0.656059, -0.642788, -0.629320, -0.615661, -0.601815,
  -0.587785, -0.573577, -0.559193, -0.544639, -0.529919, -0.515038,
  -0.500000, -0.484809, -0.469471, -0.453991, -0.438371, -0.422618,
  -0.406736, -0.390731, -0.374607, -0.358368, -0.342020, -0.325568,
  -0.309017, -0.292372, -0.275638, -0.258819, -0.241922, -0.224951,
  -0.207912, -0.190809, -0.173648, -0.156434, -0.139173, -0.121869,
  -0.104529, -0.087156, -0.069756, -0.052336, -0.034900, -0.017453
};
