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
    console.log("render: ", timestamp);
    for (let y = 0; y < board.h; y++) {
        for (let x = 0; x < board.w; x++) {
            if (board.data[y * board.h + x] == 1) {
                ctx.fillStyle = style.cell.alive;
            } else {
                ctx.fillStyle = style.cell.dead;
            }
            ctx.fillRect(x * style.cell.size, y * style.cell.size, style.cell.size, style.cell.size);
        }
    }
}

function step(ctx, board, timestamp) {
    render(ctx, board, timestamp);
    _goli_step(board.data, board.x, board.y);
}

function loop(state, ctx, board) {
    if (state.running) {
        window.requestAnimationFrame((timestamp) => {
            step(ctx, board, timestamp);
            loop(state, ctx, board);
        })
    }
}

function main() {
    _goli_init();
    const board = {
        data: new Uint8Array(Module.buffer, _goli_get_board()),
        w: _goli_get_width(),
        h: _goli_get_height(),
    };
    
    const ctx = elements.canvas.getContext("2d");
    // TODO: init canvas size

    elements.controls.step_button.addEventListener("click", () => {
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