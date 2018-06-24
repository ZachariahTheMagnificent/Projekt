cmake_minimum_required( VERSION 3.9 )
project( Projekt )

include_directories( ${PROJECT_SOURCE_DIR}/dependencies/include )

link_directories( ${PROJECT_SOURCE_DIR}/dependencies/lib/vulkan )
link_directories( ${PROJECT_SOURCE_DIR}/dependencies/lib/glfw )

set(CMAKE_CXX_STANDARD 17)

add_executable(Projekt main.cpp

        Engine/Graphics/Renderer.h
        Engine/Graphics/Renderer.cpp
        Engine/Graphics/Vertex.h
        Engine/Utils/Exception/Exception.h
        Engine/Utils/Exception/GlfwException.h
        Engine/Utils/Exception/VulkanException.h
        Engine/Utils/File/Read.h
        Engine/Utils/File/Write.h
        Engine/Vulkan/Core/CommandBuffers.h
        Engine/Vulkan/Core/CommandPool.cpp
        Engine/Vulkan/Core/CommandPool.h
        Engine/Vulkan/Core/DebugReport.cpp
        Engine/Vulkan/Core/DebugReport.h
        Engine/Vulkan/Core/Fences.h
        Engine/Vulkan/Core/Instance.cpp
        Engine/Vulkan/Core/Instance.h
        Engine/Vulkan/Core/LogicalDevice.cpp
        Engine/Vulkan/Core/LogicalDevice.h
        Engine/Vulkan/Core/PhysicalDevice.cpp
        Engine/Vulkan/Core/PhysicalDevice.h
        Engine/Vulkan/Core/Queue.cpp
        Engine/Vulkan/Core/Queue.h
        Engine/Vulkan/Core/Semaphores.h
        Engine/Vulkan/Core/ShaderModule.cpp
        Engine/Vulkan/Core/ShaderModule.h
        Engine/Vulkan/Graphics/Surface.h
        Engine/Vulkan/Helpers/QueueFamilyIndices.h
        Engine/Window/Window.h
        Engine/Window/Window.cpp

        Game/Game.h
        Game/Game.cpp
        Engine/Vulkan/Graphics/Surface.cpp)

target_link_libraries( Projekt libvulkan.so libglfw.so )