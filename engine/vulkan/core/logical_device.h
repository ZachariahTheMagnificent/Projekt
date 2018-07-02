/*!
 *
 */

#ifndef COMPUTE_LOGICALDEVICE_H
#define COMPUTE_LOGICALDEVICE_H

#include <vulkan/vulkan.h>

#include "physical_device.h"

namespace vk
{
    namespace core
    {
        class logical_device
        {
        public:
            logical_device( ) = default;
            logical_device( physical_device& physical_device,
                            const std::vector<const char*>& validation_layers,
                            const std::vector<const char*>& device_extensions );
            logical_device( const logical_device& logical_device ) = delete;
            logical_device( logical_device&& logical_device ) noexcept;
            ~logical_device( );

            VkDevice get()
            {
                return device_handle_;
            }

            logical_device& operator=( const logical_device& logical_device ) = delete;
            logical_device& operator=( logical_device&& logical_device ) noexcept;

            VkQueue get_queue( int32_t family_index, uint32_t queue_index ) const;

            VkCommandPool create_command_pool( VkCommandPoolCreateInfo& create_info ) const;
            VkCommandPool destroy_command_pool( VkCommandPool& command_pool_handle ) const;

            VkCommandBuffer allocate_command_buffer( VkCommandBufferAllocateInfo& allocate_info ) const;
            VkCommandBuffer free_command_buffer( const VkCommandPool& command_pool_handle, VkCommandBuffer& command_buffer_handle ) const;
            std::vector<VkCommandBuffer> allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t number ) const;
            std::vector<VkCommandBuffer> free_command_buffers( const VkCommandPool& command_pool_handle, std::vector<VkCommandBuffer>& command_buffer_handles ) const;

            VkSemaphore create_semaphore( VkSemaphoreCreateInfo& create_info ) const;
            VkSemaphore destroy_semaphore( VkSemaphore& semaphore_handle ) const;

            VkFence create_fence( VkFenceCreateInfo& create_info ) const;
            VkFence destroy_fence( VkFence& fence_handle ) const;

            void wait_for_fences( VkFence* p_fence_handle, uint32_t fence_count, VkBool32 wait_all, uint64_t timeout ) const;
            void reset_fences( VkFence* p_fence_handle, uint32_t fence_count ) const;

            VkSwapchainKHR create_swapchain( VkSwapchainCreateInfoKHR& create_info ) const;
            VkSwapchainKHR destroy_swapchain( VkSwapchainKHR& swapchain_handle ) const;

            VkImage* create_images( VkSwapchainKHR& swapchain_handle, uint32_t& count ) const;
            VkImage* destroy_images( VkImage* image_handles ) const;

            VkImageView* create_image_views( VkImage* image_handles, VkImageViewCreateInfo& create_info, uint32_t count ) const;
            VkImageView* destroy_image_views( VkImageView* image_view_handles, uint32_t count ) const;

            VkRenderPass create_render_pass( VkRenderPassCreateInfo& create_info ) const;
            VkRenderPass destroy_render_pass( VkRenderPass& render_pass_handle ) const;

            VkFramebuffer* create_frame_buffers( VkImageView* image_view_handles, VkFramebufferCreateInfo& create_info, uint32_t count ) const;
            VkFramebuffer* destroy_frame_buffers( VkFramebuffer* frame_buffer_handles, uint32_t count ) const;

            VkPipelineLayout create_pipeline_layout( VkPipelineLayoutCreateInfo& create_info ) const;
            VkPipelineLayout destroy_pipeline_layout( VkPipelineLayout& pipeline_layout_handle ) const;

            VkPipelineCache create_pipeline_cache( VkPipelineCacheCreateInfo& create_info ) const;
            VkPipelineCache destroy_pipeline_cache( VkPipelineCache& pipeline_cache_handle ) const;

            VkPipeline create_compute_pipeline( VkPipelineCache pipeline_cache_handle, VkComputePipelineCreateInfo& create_info ) const;
            VkPipeline create_graphics_pipeline( VkPipelineCache pipeline_cache_handle, VkGraphicsPipelineCreateInfo& create_info ) const;
            VkPipeline destroy_pipeline( VkPipeline pipeline_handle ) const;

            VkShaderModule create_shader_module( VkShaderModuleCreateInfo& create_info ) const;
            VkShaderModule destroy_shader_module( VkShaderModule& shader_module_handle ) const;

        private:
            VkDevice device_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_LOGICALDEVICE_H
