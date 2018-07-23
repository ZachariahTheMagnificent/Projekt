cmake_minimum_required( VERSION 3.9 )
project( Projekt )

if( WIN32 )
    include_directories( ${PROJECT_SOURCE_DIR}/dependencies/win32/Include )

    link_directories( ${PROJECT_SOURCE_DIR}/dependencies/win32/Lib/vulkan )
    link_directories( ${PROJECT_SOURCE_DIR}/dependencies/win32/Lib/glfw )
else()
    include_directories( ${PROJECT_SOURCE_DIR}/dependencies/linux/include )

    link_directories( ${PROJECT_SOURCE_DIR}/dependencies/linux/lib/vulkan )
    link_directories( ${PROJECT_SOURCE_DIR}/dependencies/linux/lib/glfw )
endif()



set( CMAKE_CXX_STANDARD 17 )

add_executable( Projekt

        game/game.cpp
        game/game.h
        game/main.cpp

        engine/graphics/renderer.h
        engine/graphics/renderer.cpp
        engine/utils/exception/exception.h
        engine/utils/exception/glfw_exception.h
        engine/utils/exception/vulkan_exception.h
        engine/utils/file_io/read.h
        engine/utils/file_io/write.h
        engine/vulkan/core/command_buffers.cpp
        engine/vulkan/core/command_buffers.h
        engine/vulkan/core/command_pool.cpp
        engine/vulkan/core/command_pool.h
        engine/vulkan/core/compute_pipeline.cpp
        engine/vulkan/core/compute_pipeline.h
        engine/vulkan/core/debug_report.cpp
        engine/vulkan/core/debug_report.h
        engine/vulkan/core/descriptor_pool.cpp
        engine/vulkan/core/descriptor_pool.h
        engine/vulkan/core/descriptor_set_layout.cpp
        engine/vulkan/core/descriptor_set_layout.h
        engine/vulkan/core/descriptor_sets.cpp
        engine/vulkan/core/descriptor_sets.h
        engine/vulkan/core/fences.cpp
        engine/vulkan/core/fences.h
        engine/vulkan/core/index_buffer.cpp
        engine/vulkan/core/index_buffer.h
        engine/vulkan/core/instance.cpp
        engine/vulkan/core/instance.h
        engine/vulkan/core/logical_device.cpp
        engine/vulkan/core/logical_device.h
        engine/vulkan/core/physical_device.cpp
        engine/vulkan/core/physical_device.h
        engine/vulkan/core/queue.cpp
        engine/vulkan/core/queue.h
        engine/vulkan/core/render_pass.cpp
        engine/vulkan/core/render_pass.h
        engine/vulkan/core/semaphores.cpp
        engine/vulkan/core/semaphores.h
        engine/vulkan/core/shader_module.cpp
        engine/vulkan/core/shader_module.h
        engine/vulkan/core/vertex_buffer.cpp
        engine/vulkan/core/vertex_buffer.h
        engine/vulkan/graphics/frame_buffers.cpp
        engine/vulkan/graphics/frame_buffers.h
        engine/vulkan/graphics/graphics_pipeline.cpp
        engine/vulkan/graphics/graphics_pipeline.h
        engine/vulkan/graphics/surface.cpp
        engine/vulkan/graphics/surface.h
        engine/vulkan/graphics/swapchain.cpp
        engine/vulkan/graphics/swapchain.h
        engine/vulkan/graphics/texture_image.h
        engine/vulkan/graphics/texture_image.cpp
        engine/vulkan/graphics/uniform_buffer_object.h
        engine/vulkan/graphics/uniform_buffers.cpp
        engine/vulkan/graphics/uniform_buffers.h
        engine/vulkan/graphics/vertex.h
        engine/vulkan/helpers/queue_family_indices.h
        engine/vulkan/helpers/swapchain_support_details.h
        engine/window/event/event.h
        engine/window/event/event_handler.h
        engine/window/io/keyboard.h
        engine/window/io/mouse.h
        engine/window/glfw_callbacks.h
        engine/window/window.cpp
        engine/window/window.h
        )

if( WIN32 )
    target_link_libraries( Projekt libvulkan.so libglfw.so )
else()
    target_link_libraries( Projekt libvulkan.so libglfw.so )
endif()