/*!
 *
 */

#ifndef COMPUTE_COMMANDBUFFERS_H
#define COMPUTE_COMMANDBUFFERS_H

#include <vulkan/vulkan.h>

#include "command_pool.h"

namespace vk
{
    namespace core
    {
        template<size_t size>
        class command_buffers
        {
        public:
            command_buffers( ) = default;
            explicit command_buffers( const command_pool* p_command_pool )
                    :
                    p_command_pool_( p_command_pool )
            {
                command_buffer_handles_.reserve( num_elems );

                VkCommandBufferAllocateInfo allocate_info = {};
                allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocate_info.commandBufferCount = static_cast<uint32_t>( num_elems );

                command_buffer_handles_ = p_command_pool_->allocate_command_buffers( allocate_info, num_elems );
            }
            command_buffers( const command_buffers& command_buffers ) = delete;
            command_buffers( command_buffers&& command_buffers ) noexcept
            {
                *this = std::move( command_buffers );
            }
            ~command_buffers( )
            {
                command_buffer_handles_ = p_command_pool_->free_command_buffers( command_buffer_handles_ );
            }


            command_buffers& operator=( const command_buffers& command_buffers ) = delete;
            command_buffers& operator=( command_buffers&& command_buffers ) noexcept
            {
                if( this != &command_buffers )
                {
                    if( command_buffer_handles_.size() != 0 )
                        command_buffer_handles_ = p_command_pool_->free_command_buffers( command_buffer_handles_ );

                    command_buffer_handles_ = command_buffers.command_buffer_handles_;
                    command_buffers.command_buffer_handles_ = std::vector<VkCommandBuffer>( );

                    p_command_pool_ = command_buffers.p_command_pool_;
                }

                return *this;
            }

        private:
            const command_pool* p_command_pool_;

            std::vector<VkCommandBuffer> command_buffer_handles_ = {};
            size_t num_elems = size;
        };

    }
}

#endif //COMPUTE_COMMANDBUFFERS_H