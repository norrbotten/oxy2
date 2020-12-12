#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#include <omp.h>
#include <signal.h>

#include "renderer/timer.hpp"

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/integrators/naive.hpp"
#include "renderer/integrators/preview.hpp"

#include "renderer/material/materials.hpp"
#include "renderer/tracing/sdf.hpp"
#include "renderer/tracing/sphere.hpp"

#include "renderer/sdl/parser.hpp"

#include "renderer/pools/material_pool.hpp"

using namespace Oxy;

int main() {}