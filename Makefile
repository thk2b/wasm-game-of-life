build:
	emcc goli.c -s WASM=1 -O2 \
	-s EXPORTED_FUNCTIONS="['_goli_get_board', '_goli_init', '_goli_step', '_goli_get_width', '_goli_get_height']" \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
	-o index.js

serve:
	python3 -m http.server 8080