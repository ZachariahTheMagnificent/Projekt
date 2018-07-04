/*!
 *
 */

#include "command_buffers.h"

namespace vk
{
    namespace core
    {
        command_buffers::command_buffers( const command_pool* p_command_pool, size_t count )
                :
                p_command_pool_( p_command_pool ),
                count_( count )
        {
            VkCommandBufferAllocateInfo allocate_info = {};
            allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocate_info.commandBufferCount = static_cast<uint32_t>( count_ );

            command_buffer_handles_ = p_command_pool_->allocate_command_buffers( allocate_info, count_ );
        }
        command_buffers::command_buffers( command_buffers&& command_buffers ) noexcept
        {
            *this = std::move( command_buffers );
        }
        command_buffers::~command_buffers( )
        {
            if( command_buffer_handles_ != VK_NULL_HANDLE )
                command_buffer_handles_ = p_command_pool_->free_command_buffers( command_buffer_handles_, count_ );
        }

        command_buffers&
        command_buffers::operator=( command_buffers&& command_buffers ) noexcept
        {
            if( this != &command_buffers )
            {
                if( command_buffer_handles_ != VK_NULL_HANDLE )
                    command_buffer_handles_ = p_command_pool_->free_command_buffers( command_buffer_handles_, count_ );

                count_ = command_buffers.count_;
                command_buffers.count_ = 0;

                command_buffer_handles_ = command_buffers.command_buffer_handles_;
                command_buffers.command_buffer_handles_ = VK_NULL_HANDLE;

                p_command_pool_ = command_buffers.p_command_pool_;
            }

            return *this;
        }

        const VkCommandBuffer&
        command_buffers::operator[]( size_t i )
        {
            return command_buffer_handles_[i];
        }

        void
        command_buffers::begin( VkCommandBufferUsageFlags flags, uint32_t index )
        {
            VkCommandBufferBeginInfo command_buffer_begin_info = {};
            command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            command_buffer_begin_info.flags = flags;

            if( vkBeginCommandBuffer( command_buffer_handles_[index], &command_buffer_begin_info ) != VK_SUCCESS )
                std::cerr << "Failed to begin recording command buffer." << std::endl;
        }
        void
        command_buffers::end( uint32_t index )
        {
            if( vkEndCommandBuffer( command_buffer_handles_[index] ) != VK_SUCCESS )
                std::cerr << "Failed to end recording command buffer." << std::endl;
        }

        void
        command_buffers::begin_render_pass( VkRenderPassBeginInfo& begin_info, VkSubpassContents contents, uint32_t index )
        {
            vkCmdBeginRenderPass( command_buffer_handles_[index], &begin_info, contents );
        }
        void
        command_buffers::end_render_pass( uint32_t index )
        {
            vkCmdEndRenderPass( command_buffer_handles_[index] );

        }

        void
        command_buffers::bind_pipeline( VkPipelineBindPoint pipeline_bind_point, VkPipeline& pipeline_handle, uint32_t index )
        {
            vkCmdBindPipeline( command_buffer_handles_[index], pipeline_bind_point, pipeline_handle );
        }
        void
        command_buffers::bind_vertex_buffers( uint32_t first_binding, uint32_t binding_count, VkBuffer* p_buffers,
                                             VkDeviceSize* p_offset, uint32_t index )
        {
            vkCmdBindVertexBuffers( command_buffer_handles_[index], first_binding, binding_count, p_buffers, p_offset );
        }
        void
        command_buffers::bind_index_buffer( VkBuffer& buffer, VkDeviceSize offset, VkIndexType index_type,
                                                 uint32_t index )
        {
            vkCmdBindIndexBuffer( command_buffer_handles_[index], buffer, offset, index_type );
        }

        void
        command_buffers::draw_indexed( uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                            int32_t vertex_offset, uint32_t first_instance, uint32_t index )
        {
            vkCmdDrawIndexed( command_buffer_handles_[index], index_count, instance_count, first_index, vertex_offset, first_instance );
        }

        void
        command_buffers::set_viewport( uint32_t first_viewport, uint32_t viewport_count, VkViewport* p_viewports, uint32_t index )
        {
            vkCmdSetViewport( command_buffer_handles_[index], first_viewport, viewport_count, p_viewports );
        }

        void
        command_buffers::copy_buffer( VkBuffer& src_buffer, VkBuffer& dst_buffer, uint32_t region_count,
                                           const VkBufferCopy* p_regions, uint32_t index )
        {
            vkCmdCopyBuffer( command_buffer_handles_[index], src_buffer, dst_buffer, region_count, p_regions );
        }
    }
}