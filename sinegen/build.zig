const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const mode = b.standardReleaseOptions();
    const target = b.standardTargetOptions(.{});

    const exe = b.addExecutable(.{
        .name = "sine_wave_generator",
        .root_source_file = .{ .source_file = "src/main.zig" },
        .target = target,
        .optimize = mode,
    });

    exe.setBuildMode(mode);
    exe.linkLibC();
    exe.addIncludeDir("libs/miniaudio");
    exe.addIncludeDir("libs/sokol");
    exe.linkSystemLibrary("m"); // Link the math library

    exe.install();
}
