
target("blocker")
    set_kind("shared")
    add_deps("time","node")
    includes("*.h", {interface = true})
    add_files("blocker_manager.cpp")

-- target("blocker_test")
--     set_kind("binary")
--     includes("*.h")
--     add_deps("core")
--     add_files("*.cc")