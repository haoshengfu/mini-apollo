
target("time")
    set_kind("shared")
    add_deps("common")
    includes("*.h", {interface = true})
    add_files("*.cpp")