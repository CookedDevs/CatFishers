-- Define the project
set_project("commandline")
set_version("1.0.0")

-- Set C++ standard
set_languages("c++17")

-- Add compiler flags for UNIX and MSVC
if is_plat("linux", "cygwin", "android") then
    add_cxxflags("-Wall", "-Wextra", "-pedantic")
elseif is_plat("windows") then
    add_cxxflags("/permissive-")
end

-- Set up platform-specific definitions
if is_plat("windows") then
    add_defines("COMMANDLINE_PLATFORM_WINDOWS=1")
    set_group("External")
elseif is_plat("linux", "apple", "unix") then
    add_defines("COMMANDLINE_PLATFORM_LINUX=1")
end

-- Set up the library source files
add_headerfiles("*.h")
add_files
(
    "src/windows_impl.cpp", 
    "src/linux_impl.cpp", 
    "src/backends/InteractiveBackend.cpp",
    "src/backends/Backend.cpp",
    "src/commandline.cpp", 
    "src/backends/BufferedBackend.cpp",
    "src/helper/ansi.cpp"
)

-- Define the library
target("commandline")
    set_kind("static") -- Change to "shared" if you want a shared library
    add_defines("DOCTEST_CONFIG_DISABLE=1") -- Equivalent to target_compile_definitions(commandline PRIVATE -DDOCTEST_CONFIG_DISABLE=1)
    add_includedirs("src", {public = true})
    add_includedirs("tests")
    add_packages("threads") -- Linking to Threads library

    if is_plat("windows") then
        add_defines("PLATFORM_WINDOWS=1")
    elseif is_plat("linux") then
        add_defines("PLATFORM_LINUX=1")
    end
-- Optional: Build examples if enabled
option("BUILD_EXAMPLES", {default = false, description = "Build example program"})
if has_config("BUILD_EXAMPLES") then
    target("commandline_test")
        set_kind("binary")
        add_files("src/main.cpp")
        add_deps("commandline") -- Link commandline library
        if is_plat("windows") then
            add_defines("PLATFORM_WINDOWS=1")
        elseif is_plat("linux") then
            add_defines("PLATFORM_LINUX=1")
    end
end

-- Optional: Build tests if enabled
option("COMMANDLINE_TESTS", {default = false, description = "Build tests"})
if has_config("COMMANDLINE_TESTS") then
    target("commandline_tests")
        set_kind("binary")
        add_files("tests/tests.cpp")
        add_deps("commandline") -- Link commandline library
        add_includedirs("src", "tests")
        if is_plat("windows") then
            add_defines("PLATFORM_WINDOWS=1")
        elseif is_plat("linux") then
            add_defines("PLATFORM_LINUX=1")
    end
end