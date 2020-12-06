workspace "oxy2"
    language "C++"
    cppdialect "C++17"

    location "build"

    configurations { "debug", "release" }

    buildoptions { "-std=c++2a" }

    filter { "configurations:debug" }
        symbols "On"
        optimize "Off"
    
    filter { "configurations:release" }
        symbols "Off"
        optimize "Full"
        buildoptions { "-Werror", "-Wextra", "-Wall", "-Wpedantic" }

    filter { }

project "oxy2"
    kind "ConsoleApp"

    files { "src/**.cpp", "ext/lodepng/lodepng.cpp" }

    includedirs { "src/", "ext/glm", "ext/lodepng" }

    excludes { "src/oldrenderer/**.*" }

    buildoptions "-march=native"

    links "pthread"

newaction {
    trigger = "build",
    description = "build",
    execute = function()
        os.execute("rm -rf build")
        os.execute("premake5 gmake2 && cd build && make -j")
    end
}

newaction {
    trigger = "debug",
    description = "debug",
    execute = function()
        os.execute("(premake5 gmake2 && cd build && make -j) && gdb ./build/bin/debug/oxy2")
    end
}

newaction {
    trigger = "run",
    description = "run",
    execute = function()
        os.execute("(premake5 gmake2 && cd build && make -j config=release) && time -p ./build/bin/release/oxy2")
    end
}
