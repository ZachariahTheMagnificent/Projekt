/*!
 *
 */

#ifndef PROJEKT_RENDERER_H
#define PROJEKT_RENDERER_H

#include <vulkan/vulkan.h>

#include "Vertex.h"
#include "../Window/Window.h"
#include "../Vulkan/Core/Instance.h"
#include "../Vulkan/Core/DebugReport.h"
#include "../Vulkan/Graphics/Surface.h"
#include "../Vulkan/Core/PhysicalDevice.h"
#include "../Vulkan/Core/LogicalDevice.h"
#include "../Vulkan/Core/Queue.h"
#include "../Vulkan/Core/CommandPool.h"

class Renderer
{
private:
    struct QueueFamilyIndices
    {
        int32_t graphics_family = -1;
        int32_t present_family = -1;

        bool is_complete()
        {
            return graphics_family >= 0 && present_family >= 0;
        }
    };

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;

        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

public:
    explicit Renderer( const Window& window );
    ~Renderer();

    void draw_frame();

    void recreate_swapchain();
    void cleanup_swapchain();

private:
    /*
    VkInstance                      create_instance() const;
    VkDebugReportCallbackEXT        create_debug_callback() const;
    VkSurfaceKHR                    create_surface() const;
    VkPhysicalDevice                pick_gpu();
    VkDevice                        create_device() const;
    VkQueue                         get_queue( int32_t family_index, uint32_t queue_index ) const;
    VkCommandPool                   create_command_pool();
     */
    VkSemaphore                     create_semaphore() const;
    VkFence                         create_fence() const;

    std::vector<VkSemaphore>        create_semaphores() const;
    std::vector<VkFence>            create_fences() const;

    VkSwapchainKHR                  create_swapchain();
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


    bool check_validation_layer_support() const;
    bool check_device_extension_support( VkPhysicalDevice& gpu_handle ) const;
    bool is_device_suitable( VkPhysicalDevice &gpu_handle );

    VkSurfaceFormatKHR choose_surface_format( const std::vector<VkSurfaceFormatKHR>& available_formats ) const;
    VkPresentModeKHR  choose_present_mode( const std::vector<VkPresentModeKHR>& available_present_modes ) const;
    VkExtent2D choose_extent( VkSurfaceCapabilitiesKHR& capabilities ) const;

    QueueFamilyIndices find_queue_family_indices( VkPhysicalDevice &gpu_handle ) const;
    SwapchainSupportDetails query_swapchain_support( VkPhysicalDevice &gpu_handle ) const;

private:
    const std::vector<const char*> validation_layers = {
            "VK_LAYER_LUNARG_standard_validation"
    };
    const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    const Window& window_;

    Vk::Core::Instance          instance_;
    Vk::Core::DebugReport       debug_report_;
    Vk::Graphics::Surface       surface_;
    Vk::Core::PhysicalDevice    gpu_;
    Vk::Core::LogicalDevice     logical_device_;
    Vk::Core::Queue             graphics_queue_;
    Vk::Core::Queue             present_queue_;
    Vk::Core::CommandPool       command_pool_;

    /*
    VkInstance                      instance_handle_                    = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT        debug_callback_handle_              = VK_NULL_HANDLE;
    VkSurfaceKHR                    surface_handle_                     = VK_NULL_HANDLE;
    VkPhysicalDevice                gpu_handle_                         = VK_NULL_HANDLE;
    VkDevice                        device_handle_                      = VK_NULL_HANDLE;
    VkQueue                         graphics_queue_handle_              = VK_NULL_HANDLE;
    VkQueue                         present_queue_handle_               = VK_NULL_HANDLE;
    VkCommandPool                   command_pool_handle_                = VK_NULL_HANDLE;
     */

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

    // QueueFamilyIndices queue_family_indices_;


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
