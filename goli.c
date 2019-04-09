#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <emscripten/emscripten.h>

typedef unsigned char byte;

#define WIDTH 100
#define HEIGHT 75

static byte board[HEIGHT * WIDTH];

EMSCRIPTEN_KEEPALIVE
byte *goli_get_board(void) {
	return board;
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
	memset(board, 0, HEIGHT * WIDTH);
	board[0] = 1;
	board[10 * 10] = 1;
	board[11 * 10] = 1;
	board[12 * 10] = 1;
	board[13 * 10] = 1;
}

EMSCRIPTEN_KEEPALIVE
void goli_step(void/*byte *board, size_t h, size_t w*/) {
	size_t limit = WIDTH * HEIGHT;
	for (size_t i = 0; i < limit; i++) {
		board[i] = board[i] == 1 ? 0 : 1;
	}
}

int main() {
	printf("Running...\n");
}