const style = {
    cell: {
        size: 10,
        alive: 'green',
        dead: 'blue'
    }
};

const elements = {
    controls: {
        step_button: document.getElementById("controls_step"),
        run_button: document.getElementById("controls_run"),
    },
    canvas: document.getElementById("canvas"),
};

const state = {
    running: false,
    fps: 15
};

function render(ctx, board, timestamp) {
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
    console.log('render took', Date.now() - timestamp);
}

function time(msg, cb) {
    const begin = Date.now();
    cb();
    console.log(msg, Date.now() - begin);
}

function step(ctx, board, timestamp) {
    time('step took:', () => _goli_step(board.data, board.x, board.y));
    render(ctx, board, timestamp);
}

function loop(state, ctx, board) {
    if (state.running) {
        window.requestAnimationFrame((timestamp) => {
            step(ctx, board, timestamp);
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
    console.log(board);
    set_canvas_size(elements.canvas, board);
    const ctx = elements.canvas.getContext("2d");

    render(ctx, board, Date.now());
    elements.controls.step_button.addEventListener("click", () => {
        board.data = new Uint8Array(Module.buffer, _goli_get_board())
        step(ctx, board, Date.now());
    });

    elements.controls.run_button.addEventListener("click", () => {
        state.running = !state.running;
        if (state.running) {
            loop(state, ctx, board);
        }
    });
}

Module.postRun.push(main);