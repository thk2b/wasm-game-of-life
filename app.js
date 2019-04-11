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

function render(ctx, world) {
    console.log(_goli_render(world.data_offset));
    console.log(world.data);
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

function main() {
    _goli_init_world(1000, 1000);
    const [w, h] = [1000, 1000];
    const data_offset = _goli_get_image_buffer();
    const world = {
        w, h,
        data_offset,
        data: new Uint8ClampedArray(
            Module.buffer, data_offset,
            w * h * 4
        )
    };
    console.log(world.data);
    set_canvas_size(elements.canvas, world);
    const ctx = elements.canvas.getContext("2d");

    elements.controls.step_button.addEventListener("click", () => {
        step(ctx, world, performance.now());
    });

    elements.controls.run_button.addEventListener("click", () => {
        state.running = !state.running;
        if (state.running) {
            loop(state, ctx, world);
        }
    });
}

Module.postRun.push(main);

function time(el, cb) {
    const begin = performance.now();
    cb();
    const ms = performance.now() - begin;
    el.innerHTML = ms.toString();
}
