-- Server target
target("Server")
    set_kind("binary")

    -- Recursively add all source files in src
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")

    -- Include directories
    add_includedirs("src", {public=false})
    --add_includedirs("CatCore/include", {public=false})  -- adjust if CatCore headers are elsewhere

    -- Dependencies
    add_deps("CatCore")
    add_packages("raylib", "enet6")

    -- Android system libraries if needed
    if is_plat("windows") then
        add_syslinks("gdi32", "winmm", "user32", "kernel32")
        set_group("Server")  -- top-level group in Visual Studio
    elseif is_plat("linux") then
        add_syslinks("pthread", "dl", "m")
        add_ldflags("-Wl,--as-needed")
    elseif is_plat("android") then
        add_syslinks("m", "log", "android")
    end
