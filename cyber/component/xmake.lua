target("component")
    set_kind("shared")
    add_deps("scheduler","timer","class_loader","node")
    add_files("*.cpp")

-- target("component_test")
--     set_kind("static")
--     add_files("*.cc")