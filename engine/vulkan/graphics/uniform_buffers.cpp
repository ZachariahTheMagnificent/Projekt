/*!
 *
 */

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

#include "uniform_buffers.h"

#include "uniform_buffer_object.h"
#include "../../utils/exception/vulkan_exception.h"

namespace vk
{
    namespace graphics
    {
        uniform_buffers::uniform_buffers( const core::logical_device* p_logical_device,
                                          const core::physical_device& physical_device,
                                          uint32_t count )
            :
            p_logical_device_( p_logical_device ),
            count_( count )
        {
            VkDeviceSize buffer_size = sizeof( uniform_buffer_object );

            buffer_handles_ = new VkBuffer[count_];
            memory_handles_ = new VkDeviceMemory[count_];

            for( auto i = 0; i < count_; ++i )
            {
                create_buffer( physical_device, buffer_size,
                               VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               buffer_handles_[i], memory_handles_[i] );
            }
        }
        uniform_buffers::uniform_buffers( uniform_buffers&& uniform_buffers ) noexcept
        {
            *this = std::move( uniform_buffers );
        }

        uniform_buffers::~uniform_buffers( )
        {
            for( auto i = 0; i < count_; ++i )
            {
                if( buffer_handles_[i] != VK_NULL_HANDLE )
                    buffer_handles_[i] = p_logical_device_->destroy_buffer( buffer_handles_[i] );

                if( memory_handles_[i] != VK_NULL_HANDLE )
                    memory_handles_[i] = p_logical_device_->free_memory( memory_handles_[i] );
            }
        }

        void
        uniform_buffers::update( glm::mat4& model_matrix, glm::mat4& view_matrix, glm::mat4& proj_matrix, uint32_t index )
        {
            uniform_buffer_object ubo = {};
            ubo.model           = model_matrix;
            ubo.view            = view_matrix;
            ubo.proj            = proj_matrix;
            ubo.proj[1][1]      *= -1;

            VkDeviceSize offset = 0;
            VkDeviceSize size = sizeof( ubo );
            VkMemoryMapFlags flags = 0;

            void* data;
            p_logical_device_->map_memory( memory_handles_[index], offset, size, flags, &data );
            {
                memcpy( data, &ubo, sizeof( ubo ) );
            }
            p_logical_device_->unmap_memory( memory_handles_[index] );

        }

        uniform_buffers&
        uniform_buffers::operator=( uniform_buffers&& uniform_buffers ) noexcept
        {
            if( this != &uniform_buffers )
            {
                for( auto i = 0; i < count_; ++i )
                {
                    if( buffer_handles_[i] != VK_NULL_HANDLE )
                        buffer_handles_[i] = p_logical_device_->destroy_buffer( buffer_handles_[i] );

                    if( memory_handles_[i] != VK_NULL_HANDLE )
                        memory_handles_[i] = p_logical_device_->free_memory( memory_handles_[i] );
                }

                count_ = uniform_buffers.count_;
                uniform_buffers.count_ = 0;

                buffer_handles_ = uniform_buffers.buffer_handles_;
                uniform_buffers.buffer_handles_ = VK_NULL_HANDLE;

                memory_handles_ = uniform_buffers.memory_handles_;
                uniform_buffers.memory_handles_ = VK_NULL_HANDLE;

                p_logical_device_ = uniform_buffers.p_logical_device_;
            }

            return *this;
        }


        void
        uniform_buffers::create_buffer( const core::physical_device& physical_device, VkDeviceSize& size,
                                        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                        VkBuffer& buffer, VkDeviceMemory& buffer_memory )
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

        uint32_t
        uniform_buffers::find_memory_type( const core::physical_device& physical_device, uint32_t type_filter,
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

            throw vulkan_exception{ "Failed to find a suitable memory type.", __FILE__, __LINE__ };
        }
    }
}