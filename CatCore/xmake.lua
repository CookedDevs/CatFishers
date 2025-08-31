-- CatCore library
target("CatCore")
    set_kind("static")

    -- Recursively add all source files in CatCore/src
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")

    -- Include directories
    add_includedirs("src", {public=true})

    -- Dependencies
    add_packages("enet6", "nlohmann_json")
