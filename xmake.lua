-- xmake.lua for CatFishers
add_rules("mode.debug", "mode.release")
-- --------------------
-- Dependencies
-- --------------------

includes("Libraries/raylib") 
add_requires("enet6", {system=false})
add_requires("nlohmann_json")


-- Set project

set_project("CatFishers")
set_languages("c++17")

-- App info
APP_NAME       = "CatFishers"
APP_PACKAGE    = "com.raylib.catfishers"
APP_VERSION    = "1.0"
APP_VERSIONCODE= 1
APP_ORIENTATION= "landscape"

-- Paths
SRC_DIR        = "src"
JNI_LIBS_DIR   = "jniLibs"
RES_DIR        = "res"
BUILD_DIR      = "build_android"
LIB_CLIENT     = path.join(JNI_LIBS_DIR, "$(TARGET_ARCH_ABI)/libmain.so")
LIB_RAYLIB     = path.join(JNI_LIBS_DIR, "$(TARGET_ARCH_ABI)/libraylib.so")


if is_plat("android") then
    set_arch("arm64-v8a")
end

-- --------------------
-- Subdirectories
-- --------------------
includes("CatCore")
includes("Client")
includes("Server")

task("package_apk")
    set_category("package")
    set_menu { usage = "xmake package_apk", description = "Package APK including libmain.so (no package.tools.android required)" }
    on_run(function ()

        -- build the project first (abort if build fails)
        print(">>> Running xmake to build native artifacts (libmain.so)...")
        -- run plain `xmake` (honors the current xmake.lua and user's config)
        os.exec("xmake")


        -- config (change if you want)
        local build_dir = BUILD_DIR or "build_android"
        local app_name = APP_NAME or "CatFishers"
        local app_package = APP_PACKAGE or "com.raylib.catfishers"
        local app_version = APP_VERSION or "1.0"
        local app_versioncode = APP_VERSIONCODE or 1
        local min_sdk = 29
        local target_sdk = 33
        local abi = os.getenv("XMAKE_TARGET_ARCH_ABI") or "arm64-v8a" -- default if not built
        local projectdir = os.projectdir()

        os.tryrm(projectdir .. "/" .. build_dir .. "/" .. app_name .. ".unaligned.apk")
        os.tryrm(projectdir .. "/" .. build_dir .. "/" .. app_name .. ".aligned.apk")
        os.tryrm(projectdir .. "/" .. build_dir .. "/" .. app_name .. ".apk.idsig")
        os.tryrm(projectdir .. "/" .. build_dir .. "/" .. app_name .. ".keystore")
        os.tryrm(projectdir .. "/" .. build_dir .. "/" .. app_name .. ".apk")

        -- env checks
        local android_home = os.getenv("ANDROID_HOME") or os.getenv("ANDROID_SDK_ROOT")
        if not android_home then
            raise("ANDROID_HOME / ANDROID_SDK_ROOT is not set. Set your Android SDK path before packaging.")
        end
        local java_home = os.getenv("JAVA_HOME")
        if not java_home then
            print("Warning: JAVA_HOME not set. keytool may be unavailable for generating keystore.")
        end

        -- locate latest build-tools dir
        local build_tools_root = path.join(android_home, "build-tools")
        if not os.isdir(build_tools_root) then
            raise("Android build-tools not found under: " .. build_tools_root)
        end
        local tools_dirs = os.dirs(build_tools_root .. "/*")
        table.sort(tools_dirs)
        if #tools_dirs == 0 then
            raise("No build-tools versions found under: " .. build_tools_root)
        end
        local build_tools = tools_dirs[#tools_dirs] -- pick latest lexicographically
        print("Using build-tools:", build_tools)

        -- helper to find executable (exe on windows)
        local function find_exec(base, name)
            local f = path.join(base, name)
            if os.host() == "windows" then
                if os.isfile(f .. ".exe") then return f .. ".exe" end
                if os.isfile(f .. ".bat") then return f .. ".bat" end
            else
                if os.isfile(f) then return f end
            end
            -- try name as-is
            if os.isfile(f) then return f end
            return nil
        end

        local aapt = find_exec(build_tools, "aapt")
        local zipalign = find_exec(build_tools, "zipalign")
        local apksigner = find_exec(build_tools, "apksigner")
        local d8 = find_exec(build_tools, "d8") 
        local keytool = java_home and find_exec(path.join(java_home, "bin"), "keytool")
        local javac = java_home and find_exec(path.join(java_home, "bin"), "javac")
        if not aapt then raise("aapt not found in build-tools: " .. build_tools) end
        if not zipalign then raise("zipalign not found in build-tools: " .. build_tools) end
        if not apksigner then
            -- apksigner may be a jar in newer SDKs, but usually an executable exists
            -- we try also android-sdk/build-tools/<ver>/apksigner
            raise("apksigner not found in build-tools: " .. build_tools)
        end

        -- find android.jar for min_sdk (fallback to any platform)
        local android_jar = path.join(android_home, "platforms", "android-" .. tostring(min_sdk), "android.jar")
        if not os.isfile(android_jar) then
            -- find highest platform available
            local plat_dirs = os.dirs(path.join(android_home, "platforms", "android-*"))
            table.sort(plat_dirs)
            if #plat_dirs == 0 then
                raise("No Android platforms found under: " .. path.join(android_home, "platforms"))
            end
            android_jar = path.join(plat_dirs[#plat_dirs], "android.jar")
            if not os.isfile(android_jar) then
                raise("android.jar not found. Install an Android platform (sdkmanager).")
            end
        end
        print("Using android.jar:", android_jar)

        -- prepare build layout
        local apk_build_res = path.join(projectdir, build_dir, "res")
        local apk_build_assets = path.join(projectdir, build_dir, "assets")
        local apk_build_libs = path.join(projectdir, build_dir, "lib")
        os.mkdir(apk_build_res); os.mkdir(apk_build_assets); os.mkdir(apk_build_libs)

        -- ensure res/values/strings.xml
        local values_dir = path.join(apk_build_res, "values")
        os.mkdir(values_dir)
        local sx = io.open(path.join(values_dir, "strings.xml"), "w")
        sx:write('<?xml version="1.0" encoding="utf-8"?>\n<resources>\n')
        sx:write(("  <string name=\"app_name\">%s</string>\n"):format(app_name))
        sx:write('</resources>\n')
        sx:close()

        -- Generate NativeLoader.java
        local java_pkg_dir = path.join(projectdir, build_dir, "src", "com", "raylib", "catfishers")

        -- Ensure AndroidManifest exists (create minimal if not)
        local manifest_path = path.join(projectdir, build_dir, "AndroidManifest.xml")
        if not os.isfile(manifest_path) then
            local mf = io.open(manifest_path, "w")
            mf:write('<?xml version="1.0" encoding="utf-8"?>\n')
            mf:write(('<manifest xmlns:android="http://schemas.android.com/apk/res/android" package="%s" android:versionCode="%d" android:versionName="%s">\n')
                :format(app_package, app_versioncode, app_version))
        
            -- internet permission for ENet / network access
            mf:write('  <uses-permission android:name="android.permission.INTERNET" />\n')
            -- optional: if you want to query network state on device
            -- mf:write('  <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />\n')
        
            mf:write(('  <uses-sdk android:minSdkVersion="%d" android:targetSdkVersion="%d" />\n')
                :format(min_sdk, target_sdk))
            mf:write('  <uses-feature android:glEsVersion="0x00020000" android:required="true" />\n')
            mf:write('  <application android:allowBackup="false" android:label="@string/app_name" android:icon="@drawable/icon">\n')
            mf:write('    <activity android:name="com.raylib.catfishers.NativeLoader" android:theme="@android:style/Theme.NoTitleBar.Fullscreen" android:configChanges="orientation|keyboard|keyboardHidden|screenSize" android:screenOrientation="landscape" android:launchMode="singleTask" android:clearTaskOnLaunch="true" android:exported="true">\n')
            mf:write('      <meta-data android:name="android.app.lib_name" android:value="main" />\n')
            mf:write('      <intent-filter>\n')
            mf:write('        <action android:name="android.intent.action.MAIN" />\n')
            mf:write('        <category android:name="android.intent.category.LAUNCHER" />\n')
            mf:write('      </intent-filter>\n')
            mf:write('    </activity>\n')
            mf:write('  </application>\n')
            mf:write('</manifest>\n')
            mf:close()
        end

        -- copy libmain.so into build_dir/lib/<abi>/
        local built_lib = path.join(projectdir, "build", "android", abi, "release", "libmain.so")
        local jni_prebuilt = path.join(projectdir, "jniLibs", abi, "libmain.so")
        local dest_abi_dir = path.join(apk_build_libs, abi)
        os.mkdir(dest_abi_dir)

        if os.isfile(built_lib) then
            os.cp(built_lib, path.join(dest_abi_dir, "libmain.so"))
            print("Copied built lib:", built_lib)
        elseif os.isfile(jni_prebuilt) then
            os.cp(jni_prebuilt, path.join(dest_abi_dir, "libmain.so"))
            print("Copied prebuilt jniLibs lib:", jni_prebuilt)
        else
            -- not fatal immediately - allow other ABIs or user to fix
            print(("Warning: libmain.so not found for ABI %s. Looked at:\n  %s\n  %s\n"):format(abi, built_lib, jni_prebuilt))
        end

        -- optionally copy libraylib.so if present under jniLibs
        local raylib_prebuilt = path.join(projectdir, "jniLibs", abi, "libraylib.so")
        if os.isfile(raylib_prebuilt) then
            os.cp(raylib_prebuilt, path.join(dest_abi_dir, "libraylib.so"))
            print("Copied prebuilt libraylib.so:", raylib_prebuilt)
        end

        -- Compile, Dex, and package the Java loader
        local java_src_dir = path.join(projectdir, "android", "src") -- <-- changed to android/src
        local classes_dir = path.join(projectdir, build_dir, "classes")
        local dex_path = path.join(projectdir, build_dir, "classes.dex")
        os.mkdir(classes_dir)

        -- 1. Compile all .java files under android/src
        local java_files = os.files(path.join(java_src_dir, "**.java"))
        if #java_files == 0 then
            raise("No Java files found under: " .. java_src_dir)
        end
        
        local javac_cmd = ('"%s" -cp "%s" -d "%s" %s'):format(
            javac,
            android_jar,
            classes_dir,
            table.concat(java_files, " ")
        )
        print("\nCompiling Java:", javac_cmd)
        os.exec(javac_cmd)
        
        -- 2. Dex all compiled .class files
        local class_files = os.files(path.join(classes_dir, "**.class"))
        local d8_cmd = ('"%s" --output "%s" %s'):format(d8, path.join(projectdir, build_dir), table.concat(class_files, " "))
        print("\nRunning d8:", d8_cmd)
        os.exec(d8_cmd)

        -- Build output apk paths
        local unaligned_apk = path.join(projectdir, build_dir, app_name .. ".unaligned.apk")
        local aligned_apk = path.join(projectdir, build_dir, app_name .. ".aligned.apk")
        local signed_apk = path.join(projectdir, build_dir, app_name .. ".apk")


        -- run aapt package
        local aapt_cmd = ("%s package -f -M %s -S %s -I %s -F %s"):format(aapt, manifest_path, apk_build_res, android_jar, unaligned_apk)
        print("\nRunning aapt:", aapt_cmd)
        os.exec(aapt_cmd)

        
        os.cd("$(projectdir)/build_android")

        -- Corrected aapt_add function to take both source and destination paths
        local function aapt_add(file)
            if os.isfile(file) then
                local cmd = string.format(
                    "%s add %s %s",
                    aapt,
                    unaligned_apk,
                    file
                )
                print("aapt add:", cmd)
                os.exec(cmd)
            end
        end

        for _, abi_dir in ipairs(os.dirs("lib/*")) do
            local abi_name = path.basename(abi_dir)
            for _, f in ipairs(os.files(path.join(abi_dir, "*"))) do
                local abi_name2 = path.basename(f)
                print(abi_name2)
                aapt_add("lib/" .. abi_name .. "/" .. abi_name2 .. ".so")
            end
        end
        
        aapt_add("classes.dex")

        -- zipalign
        local zipalign_cmd = ("%s -p 4 %s %s"):format(zipalign, unaligned_apk, aligned_apk)
        print("\nRunning zipalign:", zipalign_cmd)
        os.exec(zipalign_cmd)

        -- ensure keystore exists, generate if missing
        local keystore = path.join(projectdir, build_dir, app_name .. ".keystore")
        if not os.isfile(keystore) then
            if java_home then
                local keytool = path.join(java_home, "bin", "keytool")
                if os.host() == "windows" and os.isfile(keytool .. ".exe") then keytool = keytool .. ".exe" end
                local gen_cmd = ('%s -genkeypair -alias %sKey -keystore %s -storepass android -keypass android -dname "CN=Unknown, O=Unknown, C=US" -validity 10000 -keyalg RSA'):format(keytool, app_name, keystore)
                print("Generating keystore:", gen_cmd)
                os.exec(gen_cmd)
            else
                raise("Keystore not found and JAVA_HOME not set to generate one automatically.")
            end
        end

        -- apksigner sign
        local sign_cmd = ("%s sign --ks %s --ks-pass pass:android --key-pass pass:android --out %s %s"):format(apksigner, keystore, signed_apk, aligned_apk)
        print("\nSigning APK:", sign_cmd)
        os.exec(sign_cmd)   
        print("APK created and signed at:", signed_apk)
        print("\nYou can install with: adb install -r " .. signed_apk)
        -- optionally run install: os.exec(path.join(android_home, "platform-tools", "adb") .. " install -r " .. quoted(signed_apk))
    end)

task("deploy")
    set_category("deploy")
    set_menu { usage = "xmake deploy", description = "build project create package and install to android phone" }

    on_run(function ()
        local build_dir  = BUILD_DIR or "build_android"
        local app_name   = APP_NAME or "CatFishers"
        local app_package= APP_PACKAGE or "com.raylib.catfishers"

        print(">>> Running xmake package_apk to build .apk files")
        os.exec("xmake package_apk")
        
        print(">>> Running adb commands")
        print(">>> Checking if package is installed: " .. app_package)
        local list_out = os.iorun("adb shell pm list packages " .. app_package) or ""
        local present = false
        if list_out:match(app_package) then
            present = true
            print(">>> Package present on device.")
        else
            print(">>> Package not installed on device.")
        end

        if present then
            os.exec("adb uninstall " .. app_package)
        else
            print("Package not installed; skipping uninstall.")
        end
        print("\nInstalling new apk:")
        os.exec("adb install " .. os.projectdir() .. "/" .. build_dir .. "/" .. app_name .. ".apk")  

    end)