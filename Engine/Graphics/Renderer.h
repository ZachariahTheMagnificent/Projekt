/*!
 *
 */

#ifndef PROJEKT_RENDERER_H
#define PROJEKT_RENDERER_H

#include <vulkan/vulkan.h>

#include "Vertex.h"
#include "../Window/Window.h"
#include "../Vulkan/Instance.h"
#include "../Vulkan/DebugReport.h"
#include "../Vulkan/Surface.h"
#include "../Vulkan/PhysicalDevice.h"

class Renderer
{
public:
    explicit Renderer( Window& window );
    ~Renderer();

    void draw_frame();

    void recreate_swapchain();
    void cleanup_swapchain();

private:
    VkDevice                        create_device( );
    VkQueue                         get_queue( int32_t family_index, uint32_t queue_index );
    VkCommandPool                   create_command_pool( );
    VkSemaphore                     create_semaphore();
    VkFence                         create_fence();

    std::vector<VkSemaphore>        create_semaphores();
    std::vector<VkFence>            create_fences();

    VkSwapchainKHR                  create_swapchain( );
    VkSwapchainKHR                  create_swapchain( VkSwapchainKHR& old_swapchain );
    std::vector<VkImageView>        create_image_views();
    std::vector<VkFramebuffer>      create_framebuffers();
    std::vector<VkCommandBuffer>    create_command_buffers();

    VkRenderPass                    create_render_pass();
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

private:
    const std::vector<const char*> validation_layers = {
            "VK_LAYER_LUNARG_standard_validation"
    };
    const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    Window& window_;

    Vk::Instance instance_;
    Vk::DebugReport debug_report_;
    Vk::Surface surface_;
    Vk::PhysicalDevice gpu_;

    VkDevice                        device_handle_                      = VK_NULL_HANDLE;
    VkQueue                         graphics_queue_handle_              = VK_NULL_HANDLE;
    VkQueue                         present_queue_handle_               = VK_NULL_HANDLE;
    VkCommandPool                   command_pool_handle_                = VK_NULL_HANDLE;

    std::vector<VkSemaphore>        image_available_semaphore_handles_;
    std::vector<VkSemaphore>        render_finished_semaphore_handles_;
    std::vector<VkFence>            fences_;

    VkSwapchainKHR                  swapchain_handle_                   = VK_NULL_HANDLE;
    VkFormat                        swapchain_format_handle_;
    VkExtent2D                      swapchain_extent_handle_;
    std::vector<VkImage>            swapchain_images_;
    std::vector<VkImageView>        swapchain_image_view_handles_;
    std::vector<VkFramebuffer >     swapchain_framebuffer_handles_;

    std::vector<VkCommandBuffer>    command_buffer_handles_;

    VkRenderPass                    render_pass_handle_                 = VK_NULL_HANDLE;

    VkPipeline                      pso_handle_                         = VK_NULL_HANDLE;
    VkPipelineLayout                pso_layout_handle_                  = VK_NULL_HANDLE;

    VkBuffer                        vertex_buffer_handle_               = VK_NULL_HANDLE;
    VkDeviceMemory                  vertex_buffer_memory_handle_        = VK_NULL_HANDLE;

    VkBuffer                        index_buffer_handle_                = VK_NULL_HANDLE;
    VkDeviceMemory                  index_buffer_memory_handle_         = VK_NULL_HANDLE;

    Vk::Helpers::QueueFamilyIndices queue_family_indices_;

    size_t current_frame = 0;

    const std::vector<Vertex> vertices = {
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
