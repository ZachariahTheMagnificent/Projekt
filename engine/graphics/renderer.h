/*!
 *
 */

#ifndef PROJEKT_RENDERER_H
#define PROJEKT_RENDERER_H

#include <vulkan/vulkan.h>

#include "vertex.h"
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

class renderer
{
private:
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;

        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

public:
    explicit renderer( const window& window );
    ~renderer();

    void draw_frame();

    void recreate_swapchain();
    void cleanup_swapchain();

private:
    VkSemaphore                     create_semaphore() const;
    VkFence                         create_fence() const;

    std::vector<VkSemaphore>        create_semaphores() const;
    std::vector<VkFence>            create_fences() const;

    std::vector<VkFramebuffer>      create_framebuffers();
    std::vector<VkCommandBuffer>    create_command_buffers();

    void                            create_pso();
    VkShaderModule                  create_shader_module( const std::string& shader_code );

    void record_commands();

    void create_buffer( VkDeviceSize& size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                        VkBuffer& buffer, VkDeviceMemory& buffer_memory );
    void copy_buffer( VkBuffer& src_buffer, VkBuffer& dst_buffer, VkDeviceSize& size );
    void create_vertex_buffer();
    void create_index_buffer();
    uint32_t find_memory_type( uint32_t type_filter, VkMemoryPropertyFlags properties );


    VkSurfaceFormatKHR choose_surface_format( const std::vector<VkSurfaceFormatKHR>& available_formats ) const;
    VkPresentModeKHR  choose_present_mode( const std::vector<VkPresentModeKHR>& available_present_modes ) const;
    VkExtent2D choose_extent( VkSurfaceCapabilitiesKHR& capabilities ) const;

    SwapchainSupportDetails query_swapchain_support( VkPhysicalDevice &gpu_handle ) const;

private:
    const std::vector<const char*> validation_layers = {
            "VK_LAYER_LUNARG_standard_validation"
    };
    const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    const window& window_;

    vk::core::instance          instance_;
    vk::core::debug_report      debug_report_;
    vk::graphics::surface       surface_;
    vk::core::physical_device   gpu_;
    vk::core::logical_device    logical_device_;
    vk::core::queue             graphics_queue_;
    vk::core::queue             present_queue_;
    vk::core::command_pool      command_pool_;

    std::vector<VkSemaphore>        image_available_semaphore_handles_;
    std::vector<VkSemaphore>        render_finished_semaphore_handles_;
    std::vector<VkFence>            fences_;

    vk::graphics::swapchain     swapchain_;
    vk::core::render_pass       render_pass_;

    std::vector<VkFramebuffer>     swapchain_framebuffer_handles_;

    std::vector<VkCommandBuffer>    command_buffer_handles_;

    VkPipeline                      pso_handle_                         = VK_NULL_HANDLE;
    VkPipelineLayout                pso_layout_handle_                  = VK_NULL_HANDLE;

    VkBuffer                        vertex_buffer_handle_               = VK_NULL_HANDLE;
    VkDeviceMemory                  vertex_buffer_memory_handle_        = VK_NULL_HANDLE;

    VkBuffer                        index_buffer_handle_                = VK_NULL_HANDLE;
    VkDeviceMemory                  index_buffer_memory_handle_         = VK_NULL_HANDLE;

    size_t current_frame = 0;

    const std::vector<vertex> vertices = {
            { { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
    };

    const std::vector<std::uint16_t> indices_ = {
            0, 1, 3, 1, 2, 3
    };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
};

#endif //PROJEKT_RENDERER_H
