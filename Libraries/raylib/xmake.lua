-- Libraries/raylib/xmake.lua
-- Minimal robust wrapper: avoid calling is_enabled() at top-level.
set_project("raylib")
set_version("5.5.0")
set_languages("c11")

-- You may set the raylib build type via:
--  - xmake config flag:  xmake f --raylib_shared=y
--  - environment variable: RAYLIB_SHARED=1
--
-- The actual kind (static/shared) is chosen inside src/xmake.lua using get_config()/os.getenv().

-- Expose an optional menu flag (optional; does not use is_enabled here)

option("raylib_shared")
    set_showmenu(true)
    set_default(false)
    set_description("Build raylib as shared library")
option_end()

if is_plat("android") then
    set_arch("arm64-v8a")
    add_cxflags("-fPIC", "-O2")
    add_ldflags("-fPIC")
end

-- Include the actual library build script
includes("src")
