/*!
 *
 */

#ifndef PROJEKT_RENDERER_H
#define PROJEKT_RENDERER_H

#include <vulkan/vulkan.hpp>

#include "../Window/Window.h"

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
        vk::SurfaceCapabilitiesKHR capabilities;

        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> present_modes;
    };

public:
    explicit Renderer( Window& window );
    ~Renderer();

    void draw_frame();

    void recreate_swapchain();

private:
    vk::Instance                    create_instance() const;
    vk::DebugReportCallbackEXT      create_debug_callback() const;
    vk::SurfaceKHR                  create_surface() const;
    vk::PhysicalDevice              pick_gpu();
    vk::Device                      create_device() const;
    vk::Queue                       get_queue( int32_t family_index, uint32_t queue_index ) const;
    vk::CommandPool                 create_command_pool() const;
    vk::Semaphore                   create_semaphore() const;

    vk::SwapchainKHR                create_swapchain();
    vk::SwapchainKHR                create_swapchain( vk::SwapchainKHR& old_swapchain );
    std::vector<vk::ImageView>      create_image_views();
    std::vector<vk::Framebuffer>    create_framebuffers();
    std::vector<vk::CommandBuffer>  create_command_buffers();

    vk::RenderPass                  create_render_pass();
    void                            create_pso();
    vk::ShaderModule                create_shader_module( const std::string& shader_code );

    void record_commands();

    bool check_validation_layer_support() const;
    bool check_device_extension_support( vk::PhysicalDevice& gpu_handle ) const;
    bool is_device_suitable( vk::PhysicalDevice &gpu_handle );

    vk::SurfaceFormatKHR choose_surface_format( const std::vector<vk::SurfaceFormatKHR>& available_formats ) const;
    vk::PresentModeKHR  choose_present_mode( const std::vector<vk::PresentModeKHR>& available_present_modes ) const;
    vk::Extent2D choose_extent( vk::SurfaceCapabilitiesKHR& capabilities ) const;

    QueueFamilyIndices find_queue_family_indices( vk::PhysicalDevice &gpu_handle ) const;
    SwapchainSupportDetails query_swapchain_support( vk::PhysicalDevice &gpu_handle ) const;

private:
    const std::vector<const char*> validation_layers = {
            "VK_LAYER_LUNARG_standard_validation"
    };
    const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    Window& window_;

    vk::Instance                    instance_handle_;
    vk::DebugReportCallbackEXT      debug_callback_handle_;
    vk::SurfaceKHR                  surface_handle_;
    vk::PhysicalDevice              gpu_handle_;
    vk::Device                      device_handle_;
    vk::Queue                       graphics_queue_handle_;
    vk::Queue                       present_queue_handle_;
    vk::CommandPool                 command_pool_handle_;
    vk::Semaphore                   image_available_semaphore_handle_;
    vk::Semaphore                   render_finished_semaphore_handle_;

    vk::SwapchainKHR                swapchain_handle_;
    vk::Format                      swapchain_format_handle_;
    vk::Extent2D                    swapchain_extent_handle_;
    std::vector<vk::Image>          swapchain_images_;
    std::vector<vk::ImageView>      swapchain_image_view_handles_;
    std::vector<vk::Framebuffer>    swapchain_framebuffer_handles_;

    std::vector<vk::CommandBuffer>  command_buffer_handles;

    vk::RenderPass                  render_pass_handle_;

    vk::Pipeline                    pso_handle_;
    vk::PipelineLayout              pso_layout_handle_;


    QueueFamilyIndices indices_;
};

#endif //PROJEKT_RENDERER_H
