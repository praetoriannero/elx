set_project("elx")
set_version("0.1.0")
add_rules("plugin.compile_commands.autoupdate")

add_requires("unity_test","libbacktrace")
add_rules("mode.debug", "mode.release")
set_defaultmode("debug")

set_policy("check.auto_ignore_flags", false)

target("libelx")
	set_basename("elx")
	set_languages("c23")
	set_kind("shared")
	add_headerfiles("src/**.h")
	add_files("src/**.c|main.c")
	add_packages("unity_test", "libbacktrace")
	add_includedirs("src/", {public = true})

target("elx")
	set_kind("binary")
	add_cflags("-fPIE")
	add_deps("libelx")
	add_files("src/main.c")
	set_languages("c23")
	add_packages("unity_test", "libbacktrace")

for _, file in ipairs(os.files("tests/test_*.c")) do
	local test_name = path.basename(file)
	target(test_name)
		add_files(file)
		add_deps("libelx")
		set_languages("c23")
    add_cflags("-fPIE")
    -- set_symbols("debug")
		set_kind("binary")
		set_default("false")
		add_packages("unity_test", "libbacktrace")
		add_tests("default")
end

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
 	"-fPIE",
 	-- "-no-pie",
 	"-fno-omit-frame-pointer"
 )

 if is_mode("debug") then
 	add_ldflags("-rdynamic", "-fsanitize=address")
 end

	add_ldflags("-rdynamic", "-fsanitize=address")
