-- CatCore library
target("CatCore")
    set_kind("static")

    -- Recursively add all source files in CatCore/src
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")  -- Tracks headers in IDEs

    -- Include directories
    --add_includedirs("CatCore/include", {public=true})  -- public so clients can include

    -- Dependencies
    add_packages("raylib", "enet6")
