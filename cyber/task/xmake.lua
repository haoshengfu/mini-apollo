target("task")
    set_kind("shared")
    add_deps("scheduler")
    includes("*.h", {interface = true})
    add_files("*.cpp")