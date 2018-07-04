/*!
 *
 */

#ifndef PROJEKT_VERTEX_BUFFER_H
#define PROJEKT_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "../graphics/vertex.h"
#include "command_pool.h"
#include "queue.h"

namespace vk
{
    namespace core
    {
        class vertex_buffer
        {
        public:
            vertex_buffer( ) = default;
            vertex_buffer( const logical_device* p_logical_device, const physical_device& physical_device,
                           const command_pool& command_pool, queue& queue,
                           const std::vector<vk::graphics::vertex>& vertices );
            vertex_buffer( const vertex_buffer& vertex_buffer ) = delete;
            vertex_buffer( vertex_buffer&& vertex_buffer ) noexcept;
            ~vertex_buffer( );

            VkBuffer& get()
            {
                return buffer_handle_;
            }

            vertex_buffer& operator=( const vertex_buffer& vertex_buffer ) = delete;
            vertex_buffer& operator=( vertex_buffer&& vertex_buffer ) noexcept;

        private:
            void create_buffer( const physical_device& physical_device, VkDeviceSize& size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory );
            void copy_buffer( const command_pool& command_pool, queue& queue, VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize &size );

            uint32_t find_memory_type( const physical_device& physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties );
        private:
            const logical_device* p_logical_device_;

            VkBuffer buffer_handle_ = VK_NULL_HANDLE;
            VkDeviceMemory buffer_memory_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_VERTEX_BUFFER_H
