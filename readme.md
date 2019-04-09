# Conway's game of life

## Rules

According to [Wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), the rules for Conway's game of life are:

- Any live cell with fewer than two live neighbours dies, as if by underpopulation.

- Any live cell with two or three live neighbours lives on to the next generation.

- Any live cell with more than three live neighbours dies, as if by overpopulation.

- Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

## Building the project

```
source ~/emsdk/emsdk_env.sh
make build
make serve
```

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