const std = @import("std");
const mem = @import("std").mem;
const builtin = @import("builtin");

const ma = @cImport({
    @cInclude("miniaudio.h");
});

const sokol_app = @cImport({
    @cInclude("sokol_app.h");
});

const sokol_gfx = @cImport({
    @cInclude("sokol_gfx.h");
});

const sokol_glue = @cImport({
    @cInclude("sokol_glue.h");
});

const sokol_time = @cImport({
    @cInclude("sokol_time.h");
});

var sine_freq: f32 = 440.0;
const sample_rate: u32 = 44100;
var phase: f32 = 0.0;
const PI2: f32 = 6.28318530718;

fn audio_callback(pOutput: [*c]f32, frameCount: usize, _: ?*c_void) void {
    for (pOutput) |*sample| {
        *sample = std.math.sin(phase * PI2);
        phase += sine_freq / sample_rate;
        if (phase >= 1.0) phase -= 1.0;
    }
}

fn init_audio() void {
    var config = ma.device_config_init(ma.device_type_playback);
    config.playback.format = ma.format_f32;
    config.playback.channels = 1;
    config.sampleRate = sample_rate;
    config.dataCallback = audio_callback;

    var device = mem.zeroes(ma.device);
    if (ma.device_init(null, &config, &device) != ma.result_success) {
        std.debug.print("Failed to initialize playback device.\n", .{});
        return;
    }

    if (ma.device_start(&device) != ma.result_success) {
        std.debug.print("Failed to start playback device.\n", .{});
        ma.device_uninit(&device);
        return;
    }
}

fn init_cb() noreturn {
    sokol_gfx.sg_setup(&(sokol_gfx.sg_desc){});
    sokol_time.stm_setup();

    init_audio();
}

fn frame_cb() noreturn  {
    sokol_gfx.sg_begin_default_pass(&(sokol_gfx.sg_pass_action){.colors = .{sokol_gfx.sg_color_attachment_action{.action = sokol_gfx.SG_ACTION_CLEAR, .val = .{0.0, 0.0, 0.0, 1.0}}}}, sokol_app.sapp_width(), sokol_app.sapp_height());
    sokol_gfx.sg_end_pass();
    sokol_gfx.sg_commit();
}

fn cleanup_cb() noreturn {
    sokol_gfx.sg_shutdown();
}

fn event_cb(ev: *const sokol_app.sapp_event) noreturn {
    if (ev.type == sokol_app.SAPP_EVENTTYPE_MOUSE_SCROLL) {
        sine_freq += ev.scroll_y * 10.0;
        if (sine_freq < 20.0) sine_freq = 20.0;
        if (sine_freq > 20000.0) sine_freq = 20000.0;
    }
}

pub fn main() void {
    const desc = sokol_app.sapp_desc{
        .width = 640,
        .height = 480,
        .init_cb = init_cb,
        .frame_cb = frame_cb,
        .cleanup_cb = cleanup_cb,
        .event_cb = event_cb,
        .sample_count = 1,
        .swap_interval = 1,
        .window_title = "Sine Wave Generator",
        .icon = sokol_app.sapp_icon_desc{},
    };

    sokol_app.sapp_run(&desc);
}
