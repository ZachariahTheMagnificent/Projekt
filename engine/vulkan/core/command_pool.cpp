/*!
 *
 */

#include "command_pool.h"

namespace vk
{
    namespace core
    {
        command_pool::command_pool( const physical_device& physical_device,
                                  const logical_device* p_logical_device,
                                  const helpers::queue_family_type& type )
                :
                p_logical_device_( p_logical_device )
        {
            VkCommandPoolCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            create_info.queueFamilyIndex = static_cast<uint32_t>( physical_device.get_queue_family_index( type ) );

            command_pool_handle_ = p_logical_device_->create_command_pool( create_info );
        }
        command_pool::command_pool( command_pool&& command_pool ) noexcept
        {
            *this = std::move( command_pool );
        }
        command_pool::~command_pool( )
        {
            if( command_pool_handle_ != VK_NULL_HANDLE )
                command_pool_handle_ = p_logical_device_->destroy_command_pool( command_pool_handle_ );
        }


        VkCommandBuffer*
        command_pool::allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t count ) const
        {
            allocate_info.commandPool = command_pool_handle_;

            return p_logical_device_->allocate_command_buffers( allocate_info, count );
        }

        VkCommandBuffer*
        command_pool::free_command_buffers( VkCommandBuffer* command_buffer_handles, uint32_t count ) const
        {
            return p_logical_device_->free_command_buffers( command_pool_handle_, command_buffer_handles, count );
        }

        command_pool&
        command_pool::operator=( command_pool&& command_pool ) noexcept
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
