cmake_minimum_required( VERSION 3.9 )
project( Projekt )

include_directories( ${PROJECT_SOURCE_DIR}/dependencies/include )

link_directories( ${PROJECT_SOURCE_DIR}/dependencies/lib/vulkan )
link_directories( ${PROJECT_SOURCE_DIR}/dependencies/lib/glfw )

set(CMAKE_CXX_STANDARD 17)

add_executable(Projekt main.cpp

        Engine/Graphics/Renderer.h
        Engine/Graphics/Renderer.cpp
        Engine/Utils/Exception/Exception.h
        Engine/Utils/Exception/GlfwException.h
        Engine/Utils/Exception/VulkanException.h
        Engine/Utils/File/Read.h
        Engine/Utils/File/Write.h
        Engine/Window/Window.h
        Engine/Window/Window.cpp

        Game/Game.h
        Game/Game.cpp Engine/Graphics/Vertex.h Engine/Vulkan/Instance.h)

target_link_libraries( Projekt libvulkan.so libglfw.so )