target("proto")
    set_kind("static")
    includes("*.h", {interface = true})
    add_files("*.cc")