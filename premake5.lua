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

local function test(name)
    project(name)
    kind "ConsoleApp"
    files { "tests/" .. name .. ".cpp", "src/**.cpp", "ext/lodepng/lodepng.cpp" }
    excludes "src/main.cpp"
    includedirs { "src/", "ext/glm", "ext/lodepng" }
    buildoptions { "-march=znver2", "-mtune=znver2", "-std=c++2a", "-fopenmp" }
    links { "pthread" }
end

test("test_tcp_client")
test("test_tcp_server")
test("test_http_client")
test("test_http_server")
test("test_json_parser")
test("test_json_ast")
test("test_json")
test("test_xsdl_parser")

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

newoption {
    trigger = "test",
    description = "specify what test to run",
}

newoption {
    trigger = "debug",
    description = "run the test in gdb",
}

newaction {
    trigger = "test",
    description = "run a test",
    execute = function()
        local test = _OPTIONS["test"]

        if _OPTIONS["debug"] then
            os.execute("premake5 gmake2 && cd build && make -j -f test_" .. test .. ".make && gdb ./bin/debug/test_" .. test)
        else
            os.execute("premake5 gmake2 && cd build && make -j -f test_" .. test .. ".make && ./bin/debug/test_" .. test)
        end
    end
}
