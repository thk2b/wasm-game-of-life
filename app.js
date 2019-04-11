const style = {
    cell: {
        size: 1,
        alive: [0, 100, 0, 255],
        dead: [255, 255, 255, 255],
    }
};

const elements = {
    controls: {
        step_button: document.getElementById("controls_step"),
        run_button: document.getElementById("controls_run"),
    },
    canvas: document.getElementById("canvas"),
    metrics: {
        render: document.getElementById("metrics_render"),
        update: document.getElementById("metrics_update"),
        total: document.getElementById("metrics_total"),
    }
};

const state = {
    running: false,
    // fps: 1 TODO: limit FPS
};

function render(ctx, board) {
    const img_size = board.h * board.w * 4;
    const img = ctx.getImageData(0, 0, board.h, board.w);
    const data = img.data;
    let cell = 0;
    for (let i = 0; i < img_size; i += 4) {
        const rgb = style.cell[board.data[cell] ? 'alive' : 'dead'];
        data[i + 0] = rgb[0];
        data[i + 1] = rgb[1];
        data[i + 2] = rgb[2];
        data[i + 3] = rgb[3];
        cell++;
    }
    ctx.putImageData(img, 0, 0);
}

function render1(ctx, board) {
    for (let y = 0; y < board.h; y++) {
        for (let x = 0; x < board.w; x++) {
            if (board.data[y * board.w + x]) {
                ctx.fillStyle = style.cell.alive;
            } else {
                ctx.fillStyle = style.cell.dead;
            }
            ctx.fillRect(x * style.cell.size, y * style.cell.size, style.cell.size, style.cell.size);
        }
    }
}

function time(el, cb) {
    const begin = performance.now();
    cb();
    const ms = performance.now() - begin;
    el.innerHTML = ms.toString();
}

function step(ctx, board, timestamp) {
    time(elements.metrics.update, () => _goli_step(board.data, board.x, board.y));
    time(elements.metrics.render, () => render(ctx, board));
}

function loop(state, ctx, board) {
    if (state.running) {
        window.requestAnimationFrame((timestamp) => {
            time(elements.metrics.total, () => step(ctx, board, timestamp));
            loop(state, ctx, board);
        })
    }
}

function set_canvas_size(cvs, board) {
    cvs.width = board.w * style.cell.size;
    cvs.height = board.h * style.cell.size;
}

function main() {
    _goli_init();
    const board = {
        data: new Uint8Array(Module.buffer, _goli_get_board()),
        w: _goli_get_width(),
        h: _goli_get_height(),
    };
    set_canvas_size(elements.canvas, board);
    const ctx = elements.canvas.getContext("2d");

    // render(ctx, board);
    const imgData = ctx.getImageData(0, 0, board.w, board.h);
    const data = new Uint8ClampedArray(Module.buffer, Module._goli_alloc_board(), board.w * board.h * 4);
    // console.log(_goli_render(data, data.length));
    // imgData.data = data;
    console.log(data);
    ctx.putImageData(new ImageData(data, board.w, board.h), 0, 0);
    elements.controls.step_button.addEventListener("click", () => {
        board.data = new Uint8Array(Module.buffer, _goli_get_board())
        step(ctx, board, performance.now());
    });

    elements.controls.run_button.addEventListener("click", () => {
        state.running = !state.running;
        if (state.running) {
            loop(state, ctx, board);
        }
    });
}

Module.postRun.push(main);