set(APP_NAME ConcreteApp)

set(SOURCES
    Device.cpp
    Engine.cpp
    Instance.cpp
    PhysicalDevice.cpp
    Surface.cpp
    Window.cpp
    Swapchain.cpp
    Image.cpp
    DescriptorSetLayout.cpp
    PipelineLayout.cpp
    # DescriptorPool.cpp
    Pipeline.cpp
)

add_executable(${APP_NAME}
    ${SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

find_package(glfw3 REQUIRED 3.3)
find_package(Vulkan REQUIRED)

target_link_libraries(${APP_NAME}
    PUBLIC glfw
    PUBLIC Vulkan::Vulkan
)

configure_file(Config.h.in Config.h)

target_include_directories(${APP_NAME}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE .
)
