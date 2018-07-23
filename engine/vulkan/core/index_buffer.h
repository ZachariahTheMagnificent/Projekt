/*!
 *
 */

#ifndef PROJEKT_INDEX_BUFFER_H
#define PROJEKT_INDEX_BUFFER_H

#include "logical_device.h"
#include "command_pool.h"
#include "queue.h"
#include "../graphics/vertex.h"

namespace vk
{
    namespace core
    {
        class index_buffer
        {
        public:
            index_buffer( ) = default;
            index_buffer( const logical_device* p_logical_device, const physical_device& physical_device,
                          const command_pool& command_pool, queue& queue,
                          const std::vector<std::uint16_t>& indices );
            index_buffer( const index_buffer& index_buffer ) = delete;
            index_buffer( index_buffer&& index_buffer ) noexcept;
            ~index_buffer( );

            VkBuffer& get()
            {
                return buffer_handle_;
            }

            uint32_t get_count()
            {
                return count_;
            }

            index_buffer& operator=( const index_buffer& index_buffer ) = delete;
            index_buffer& operator=( index_buffer&& index_buffer ) noexcept;

        private:
            void create_buffer( const physical_device& physical_device, VkDeviceSize& size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory );
            void copy_buffer( const command_pool& command_pool, queue& queue, VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize &size );

            uint32_t find_memory_type( const physical_device& physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties );

        private:
            const logical_device* p_logical_device_;

            VkBuffer buffer_handle_ = VK_NULL_HANDLE;
            VkDeviceMemory buffer_memory_handle_ = VK_NULL_HANDLE;

            uint32_t count_;
        };
    }
}

#endif //PROJEKT_INDEX_BUFFER_H
