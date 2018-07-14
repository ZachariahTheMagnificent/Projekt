/*!
 *
 */

#ifndef PROJEKT_RENDERER_H
#define PROJEKT_RENDERER_H

#include <vulkan/vulkan.h>

#include "../window/window.h"
#include "../vulkan/core/instance.h"
#include "../vulkan/core/debug_report.h"
#include "../vulkan/graphics/surface.h"
#include "../vulkan/core/physical_device.h"
#include "../vulkan/core/logical_device.h"
#include "../vulkan/core/command_pool.h"
#include "../vulkan/core/queue.h"
#include "../vulkan/graphics/swapchain.h"
#include "../vulkan/core/render_pass.h"
#include "../vulkan/graphics/vertex.h"
#include "../vulkan/graphics/graphics_pipeline.h"
#include "../vulkan/graphics/frame_buffers.h"
#include "../vulkan/core/command_buffers.h"
#include "../vulkan/core/fences.h"
#include "../vulkan/core/semaphores.h"
#include "../vulkan/core/vertex_buffer.h"
#include "../vulkan/core/index_buffer.h"
#include "../vulkan/graphics/uniform_buffers.h"
#include "../vulkan/core/descriptor_pool.h"
#include "../vulkan/core/descriptor_sets.h"

class renderer
{
public:
    renderer( const window& window );
    ~renderer();

    void prepare_frame( );
    void submit_frame( );

    void update( float dt );

    void prepare_pipeline( std::string&& vertex_shader, std::string&& fragment_shader );
    void prepare_for_rendering( const std::vector<vk::graphics::vertex>& vertices, const std::vector<std::uint16_t>& indices );

    void handle_event( event& e );

private:
    void recreate_swapchain( );
    void record_commands( );

    void create_vertex_buffer( const std::vector<vk::graphics::vertex>& vertices );
    void create_index_buffer( const std::vector<std::uint16_t>& indices );

    void handle_window_resizing();
    void handle_frame_buffer_resizing( event& e );

private:
    const std::vector<const char*> validation_layers = {
            "VK_LAYER_LUNARG_standard_validation"
    };
    const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    const window& window_;

    vk::core::instance              instance_;
    vk::core::debug_report          debug_report_;
    vk::graphics::surface           surface_;
    vk::core::physical_device       gpu_;
    vk::core::logical_device        logical_device_;
    vk::core::queue                 graphics_queue_;
    vk::core::queue                 present_queue_;
    vk::core::command_pool          command_pool_;

    vk::core::semaphores            image_available_semaphores_;
    vk::core::semaphores            render_finished_semaphores_;
    vk::core::fences                fences_;

    vk::core::descriptor_pool       descriptor_pool_;
    vk::core::descriptor_set_layout descriptor_set_layout_;
    vk::core::descriptor_sets       descriptor_sets_;

    vk::graphics::swapchain         swapchain_;
    vk::core::render_pass           render_pass_;
    vk::graphics::graphics_pipeline graphics_pipeline_;

    vk::graphics::frame_buffers     frame_buffers_;
    vk::core::command_buffers       command_buffers_;

    // TODO: put them somewhere else.
    vk::core::shader_module         vertex_shader_;
    vk::core::shader_module         fragment_shader_;

    vk::core::vertex_buffer         vertex_buffer_;
    vk::core::index_buffer          index_buffer_;
    vk::graphics::uniform_buffers   uniform_buffers_;

    size_t current_frame_ = 0;
    uint32_t image_index_ = 0;

    glm::mat4 projection_matrix_;
    float test = 0;
};

#endif //PROJEKT_RENDERER_H
