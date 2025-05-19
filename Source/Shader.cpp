#include "Shader.h"

#include "Scope.h"
#include "vulkan/vulkan.h"
#include "Config.h"

#include <stdexcept>
#include <filesystem>
#include <exception>
#include <format>
#include <fstream>

const std::vector<uint32_t> triangle_vert_code = {
#include TRIANGLE_VERT
};

const std::vector<uint32_t> triangle_frag_code = {
#include TRIANGLE_FRAG
};


