#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <emscripten/emscripten.h>

/*
Worlds contains binary data, 1 if the cell is alive else 0.
We need 2 boards because updating the state of a cell in place would affect
its neighbors's state recomputations.
*/
static uint8_t *worlds[2]= {NULL};
static size_t world_width;
static size_t world_height;
static size_t world_size;
static size_t active_world = 0;

/*
Set world state to a line in the center
*/
EMSCRIPTEN_KEEPALIVE
void populate(void) {
	uint8_t *world = worlds[active_world];
	memset(world, 0, world_size * sizeof(uint8_t));
	memset(world + ((world_height / 2 - 1) * world_width), 1, world_width * sizeof(uint8_t));
	memset(world + (world_height / 2 * world_width), 1, world_width * sizeof(uint8_t));
	memset(world + ((world_height / 2 + 1) * world_width), 1, world_width * sizeof(uint8_t));
}

/*
Allocate worlds
*/
EMSCRIPTEN_KEEPALIVE
int goli_init_world(size_t w, size_t h) {
	world_height = h;
	world_width = w;
	world_size = w * h;
	worlds[0] = malloc(world_size * sizeof(uint8_t));
	worlds[1] = malloc(world_size * sizeof(uint8_t));
	if (worlds[0] == NULL || worlds[1] == NULL) {
		printf("ERROR: Out of memory\n");
		return 1;
	}
	populate();
	return 0;
}

/*
Release worlds
*/
EMSCRIPTEN_KEEPALIVE
void goli_reset_world() {
	free(worlds[0]);
	free(worlds[1]);
	world_width = 0;
	world_height = 0;
	world_size = 0;
	active_world = 0;
}

/*
Allocates a x * y * 4 RGB world image data buffer
*/
EMSCRIPTEN_KEEPALIVE
uint8_t *goli_get_image_buffer(void) {
	uint8_t *b = malloc(world_size * sizeof(uint8_t) * 4);
	size_t limit = world_size * 4;
	for (size_t i = 0; i < limit; i += 4) {
		b[i + 0] = 0;
		b[i + 1] = 0;
		b[i + 2] = 255;
		b[i + 3] = 255;
	}
	return b;
}

static void set_pixel(uint8_t *pixel, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;
	pixel[3] = a;
}

/*
Render the current world state on a world_size * 4 RGB image data buffer
*/
uint8_t *goli_render(uint8_t *img) {
	size_t limit = world_size * 4;
	uint8_t *world = worlds[active_world];
	size_t c = 0;
	for (size_t i = 0; i < limit; i += 4, c++) {
		if (world[c]) {
			set_pixel(img + i, 0, 125, 125, 255);
		} else {
			set_pixel(img + i, 0, 0, 0, 255);
		}
	}
	return img;
}

/* represents the 8 directions [y, x]*/
#define DIR_X 1
#define DIR_Y 0
static char directions[8][2] = {
	{-1, -1}, {-1, 0}, {-1, 1},
	{ 0, -1},          { 0, 1},
	{ 1, -1}, { 1, 0}, { 1, 1}
};

// #define WRAPS
static inline uint8_t is_cell_alive(uint8_t *world, int x, int y) {
#ifdef WRAPS
	return world[((y % world_width) * world_width + (x % HEIGHT))]; // TODO: Fixme
#else
	if (x < 0 || x >= world_width || y < 0 || y >= world_height) {
		return 0;
	}
	return world[y * world_width + x];
#endif
}

static size_t count_alive_neighbors(uint8_t *world, size_t i) {
	int x = i % world_width;
	int y = i / world_width;
	size_t count = 0;
	for (size_t d = 0; d < 8; d++) {
		char *direction = directions[d];
		if (is_cell_alive(world, x + direction[DIR_X], y + direction[DIR_Y])) {
			count++;
		}
	}
	return count;
}

static inline void update_alive_cell(uint8_t *world, uint8_t *next_world, size_t i, size_t neighbors){
	assert(world[i] == 1);
	if (neighbors < 2 || neighbors > 3) {
		next_world[i] = 0;
	} else {
		next_world[i] = 1;
	}
}

static inline void update_dead_cell(uint8_t *world, uint8_t *next_world, size_t i, size_t neighbors) {
	assert(world[i] == 0);
	if (neighbors == 3) {
		next_world[i] = 1;
	} else {
		next_world[i] = 0;
	}
}

/*
Given current world state, update the other board with the next and make it active
*/
EMSCRIPTEN_KEEPALIVE
void goli_update(void) {
	uint8_t *world = worlds[active_world];
	uint8_t *next_world = worlds[!active_world];

	for (size_t i = 0; i < world_size; i++) {
		size_t neighbors = count_alive_neighbors(world, i);
		if (world[i] == 1) {
			update_alive_cell(world, next_world, i, neighbors);
		} else {
			update_dead_cell(world, next_world, i, neighbors);
		}
	}
	active_world = !active_world;
}
