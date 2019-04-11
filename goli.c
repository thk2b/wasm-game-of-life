#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <emscripten/emscripten.h>

static uint8_t *worlds[2];
static size_t world_width;
static size_t world_height;
static size_t world_size;
static size_t active_world = 0;

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
}

/*
Allocates a x * y * 4 RGB world image data buffer
*/
EMSCRIPTEN_KEEPALIVE
uint8_t *goli_get_image_buffer(void) {
	uint8_t *b = malloc(world_size * sizeof(uint8_t) * 4);
	memset(b, 0, world_size * sizeof(uint8_t) * 4);
	return b;
}

/*
Render the current world state on a world_size * 4 RGB image data buffer
*/
void goli_render(uint8_t *img) {
	memset(img, 0, world_size * sizeof(uint8_t) * 4);
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

int main() {
	printf("Running...\n");
}