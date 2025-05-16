set(APP_NAME ConcreteApp)

include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY  https://github.com/ocornut/imgui.git
    GIT_TAG         2860d7ba05667be5e2e51cf27d658352884800b8
)

FetchContent_Declare(glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         e7ea71be039836da3a98cea55ae5569cb5eb885c
)

FetchContent_MakeAvailable(imgui glfw)
find_package(Vulkan REQUIRED)

file(GLOB IMGUI_SOURCE
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

add_library(imgui STATIC
    ${IMGUI_SOURCE}
)

target_include_directories(imgui
    PUBLIC ${imgui_SOURCE_DIR}/
    PUBLIC ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)

set(SOURCES
    Device.cpp
    Instance.cpp
    PhysicalDevice.cpp
    Window.cpp
    WindowCallbacks.cpp
    Backbuffer.cpp
    Panic.cpp
    Scope.cpp
    LogicalDevice.cpp
)

add_executable(${APP_NAME}
    ${SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_link_libraries(${APP_NAME}
    PUBLIC glfw
    PUBLIC imgui
    PUBLIC Vulkan::Vulkan
)

configure_file(Config.h.in Config.h)

target_include_directories(${APP_NAME}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE .
    ${imgui_SOURCE_DIR}/
    ${imgui_SOURCE_DIR}/backends
)
