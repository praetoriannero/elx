set_project("elx")
set_version("0.1.0")
add_rules("plugin.compile_commands.autoupdate")

add_requires("glib")
-- add_requires("libunwind")
-- add_requires("libbacktrace")
add_rules("mode.debug", "mode.release")
set_defaultmode("debug")
-- set_toolchains("clang")

target("elx")
set_kind("binary")
add_files("src/**.c")
set_languages("c23")
add_packages("glib")

set_warnings("all", "extra", "pedantic")

add_cflags(
	"-Wshadow",
	"-Wconversion",
	"-Wformat=2",
	"-Wnull-dereference",
	"-Wdouble-promotion",
	"-Wundef",
	"-rdynamic",
	"-fsanitize=address",
	"-g",
	"-std=c23",
	-- "-no-pie",
	"-fno-omit-frame-pointer"
)

if is_mode("debug") then
	add_ldflags("-rdynamic", "-no-pie", "-fsanitize=address")
end
