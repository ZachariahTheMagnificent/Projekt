/*!
 *
 */

#ifndef PROJEKT_UNIFORM_BUFFER_H
#define PROJEKT_UNIFORM_BUFFER_H

#include "../core/logical_device.h"

namespace vk
{
    namespace graphics
    {
        class uniform_buffers
        {
        public:
            uniform_buffers( ) = default;
            uniform_buffers( const core::logical_device* p_logical_device, const core::physical_device& physical_device, uint32_t count );
            uniform_buffers( const uniform_buffers& uniform_buffers ) = delete;
            uniform_buffers( uniform_buffers&& uniform_buffers ) noexcept;
            ~uniform_buffers( );

            void update( float dt, glm::mat4& proj_matrix, uint32_t image_index );

            const VkBuffer* get()
            {
                return buffer_handles_;
            }

            uniform_buffers& operator=( const uniform_buffers& uniform_buffers ) = delete;
            uniform_buffers& operator=( uniform_buffers&& uniform_buffers ) noexcept;

        private:
            void create_buffer( const core::physical_device& physical_device, VkDeviceSize& size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory );

            uint32_t find_memory_type( const core::physical_device& physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties );

        private:
            const core::logical_device* p_logical_device_;

            VkBuffer* buffer_handles_ = VK_NULL_HANDLE;
            VkDeviceMemory* memory_handles_ = VK_NULL_HANDLE;

            uint32_t count_ = 0;
        };
    }
}

#endif //PROJEKT_UNIFORM_BUFFER_H
