/*!
 *
 */

#ifndef COMPUTE_LOGICALDEVICE_H
#define COMPUTE_LOGICALDEVICE_H

#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"


namespace Vk
{
    namespace Core
    {
        class LogicalDevice
        {
        public:
            LogicalDevice( ) = default;
            LogicalDevice( PhysicalDevice& physical_device,
                           const std::vector<const char*>& validation_layers,
                           const std::vector<const char*>& device_extensions );
            LogicalDevice( const LogicalDevice& logical_device ) = delete;
            LogicalDevice( LogicalDevice&& logical_device ) noexcept;
            ~LogicalDevice( );

            VkDevice get()
            {
                return device_handle_;
            }

            LogicalDevice& operator=( const LogicalDevice& logical_device ) = delete;
            LogicalDevice& operator=( LogicalDevice&& logical_device ) noexcept;

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

            VkShaderModule create_shader_module( VkShaderModuleCreateInfo& create_info ) const;
            VkShaderModule destroy_shader_module( VkShaderModule& shader_module_handle ) const;

        private:
            VkDevice device_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_LOGICALDEVICE_H
