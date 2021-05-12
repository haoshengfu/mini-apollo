target("transport")
    set_kind("shared")
    add_deps("common","proto")
    includes("message/.h","receiver/.h","transmitter/.h", {interface = true})
    add_files("transport.cpp", "common/*.cpp", "dispatcher/*.cpp",
                "message/*.cpp","qos/*.cpp")