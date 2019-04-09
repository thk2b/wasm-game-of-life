# Conway's game of life

## Structure

-   goli.c

    implementation of the game state logic. Exposes the following functions:

    -   goli_step()

    -   goli_get_state()

    -   goli_toggle_cell()

-   index.html

    browser entry point. Fetches the wasm app and wires it to the web page.

-   script.js script.wasm

    the emscripten generated wasm binary and javascript runtime.