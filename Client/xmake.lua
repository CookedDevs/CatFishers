-- Client target

target("Client")

    if is_plat("android") then
        set_kind("shared")
        set_basename("main")
        add_defines("_ANDROID_")
    else    
        set_kind("binary")
    end

    -- Recursively add all source files in Client/src
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")  -- Tracks headers in IDEs

    -- Include directories
    add_includedirs("src", {public=false})

    if is_plat("android") then
        add_includedirs(os.getenv("ANDROID_NDK_HOME") or os.getenv("NDK_ROOT") or "C:/Users/sasul/AppData/Local/Android/Sdk/ndk/29.0.13846066/sysroot/usr/include", {public=false})
    end

    -- Dependenciess
    add_packages("enet6", "nlohmann_json")
    add_deps("CatCore", "raylib")

    if not is_plat("android") then
        add_deps("commandline")
    else
        add_deps("raymoblib")
    end


    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", "NOMINMAX", "NOGDI")
        add_syslinks("gdi32", "winmm", "user32", "kernel32")
    elseif is_plat("linux") then
        add_syslinks("pthread", "dl", "m")
        add_ldflags("-Wl,--as-needed")
    elseif is_plat("android") then
        add_syslinks("m", "log", "android")

        after_build(function (target)
            import("core.project.config")
                -- detect ABI: prefer xmake-provided env var, fallback to common abi
                local abi = target:arch() or "arm64-v8a"
                -- path where xmake places the built shared lib
                local built_path = path.join(os.projectdir(), "build", "android", abi, config.get("mode"), "libmain.so")
                -- destination folder used by the package_apk task
                local dest_dir = path.join(os.projectdir(), "build_android", "lib", abi)
                local resources_dir = path.join(os.projectdir(), "build_android", "res", "drawable")
                
                print("\n>>> after_build: copying resources from Client/Resources/Images/*")
                for _, resource in ipairs(os.files("Client/Resources/Images/*")) do
                    print("    Copying " .. resource)
                    os.mkdir(resources_dir)
                    os.cp(resource, resources_dir)
                end

                print(">>> copied resources")

                print("\n>>> after_build: copying libmain.so")
                print("    built_path = " .. built_path)
                print("    dest_dir   = " .. dest_dir)

                -- ensure source exists
                if not os.isfile(built_path) then
                    raise("libmain.so not found at: " .. built_path .. "\nRun:\n  xmake f -p android --arch=" .. abi .. " && xmake")
                end
            
                -- make dest and copy
                os.mkdir(dest_dir)
                os.cp(built_path, dest_dir)
            
                print(">>> copied libmain.so -> " .. path.join(dest_dir, "libmain.so\n"))

        end)
end
before_build(function (target) 
    import("core.project.config")
    local abi = target:arch()
    local built_path = path.join(os.projectdir(), "build", config.get("plat"), abi, config.get("mode"))
    print("\n>>> after_build: copying resources")
    local resourcesLocation = path.join(os.projectdir(), "Client", "Resources")
    print(resourcesLocation)
    if os.isdir(resourcesLocation) then
        os.cp(resourcesLocation, built_path)
        print("Copied resources to built_path:", built_lib)
    end
    print(">>> copied resources")
end)
    
