# wasm-game-of-life
[Conway's game of life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), implemented in c running on the web via web assembly.

The aim of this project is to explore the capabilities of web assembly. It uses the emscripten SDK to compile C source code into wasm, which is then loaded and run by a small javascript app.

## Architecture

The C source code does the heavy lifting: `goli_update` computes the next state of the world, and `goli_render` sets pixels in the canvas image buffer based on the world state.

The JavaScript app handles the app lifecycle and user interaction. It is responsible for initializing the worlds and the image buffer. It handles user input and runs the main loop, which calls the C functions. The render function, on the javascript side, calls `ctx.putImageData` with the updated image buffer to update the canvas.

Overall, this demonstrates sharing memory between JavaScript and wasm: the wasm updates the image data buffer, which is seamlessly put to the canvas.

## Performance

The simulation is very performant, running at around 30FPS with a 2000 X 2000 world size on Chrome 73.

Further performance optimizations are possible.

## Building

To build, install and activate the emscripten SDK

Then run `make build`, `make serve`, and navigate to `localhost:8080`.
