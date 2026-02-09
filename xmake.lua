set_project("elx")
set_version("0.1.0")

add_rules("mode.debug", "mode.release")
-- set_defaultmode("debug")
-- set_toolchains("clang")

target("elx")
set_kind("binary")
add_files("src/*.c")
set_languages("c99")

set_warnings("all", "extra", "pedantic")
-- set_policy("build.sanitizer.address", true)
-- set_policy("build.sanitizer.thread", true)
-- set_policy("build.sanitizer.undefined", true)
-- set_policy("build.sanitizer.memory", true)
-- set_policy("build.sanitizer.leak", true)

add_cflags(
	"-Wshadow",
	"-Wconversion",
	"-Wformat=2",
	"-Wnull-dereference",
	"-Wdouble-promotion",
	"-Wundef",
	"-rdynamic",
	"-g",
	"-no-pie",
	"-fno-omit-frame-pointer"
)

if is_mode("debug") then
	add_ldflags("-rdynamic", "-no-pie")
end
