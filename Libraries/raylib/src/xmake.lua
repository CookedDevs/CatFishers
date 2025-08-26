-- Libraries/raylib/src/xmake.lua
-- Robust version: no is_enabled / is_kind calls so it works in any include order.

-- Decide shared vs static:
-- 1) XMake config: xmake f --raylib_shared=y
-- 2) Environment variable: RAYLIB_SHARED=1
local cfg_shared = get_config("raylib_shared")
local env_shared = os.getenv("RAYLIB_SHARED")

local build_shared = false
if cfg_shared then
    local v = tostring(cfg_shared):lower()
    if v == "y" or v == "yes" or v == "true" then
        build_shared = true
    end
end
if not build_shared and env_shared then
    local v = tostring(env_shared):lower()
    if v == "1" or v == "true" then
        build_shared = true
    end
end

target("raylib")
    -- choose kind
    if build_shared or is_plat("android") then
        set_kind("shared")
        add_defines("PLATFORM_ANDROID")
        add_defines("GRAPHICS_API_OPENGL_ES2")
    else
        set_kind("static")
    end

    set_languages("c11")

    -- Public headers (adjust if your headers live elsewhere)
    add_headerfiles("*.h")

    local src_files = {"raudio.c", "rmodels.c", "rshapes.c", "rtext.c", "rtextures.c", "utils.c", "rcore.c"}
    
    if not is_plat("android") then
        table.insert(src_files, "rglfw.c")
        add_includedirs("external/glfw/include", {public = true})
    end

    if not is_plat("linux") then
        add_includedirs("external", {public = true})
    end

    add_files(src_files)

    -- Public include dir so consumers can #include <raylib.h>
    add_includedirs(".", {public = true})

    
    

    -- Add PIC if building shared or if top-level config requested PIC
    local pic_cfg = get_config("pic")
    local want_pic = false
    if pic_cfg then
        local v = tostring(pic_cfg):lower()
        if v == "y" or v == "yes" or v == "true" then
            want_pic = true
        end
    end
    if build_shared or want_pic then
        if is_plat("linux") or is_plat("android") or is_plat("macosx") or is_plat("iphoneos") then
            add_cxflags("-fPIC")
        end
    end
    
    -- Platform-specific system libs / flags (extend if needed)
    if is_plat("windows") then
        add_defines("PLATFORM_DESKTOP_GLFW")
        add_syslinks("winmm", "gdi32", "user32", "kernel32", "shell32", "opengl32")
    elseif is_plat("linux") then
        add_syslinks("pthread", "dl", "m")
        add_defines("PLATFORM_DESKTOP_GLFW","_GLFW_X11","PLATFORM_LINUX", "__linux__", "GLFW_ENABLE_X11")
        add_ldflags("-Wl,--as-needed")
    elseif is_plat("macosx") or is_plat("iphoneos") then
        add_frameworks("Cocoa", "OpenGL", "CoreAudio")
    elseif is_plat("android") then
    
        on_load(function (target)
            -- Basic defines
            target:add("defines", "PLATFORM_ANDROID")
            target:add("defines", "EGL_NO_X11")
        
            -- Make EGLAPIENTRY empty so egl typedefs parse correctly:
            target:add("cxflags", "-DEGLAPIENTRY=")
        
            -- Try to detect Android NDK root from common env vars
            local ndk_root = os.getenv("ANDROID_NDK")
                        or os.getenv("ANDROID_NDK_ROOT")
                        or os.getenv("ANDROID_NDK_HOME")
                        or (os.getenv("ANDROID_HOME") and path.join(os.getenv("ANDROID_HOME"), "ndk"))
                        or os.getenv("ANDROID_SDK_ROOT") and path.join(os.getenv("ANDROID_SDK_ROOT"), "ndk")
        
            -- If ndk_root points to /ndk (folder containing versions), prefer explicit installed version
            if ndk_root and os.isdir(ndk_root) and not path.filename(ndk_root):match("%d") then
                -- try to pick the newest numeric subfolder (e.g. 29.0.13846066)
                for _, sub in ipairs(os.dirs(path.join(ndk_root, "*"))) do
                    if os.isdir(sub) then
                        ndk_root = sub
                        break
                    end
                end
            end
        
            if ndk_root and os.isdir(ndk_root) then
                -- native_app_glue location
                local nag = path.join(ndk_root, "sources", "android", "native_app_glue")
                if os.isdir(nag) then
                    -- Add header include so '<android_native_app_glue.h>' resolves
                    target:add("sysincludedirs", nag)
                
                    -- Add the C glue file so it's compiled into the library
                    local glue_c = path.join(nag, "android_native_app_glue.c")
                    if os.isfile(glue_c) then
                        target:add("files", glue_c)
                    end
                else
                    -- fallback: some older NDK layouts:
                    local alt = path.join(ndk_root, "platforms", "android-21", "arch-arm", "usr", "include")
                    if os.isdir(alt) then
                        target:add("sysincludedirs", alt)
                    end
                end
            
                -- Also prefer NDK sysroot includes (helps pick right EGL/GL headers)
                local sysroot_inc = path.join(ndk_root, "toolchains", "llvm", "prebuilt", "windows-x86_64", "sysroot", "usr", "include")
                if not os.isdir(sysroot_inc) then
                    sysroot_inc = path.join(ndk_root, "sysroot", "usr", "include")
                end
                if os.isdir(sysroot_inc) then
                    target:add("sysincludedirs", sysroot_inc)
                end
            end
        
            -- Link standard Android libs
            target:add("syslinks", "android", "log", "m", "EGL", "GLESv2")
        end)
    end

    if is_plat("android") then
        after_build(function (target)
            import("core.project.config")
            -- detect ABI: prefer xmake-provided env var, fallback to common abi
            local abi = config.get("arch") or "arm64-v8a"

            -- path where xmake places the built shared lib
            local built_path = path.join(os.projectdir(), "build", "android", abi, "release", "libraylib.so")
                
            -- destination folder used by the package_apk task
            local dest_dir = path.join(os.projectdir(), "build_android", "lib", abi)
                
            print(">>> after_build: copying libraylib.so")
            print("    built_path = " .. built_path)
            print("    dest_dir   = " .. dest_dir)
                
            -- ensure source exists
            if not os.isfile(built_path) then
                raise("libraylib.so not found at: " .. built_path .. "\nRun:\n  xmake f -p android --arch=" .. abi .. " && xmake")
            end
        
            -- make dest and copy
            os.mkdir(dest_dir)
            os.cp(built_path, dest_dir)
        
            print(">>> copied libraylib.so -> " .. path.join(dest_dir, "libraylib.so"))
        end)
    end