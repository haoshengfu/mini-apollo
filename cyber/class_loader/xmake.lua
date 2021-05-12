target("class_loader")
    set_kind("shared")
    add_deps("common")
    includes("*.h", {interface = true})
    add_files("*.cpp", "utility/*.cpp")

-- target("class_loader_test")
--     set_kind("static")
--     add_files("*.cc")