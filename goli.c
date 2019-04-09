#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <emscripten/emscripten.h>

typedef unsigned char byte;

#define WIDTH 50
#define HEIGHT 50
#define BOARD_SIZE (WIDTH * HEIGHT)

static byte boards[2][BOARD_SIZE];
static size_t active_board_idx = 0;

EMSCRIPTEN_KEEPALIVE
byte *goli_get_board(void) {
	return boards[active_board_idx];
}

EMSCRIPTEN_KEEPALIVE
size_t goli_get_width(void) {
	return WIDTH;
}

EMSCRIPTEN_KEEPALIVE
size_t goli_get_height(void) {
	return HEIGHT;
}

EMSCRIPTEN_KEEPALIVE
void goli_init(void) {
	memset(boards, 0, BOARD_SIZE * 2);
	memset(boards[active_board_idx] + (HEIGHT / 2 - 1) * WIDTH, 1, WIDTH);
	memset(boards[active_board_idx] + (HEIGHT / 2 + 1) * WIDTH, 1, WIDTH);
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
static inline byte is_cell_alive(byte *board, int x, int y) {
#ifdef WRAPS
	return board[((y % WIDTH) * WIDTH + (x % HEIGHT))]; // TODO: Fixme
#else
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
		return 0;
	}
	return board[y * WIDTH + x];
#endif
}

static size_t count_alive_neighbors(byte *board, size_t i) {
	int x = i % WIDTH;
	int y = i / WIDTH;
	size_t count = 0;
	for (size_t d = 0; d < 8; d++) {
		char *direction = directions[d];
		if (is_cell_alive(board, x + direction[DIR_X], y + direction[DIR_Y])) {
			count++;
		}
	}
	return count;
}

static inline void update_alive_cell(byte *board, byte *next_board, size_t i, size_t neighbors){
	assert(board[i] == 1);
	if (neighbors < 2 || neighbors > 3) {
		next_board[i] = 0;
	} else {
		next_board[i] = 1;
	}
}

static inline void update_dead_cell(byte *board, byte *next_board, size_t i, size_t neighbors) {
	assert(board[i] == 0);
	if (neighbors == 3) {
		next_board[i] = 1;
	} else {
		next_board[i] = 0;
	}
}

EMSCRIPTEN_KEEPALIVE
void goli_step(void) {
	byte *board = boards[active_board_idx];
	byte *next_board = boards[!active_board_idx];

	for (size_t i = 0; i < BOARD_SIZE; i++) {
		size_t neighbors = count_alive_neighbors(board, i);
		if (board[i] == 1) {
			update_alive_cell(board, next_board, i, neighbors);
		} else {
			update_dead_cell(board, next_board, i, neighbors);
		}
	}
	active_board_idx = !active_board_idx;
}

int main() {
	printf("Running...\n");
}