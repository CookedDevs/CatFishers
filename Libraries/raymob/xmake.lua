-- xmake.lua
add_rules("mode.debug", "mode.release")

set_project("raymoblib")
set_version("5.5.0")
set_languages("c11")

target("raymoblib")
    set_kind("shared")
    set_languages("c11")


    add_headerfiles("*.h");
    local srcs = {
        "helper.c",
        "sensor.c",
        "vibrator.c",
        "display.c",
        "soft_keyboard.c",
        "native_app_glue.c",
        "callback.c",
    }

    add_files(srcs)
    add_defines("PLATFORM_ANDROID")
    add_includedirs(".", {public = true})
    add_deps("raylib")

    after_build(function (target)
        import("core.project.config")
        -- detect ABI: prefer xmake-provided env var, fallback to common abi
        local abi = target:arch() or "arm64-v8a"
        -- path where xmake places the built shared lib
        local built_path = path.join(os.projectdir(), "build", "android", abi, "release", "libraymoblib.so")
            
        -- destination folder used by the package_apk task
        local dest_dir = path.join(os.projectdir(), "build_android", "lib", abi)
            
        print(">>> after_build: copying libraymoblib.so")
        print("    built_path = " .. built_path)
        print("    dest_dir   = " .. dest_dir)
            
        -- ensure source exists
        if not os.isfile(built_path) then
            raise("libraymoblib.so not found at: " .. built_path .. "\nRun:\n  xmake f -p android --arch=" .. abi .. " && xmake")
        end
    
        -- make dest and copy
        os.mkdir(dest_dir)
        os.cp(built_path, dest_dir)
    
        print(">>> copied libraymoblib.so -> " .. path.join(dest_dir, "libraymoblib.so"))
    end)