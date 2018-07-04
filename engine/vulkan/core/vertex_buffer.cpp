/*!
 *
 */

#include <cstring>

#include "vertex_buffer.h"
#include "command_buffers.h"

namespace vk
{
    namespace core
    {
        vertex_buffer::vertex_buffer( const logical_device* p_logical_device, const physical_device& physical_device,
                                      const command_pool& command_pool, queue& queue,
                                      const std::vector<vk::graphics::vertex>& vertices )
            :
            p_logical_device_( p_logical_device )
        {
            VkDeviceSize buffer_size = sizeof( vertices[0] ) * vertices.size();

            VkBuffer staging_buffer;
            VkDeviceMemory staging_buffer_memory;

            create_buffer( physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           staging_buffer, staging_buffer_memory );
            {
                void* data;

                VkDeviceSize offset = 0;
                VkMemoryMapFlags flags = 0;

                p_logical_device_->map_memory( staging_buffer_memory, offset, buffer_size, flags, &data );
                memcpy( data, vertices.data(), static_cast<size_t>( buffer_size ) );
                p_logical_device_->unmap_memory( staging_buffer_memory );
            }

            create_buffer( physical_device, buffer_size,
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           buffer_handle_, buffer_memory_handle_ );

            copy_buffer( command_pool, queue, staging_buffer, buffer_handle_, buffer_size );

            staging_buffer = p_logical_device_->destroy_buffer( staging_buffer );
            staging_buffer_memory = p_logical_device_->free_memory( staging_buffer_memory );
        }
        vertex_buffer::vertex_buffer( vertex_buffer&& vertex_buffer ) noexcept
        {
            *this = std::move( vertex_buffer );
        }
        vertex_buffer::~vertex_buffer( )
        {
            if( buffer_handle_ != VK_NULL_HANDLE )
                buffer_handle_ = p_logical_device_->destroy_buffer( buffer_handle_ );

            if( buffer_memory_handle_ != VK_NULL_HANDLE )
                buffer_memory_handle_ = p_logical_device_->free_memory( buffer_memory_handle_ );
        }

        vertex_buffer&
        vertex_buffer::operator=( vertex_buffer&& vertex_buffer ) noexcept
        {
            if( this != &vertex_buffer )
            {
                if( buffer_handle_ != VK_NULL_HANDLE )
                    buffer_handle_ = p_logical_device_->destroy_buffer( buffer_handle_ );

                if( buffer_memory_handle_ != VK_NULL_HANDLE )
                    buffer_memory_handle_ = p_logical_device_->free_memory( buffer_memory_handle_ );

                buffer_handle_ = vertex_buffer.buffer_handle_;
                vertex_buffer.buffer_handle_ = VK_NULL_HANDLE;

                buffer_memory_handle_ = vertex_buffer.buffer_memory_handle_;
                vertex_buffer.buffer_memory_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = vertex_buffer.p_logical_device_;
            }

            return *this;
        }

        void
        vertex_buffer::create_buffer( const physical_device& physical_device, VkDeviceSize& size, VkBufferUsageFlags usage,
                                      VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory )
        {
            VkBufferCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            create_info.size = size;
            create_info.usage = usage;
            create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            buffer = p_logical_device_->create_buffer( create_info );

            VkMemoryRequirements mem_reqs = p_logical_device_->get_buffer_memory_requirements( buffer );

            VkMemoryAllocateInfo allocate_info = {};
            allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocate_info.allocationSize = mem_reqs.size;
            allocate_info.memoryTypeIndex = find_memory_type( physical_device, mem_reqs.memoryTypeBits, properties );

            buffer_memory = p_logical_device_->allocate_memory( allocate_info );

            VkDeviceSize offset = 0;
            p_logical_device_->bind_buffer_memory( buffer, buffer_memory, offset );
        }

        void
        vertex_buffer::copy_buffer( const command_pool& command_pool, queue& queue, VkBuffer& src_buffer, VkBuffer& dst_buffer, VkDeviceSize& size )
        {
            command_buffers command_buffer( &command_pool, 1 );

            command_buffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 0 );
            {
                VkBufferCopy copy_region = {};
                copy_region.srcOffset = 0;
                copy_region.dstOffset = 0;
                copy_region.size = size;

                command_buffer.copy_buffer( src_buffer, dst_buffer, 1, &copy_region, 0 );
            }
            command_buffer.end( 0 );

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &command_buffer[0];

            queue.submit( submit_info, VK_NULL_HANDLE );
            queue.wait_idle();
        }

        uint32_t
        vertex_buffer::find_memory_type( const physical_device& physical_device, uint32_t type_filter,
                                                  VkMemoryPropertyFlags properties )
        {
            VkPhysicalDeviceMemoryProperties mem_properties = physical_device.get_memory_properties();

            for( uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i )
            {
                if( ( type_filter & ( 1 << i ) ) && ( mem_properties.memoryTypes[i].propertyFlags & properties ) == properties )
                {
                    return i;
                }
            }

            std::cerr << "Failed to find a suitable memory type." << std::endl;
        }
    }
}