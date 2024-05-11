const std = @import("std");
const builtin = @import("builtin");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const cflags = [_][]const u8{
        "-std=c17",
        "-fno-sanitize=undefined",
    };

    const engine = try buildEngineSharedLib(b, target, optimize, &cflags);

    const game = try buildGameSharedLib(b, target, optimize, &cflags);
    game.linkLibrary(engine);
    game.step.dependOn(&engine.step);

    const exe = try buildDaisExecutable(b, target, optimize, &cflags);
    exe.linkLibrary(engine);
    exe.step.dependOn(&game.step);

    addRunCommand(b, exe);
}

fn buildEngineSharedLib(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    cflags: []const []const u8,
) !*std.Build.Step.Compile {
    const engine = b.addSharedLibrary(.{
        .name = "engine",
        .target = target,
        .optimize = optimize,
    });

    const engine_source_files = try getSourceFiles(b, .{
        .paths = &.{"engine/src"},
        .allowed_exts = &.{".c"},
    });
    engine.addCSourceFiles(.{ .files = engine_source_files.items, .flags = cflags });

    engine.addIncludePath(b.path("engine/src"));
    engine.addIncludePath(try getVulkanSDKIncludePath(b));

    engine.defineCMacro("EXPORT", null);

    if (optimize == .Debug) {
        engine.defineCMacro("_DEBUG", null);
    }

    switch (builtin.os.tag) {
        .windows => {
            engine.defineCMacro("OS_WINDOWS", null);
            engine.defineCMacro("UNICODE", null);
            engine.defineCMacro("NOMINMAX", null);
            engine.defineCMacro("WIN32_LEAN_AND_MEAN", null);
            engine.defineCMacro("_CRT_SECURE_NO_WARNINGS", null);
            engine.defineCMacro("VK_USE_PLATFORM_WIN32_KHR", null);

            engine.linkLibC();
        },
        .linux => {
            engine.defineCMacro("OS_LINUX", null);
            engine.defineCMacro("VK_USE_PLATFORM_WAYLAND_KHR", null);

            engine.linkLibC();
        },
        .macos => {
            engine.defineCMacro("OS_MACOS", null);
            engine.defineCMacro("VK_USE_PLATFORM_METAL_EXT", null);

            engine.addLibraryPath(b.path("lib/arm64"));
            engine.linkFramework("Foundation");
        },
        else => unreachable,
    }

    b.installArtifact(engine);

    return engine;
}

fn buildGameSharedLib(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    cflags: []const []const u8,
) !*std.Build.Step.Compile {
    const game = b.addSharedLibrary(.{
        .name = "game",
        .target = target,
        .optimize = optimize,
    });

    const game_source_files = try getSourceFiles(b, .{
        .paths = &.{"game/src"},
        .allowed_exts = &.{".c"},
    });
    game.addCSourceFiles(.{ .files = game_source_files.items, .flags = cflags });

    game.addIncludePath(b.path("game/src"));
    game.addIncludePath(b.path("engine/src"));

    game.defineCMacro("EXPORT", null);

    if (optimize == .Debug) {
        game.defineCMacro("_DEBUG", null);
    }

    switch (builtin.os.tag) {
        .windows => {
            game.defineCMacro("OS_WINDOWS", null);
            game.defineCMacro("UNICODE", null);
            game.defineCMacro("NOMINMAX", null);
            game.defineCMacro("WIN32_LEAN_AND_MEAN", null);
            game.defineCMacro("_CRT_SECURE_NO_WARNINGS", null);

            game.linkLibC();
        },
        .linux => {
            game.defineCMacro("OS_LINUX", null);

            game.linkLibC();
        },
        .macos => {
            game.defineCMacro("OS_MACOS", null);

            game.addLibraryPath(b.path("lib/arm64"));
            game.linkFramework("Foundation");
        },
        else => unreachable,
    }

    b.installArtifact(game);

    return game;
}

fn buildDaisExecutable(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    cflags: []const []const u8,
) !*std.Build.Step.Compile {
    const exe = b.addExecutable(.{
        .name = "dais",
        .target = target,
        .optimize = optimize,
    });

    const exe_source_files = try getSourceFiles(b, .{
        .paths = &.{"dais/src"},
        .allowed_exts = &.{".c"},
    });
    exe.addCSourceFiles(.{ .files = exe_source_files.items, .flags = cflags });

    exe.addIncludePath(b.path("dais/src"));
    exe.addIncludePath(b.path("engine/src"));

    exe.defineCMacro("IMPORT", null);

    if (optimize == .Debug) {
        exe.defineCMacro("_DEBUG", null);
    }

    switch (builtin.os.tag) {
        .windows => {
            exe.defineCMacro("OS_WINDOWS", null);
            exe.defineCMacro("UNICODE", null);
            exe.defineCMacro("NOMINMAX", null);
            exe.defineCMacro("WIN32_LEAN_AND_MEAN", null);
            exe.defineCMacro("_CRT_SECURE_NO_WARNINGS", null);

            exe.linkLibC();
            exe.linkSystemLibrary("user32");
        },
        .linux => {
            exe.defineCMacro("OS_LINUX", null);

            exe.linkLibC();
        },
        .macos => {
            exe.defineCMacro("OS_MACOS", null);

            exe.addLibraryPath(b.path("lib/arm64"));
            exe.linkFramework("Foundation");
        },
        else => unreachable,
    }

    b.installArtifact(exe);

    return exe;
}

fn addRunCommand(b: *std.Build, exe: *std.Build.Step.Compile) void {
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}

fn getVulkanSDKIncludePath(b: *std.Build) !std.Build.LazyPath {
    const vulkan_sdk = try std.process.getEnvVarOwned(b.allocator, "VULKAN_SDK");
    const path = try std.fmt.allocPrintZ(b.allocator, "{s}/Include", .{vulkan_sdk});

    return .{ .cwd_relative = path };
}

fn getSourceFiles(
    b: *std.Build,
    options: struct {
        paths: []const []const u8,
        allowed_exts: []const []const u8,
    },
) !std.ArrayList([]const u8) {
    var sources = std.ArrayList([]const u8).init(b.allocator);

    for (options.paths) |path| {
        var dir = try std.fs.cwd().openDir(path, .{ .iterate = true });

        var walker = try dir.walk(b.allocator);
        defer walker.deinit();

        while (try walker.next()) |entry| {
            const ext = std.fs.path.extension(entry.basename);

            const include_file = for (options.allowed_exts) |e| {
                if (std.mem.eql(u8, ext, e)) {
                    break true;
                }
            } else false;

            if (include_file) {
                const source_path = try std.fmt.allocPrintZ(b.allocator, "{s}/{s}", .{ path, entry.path });
                try sources.append(source_path);
            }
        }
    }

    return sources;
}
