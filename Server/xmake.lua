-- Server target
target("Server")
    set_kind("binary")

    -- Recursively add all source files in src
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")

    if is_mode("debug") then
        set_symbols("debug")    -- enables debug symbols/pdb generation for supported toolchains
    end

    -- MSVC: ensure compiler / linker flags
    if is_mode("debug") and is_plat("windows") then
        add_cxflags("/Zi", {force = true})            -- generate debug info
        add_ldflags("/DEBUG", {force = true})         -- enable generating .pdb in linker
    end

    -- Include directories
    add_includedirs("src", {public=false})

    -- Dependencies
    add_deps("CatCore")
    add_packages("CatCore", "raylib", "enet6", "nlohmann_json")

    if not is_plat("android") then
        add_deps("commandline")
    end

    -- Android system libraries if needed
    if is_plat("windows") then
        add_syslinks("gdi32", "winmm", "user32", "kernel32")
    elseif is_plat("linux") then
        add_syslinks("pthread", "dl", "m")
        add_ldflags("-Wl,--as-needed")
    elseif is_plat("android") then
        add_syslinks("m", "log", "android")
    end
