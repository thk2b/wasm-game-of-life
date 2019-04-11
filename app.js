const style = {
    cell: {
        size: 1, // TODO: handle different cell sizes
    }
};

const elements = {
    controls: {
        step_button: document.getElementById("controls_step"),
        run_button: document.getElementById("controls_run"),
        restart_button: document.getElementById("controls_restart"),
        width_input: document.getElementById("controls_width"),
        height_input: document.getElementById("controls_height"),
    },
    canvas: document.getElementById("canvas"),
    metrics: {
        render: document.getElementById("metrics_render"),
        update: document.getElementById("metrics_update"),
        total: document.getElementById("metrics_total"),
    }
};



function render(ctx, world) {
    _goli_render(world.data_offset);
    ctx.putImageData(new ImageData(world.data, world.w, world.h), 0, 0);
}

function step(ctx, world) {
    time(elements.metrics.render, () => render(ctx, world));
    time(elements.metrics.update, () => _goli_update(world.data, world.x, world.y));
}

function loop(state, ctx, world) {
    if (state.running) {
        window.requestAnimationFrame((timestamp) => {
            time(elements.metrics.total, () => step(ctx, world, timestamp));
            loop(state, ctx, world);
        })
    }
}

function set_canvas_size(cvs, world) {
    cvs.width = world.w * style.cell.size;
    cvs.height = world.h * style.cell.size;
}

function init(w, h) {
    _goli_init_world(w, h);
    const data_offset = _goli_get_image_buffer();
    const world = {
        w, h,
        data_offset,
        data: new Uint8ClampedArray(
            Module.buffer, data_offset,
            w * h * 4
        )
    };
    const state = {
        running: false,
        fps: 1 // TODO: limit FPS
    };
    set_canvas_size(elements.canvas, world);
    const ctx = elements.canvas.getContext("2d");
    render(ctx, world);

    const on_step_click = () => {
        step(ctx, world, performance.now());
    };
    elements.controls.step_button.addEventListener("click", on_step_click);
    
    const on_run_click = () => {
        state.running = !state.running;
        if (state.running) {
            loop(state, ctx, world);
        }
    };
    elements.controls.run_button.addEventListener("click", on_run_click);
    return () => {
        state.running = false;
        elements.controls.step_button.removeEventListener("click", on_step_click);
        elements.controls.run_button.removeEventListener("click", on_run_click);
        _goli_reset_world();
        _goli_free_image_buffer(world.data_offset);
    }
}

function main() {
    const size = window.innerWidth < window.innerHeight ? window.innerWidth : window.innerHeight -200;
    elements.controls.width_input.value = size;
    elements.controls.height_input.value = size;
    let reset = init(size, size);
    
    elements.controls.restart_button.addEventListener("click", () => {
        reset();
        reset = init(
            elements.controls.width_input.value,
            elements.controls.height_input.value);
    })
}

Module.postRun.push(main);

function time(el, cb) {
    const begin = performance.now();
    cb();
    const ms = performance.now() - begin;
    el.innerHTML = ms.toString();
}
