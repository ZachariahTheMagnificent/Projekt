/*!
 *
 */

#include "CommandPool.h"

namespace Vk
{
    namespace Core
    {
        CommandPool::CommandPool( const PhysicalDevice& physical_device,
                                  const LogicalDevice* p_logical_device,
                                  const Helpers::QueueFamilyType& type )
                :
                p_logical_device_( p_logical_device )
        {
            VkCommandPoolCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            create_info.queueFamilyIndex = static_cast<uint32_t>( physical_device.get_queue_family_index( type ) );

            command_pool_handle_ = p_logical_device_->create_command_pool( create_info );
        }
        CommandPool::CommandPool( CommandPool&& command_pool ) noexcept
        {
            *this = std::move( command_pool );
        }
        CommandPool::~CommandPool( )
        {
            if( command_pool_handle_ != VK_NULL_HANDLE )
                command_pool_handle_ = p_logical_device_->destroy_command_pool( command_pool_handle_ );
        }

        VkCommandBuffer
        CommandPool::allocate_command_buffer( VkCommandBufferAllocateInfo& allocate_info ) const
        {
            allocate_info.commandPool = command_pool_handle_;

            return p_logical_device_->allocate_command_buffer( allocate_info );
        }
        VkCommandBuffer
        CommandPool::free_command_buffer( VkCommandBuffer& command_buffer_handle ) const
        {
            return p_logical_device_->free_command_buffer( command_pool_handle_, command_buffer_handle );
        }

        std::vector<VkCommandBuffer>
        CommandPool::allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t number ) const
        {
            allocate_info.commandPool = command_pool_handle_;

            return p_logical_device_->allocate_command_buffers( allocate_info, number );
        }
        std::vector<VkCommandBuffer>
        CommandPool::free_command_buffers( std::vector<VkCommandBuffer>& command_buffer_handles ) const
        {
            return p_logical_device_->free_command_buffers( command_pool_handle_, command_buffer_handles );
        }

        CommandPool& CommandPool::operator=( CommandPool&& command_pool ) noexcept
        {
            if( this != &command_pool )
            {
                if( command_pool_handle_ != VK_NULL_HANDLE )
                    command_pool_handle_ = p_logical_device_->destroy_command_pool( command_pool_handle_ );

                command_pool_handle_ = command_pool.command_pool_handle_;
                command_pool.command_pool_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = command_pool.p_logical_device_;
            }

            return *this;
        }
    }
}
