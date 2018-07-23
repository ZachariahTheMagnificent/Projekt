/*!
 *
 */

#ifndef COMPUTE_COMMANDPOOL_H
#define COMPUTE_COMMANDPOOL_H

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        class command_pool
        {
        public:
            command_pool( ) = default;
            command_pool( const physical_device& physical_device,
                         const logical_device* p_logical_device,
                         const helpers::queue_family_type& type );
            command_pool( const command_pool& command_pool ) = delete;
            command_pool( command_pool&& command_pool ) noexcept;
            ~command_pool( );

            VkCommandPool& get()
            {
                return command_pool_handle_;
            }

            VkCommandBuffer* allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t count ) const;
            VkCommandBuffer* free_command_buffers( VkCommandBuffer* command_buffer_handles, uint32_t count ) const;

            command_pool& operator=( const command_pool& command_pool ) = delete;
            command_pool& operator=( command_pool&& command_pool ) noexcept;

        private:
            const logical_device* p_logical_device_;

            VkCommandPool command_pool_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_COMMANDPOOL_H
