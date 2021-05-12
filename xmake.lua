-- define project
set_project("miniapollo")
set_version("1.0.0", {build = "%Y%m%d%H%M"})

-- add build modes
add_rules("mode.release", "mode.debug")

-- set common flags
set_languages("c++17")

-- add 3rd lib
add_requires("protobuf-cpp", {system = false})
add_requires("gflags", "glog", "Poco", "gtest")
add_packages("protobuf-cpp", "gflags", "glog","Poco", "gtest")

-- 如何已加入的生成后的proto文件无法使用。则重新生成proto文件后，加入文件夹。避免单独为所有target加入
-- add_files("cyber/proto/*.proto", {rules = "protobuf.cpp"})

--完全串行化 target 间的源码编译顺序，不再跨目标并行编译源码
set_policy("build.across_targets_in_parallel", false)

-- includes sub-projects
includes("cyber/base", "cyber/blocker", "cyber/binary.h",
        "cyber/common", "cyber/class_loader", "cyber/component",
        "cyber/croutine", "cyber/data", "cyber/event",
         "cyber/logger", "cyber/message", "cyber/node","cyber/proto",
         "cyber/scheduler", "cyber/task",
         "cyber/time", "cyber/timer", "cyber/transport")

-- 如果希望编译example
includes("cyber/examples")

target("state")
    set_kind("shared")
    add_deps("common")
    add_files("cyber/state.cpp")

target("core")
    set_kind("phony")
    add_deps("blocker", "class_loader", "common", "proto",
            "component","croutine","logger","message","node",
            "scheduler","task","time","timer","transport","state")

target("cyber")
    set_kind("shared")
    add_deps("core")
    add_files("cyber/cyber.cpp", "cyber/init.cpp")


target("miniapollo")
    set_kind("binary")
    add_deps("cyber")
    add_files("cyber/mainboard/*.cpp")


-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro defination
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

