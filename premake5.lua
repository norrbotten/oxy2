workspace "oxy2"
    language "C++"
    cppdialect "C++17"

    location "build"

    configurations { "debug", "release" }

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

    buildoptions { "-march=znver2", "-mtune=znver2", "-std=c++2a", "-fopenmp" }

    links { "pthread", "gomp" }

newaction {
    trigger = "build",
    description = "build",
    execute = function()
        os.execute("rm -rf build")
        os.execute("premake5 gmake2 && cd build && make -j config=release")
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
