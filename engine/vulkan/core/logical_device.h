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

            void wait_idle();

            logical_device& operator=( const logical_device& logical_device ) = delete;
            logical_device& operator=( logical_device&& logical_device ) noexcept;

            VkQueue get_queue( int32_t family_index, uint32_t queue_index ) const;

            VkCommandPool create_command_pool( VkCommandPoolCreateInfo& create_info ) const;
            VkCommandPool destroy_command_pool( VkCommandPool& command_pool_handle ) const;

            VkCommandBuffer* allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t count ) const;
            VkCommandBuffer* free_command_buffers( const VkCommandPool& command_pool_handle, VkCommandBuffer* command_buffer_handles, uint32_t count ) const;

            VkSemaphore* create_semaphores( VkSemaphoreCreateInfo& create_info, uint32_t count ) const;
            VkSemaphore* destroy_semaphores( VkSemaphore* semaphore_handles, uint32_t count ) const;

            VkFence* create_fences( VkFenceCreateInfo& create_info, uint32_t count ) const;
            VkFence* destroy_fences( VkFence* fence_handle, uint32_t count ) const;

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

            VkFramebuffer* create_frame_buffers( const VkImageView* image_view_handles, VkFramebufferCreateInfo& create_info, uint32_t count ) const;
            VkFramebuffer* destroy_frame_buffers( VkFramebuffer* frame_buffer_handles, uint32_t count ) const;

            VkPipelineLayout create_pipeline_layout( VkPipelineLayoutCreateInfo& create_info ) const;
            VkPipelineLayout destroy_pipeline_layout( VkPipelineLayout& pipeline_layout_handle ) const;

            VkPipelineCache create_pipeline_cache( VkPipelineCacheCreateInfo& create_info ) const;
            VkPipelineCache destroy_pipeline_cache( VkPipelineCache& pipeline_cache_handle ) const;

            VkPipeline create_compute_pipeline( VkPipelineCache pipeline_cache_handle, VkComputePipelineCreateInfo& create_info ) const;
            VkPipeline create_graphics_pipeline( VkPipelineCache pipeline_cache_handle, VkGraphicsPipelineCreateInfo& create_info ) const;
            VkPipeline destroy_pipeline( VkPipeline& pipeline_handle ) const;

            VkShaderModule create_shader_module( VkShaderModuleCreateInfo& create_info ) const;
            VkShaderModule destroy_shader_module( VkShaderModule& shader_module_handle ) const;

            VkBuffer create_buffer( VkBufferCreateInfo& create_info ) const;
            VkBuffer destroy_buffer( VkBuffer& buffer_handle ) const;

            VkMemoryRequirements get_buffer_memory_requirements( VkBuffer& buffer_handle ) const;

            VkDeviceMemory allocate_memory( VkMemoryAllocateInfo& allocate_info ) const;
            VkDeviceMemory free_memory( VkDeviceMemory& memory_handle ) const;

            void bind_buffer_memory( VkBuffer& buffer_handle, VkDeviceMemory& memory_handle, VkDeviceSize& offset ) const;

            void map_memory( VkDeviceMemory& memory_handle, VkDeviceSize& offset, VkDeviceSize& size, VkMemoryMapFlags& flags, void **pp_data ) const;
            void unmap_memory( VkDeviceMemory& memory_handle ) const;

            VkDescriptorSetLayout create_descriptor_set_layout( VkDescriptorSetLayoutCreateInfo& create_info ) const;
            VkDescriptorSetLayout destroy_descriptor_set_layout( VkDescriptorSetLayout& descriptor_set_layout_handle ) const;

            VkDescriptorSet* allocate_descriptor_sets_( VkDescriptorSetAllocateInfo& allocate_info, uint32_t count ) const;
            VkDescriptorSet* free_descriptor_sets_( const VkDescriptorPool& descriptor_pool_handle, VkDescriptorSet* descriptor_set_handles, uint32_t count ) const;

            void update_descriptor_set( uint32_t descriptor_write_count, const VkWriteDescriptorSet* p_descriptor_writes,
                                        uint32_t descriptor_copy_count, const VkCopyDescriptorSet* p_descriptor_copies ) const;

            VkDescriptorPool create_descriptor_pool( VkDescriptorPoolCreateInfo& create_info ) const;
            VkDescriptorPool destroy_descriptor_pool( VkDescriptorPool& descriptor_pool_handle ) const;

        private:
            VkDevice device_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_LOGICALDEVICE_H
