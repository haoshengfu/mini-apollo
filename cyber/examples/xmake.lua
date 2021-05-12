-- target("listener")
--     set_kind("shared")
--     add_deps("core")
--     add_files("listener.cc")

-- target("talker")
--     set_kind("shared")
--     add_deps("core")
--     add_files("talker.cc")

target("example")
    set_kind("shared")
    add_deps("core")

    includes("common_component/.h")
    add_files("common_component/*.cpp","proto/examples.pb.cc")

-- target("timer-example")
--     set_kind("shared")
--     add_deps("core)
--     add_files("timer_component_example/*.cc","proto/examples.pb.cc")