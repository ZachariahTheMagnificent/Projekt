/*!
 *
 */

#ifndef COMPUTE_COMMANDPOOL_H
#define COMPUTE_COMMANDPOOL_H

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"

namespace Vk
{
    namespace Core
    {
        class CommandPool
        {
        public:
            CommandPool( ) = default;
            CommandPool( const PhysicalDevice& physical_device,
                         const LogicalDevice* p_logical_device,
                         const Helpers::QueueFamilyType& type );
            CommandPool( const CommandPool& command_pool ) = delete;
            CommandPool( CommandPool&& command_pool ) noexcept;
            ~CommandPool( );

            VkCommandPool& get()
            {
                return command_pool_handle_;
            }

            VkCommandBuffer allocate_command_buffer( VkCommandBufferAllocateInfo& allocate_info ) const;
            VkCommandBuffer free_command_buffer( VkCommandBuffer& command_buffer_handle ) const;

            std::vector<VkCommandBuffer> allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t number ) const;
            std::vector<VkCommandBuffer> free_command_buffers( std::vector<VkCommandBuffer>& command_buffer_handles ) const;

            CommandPool& operator=( const CommandPool& command_pool ) = delete;
            CommandPool& operator=( CommandPool&& command_pool ) noexcept;

        private:
            const LogicalDevice* p_logical_device_;

            VkCommandPool command_pool_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_COMMANDPOOL_H
