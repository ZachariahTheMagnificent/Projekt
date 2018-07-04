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
        class command_buffers
        {
        public:
            command_buffers( ) = default;
            command_buffers( const command_pool* p_command_pool, size_t count );
            command_buffers( const command_buffers& command_buffers ) = delete;
            command_buffers( command_buffers&& command_buffers ) noexcept;
            ~command_buffers( );

            void begin( VkCommandBufferUsageFlags flags, uint32_t index );
            void end( uint32_t index );

            void begin_render_pass( VkRenderPassBeginInfo& begin_info, VkSubpassContents contents, uint32_t index );
            void end_render_pass( uint32_t index );

            void copy_buffer( VkBuffer& src_buffer, VkBuffer& dst_buffer, uint32_t region_count, const VkBufferCopy* p_regions, uint32_t index );

            void set_viewport( uint32_t first_viewport, uint32_t viewport_count, VkViewport* p_viewports, uint32_t index );

            void bind_pipeline( VkPipelineBindPoint pipeline_bind_point, VkPipeline& pipeline_handle, uint32_t index );
            void bind_vertex_buffers( uint32_t first_binding, uint32_t binding_count, VkBuffer* p_buffers, VkDeviceSize* p_offset, uint32_t index );
            void bind_index_buffer( VkBuffer& buffer, VkDeviceSize offset, VkIndexType index_type, uint32_t index );

            void draw_indexed( uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                               int32_t vertex_offset, uint32_t first_instance, uint32_t index );

            command_buffers& operator=( const command_buffers& command_buffers ) = delete;
            command_buffers& operator=( command_buffers&& command_buffers ) noexcept;

            const VkCommandBuffer& operator[]( size_t i );

            size_t get_count()
            {
                return count_;
            }

        private:
            const command_pool* p_command_pool_;

            VkCommandBuffer* command_buffer_handles_ = VK_NULL_HANDLE;
            size_t count_;
        };

    }
}

#endif //COMPUTE_COMMANDBUFFERS_H