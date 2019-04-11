build:
	emcc goli.c -s WASM=1 -O3 \
	-s EXPORTED_FUNCTIONS="['_goli_init_world', '_goli_reset_world', '_goli_get_image_buffer', '_goli_render', '_goli_update', '_goli_free_image_buffer']" \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
	-s TOTAL_MEMORY=50MB \
	-o index.js

serve:
	python3 -m http.server 8080