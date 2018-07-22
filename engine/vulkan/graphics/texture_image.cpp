/*!
 *
 */

#include "texture_image.h"
#include "../../utils/exception/vulkan_exception.h"

namespace vk
{
    namespace graphics
    {
        texture_image::texture_image( const core::logical_device& logical_device, const core::physical_device& physical_device,
                                      const std::string& image_path )
        {
            stbi_uc* pixels = stbi_load( image_path.c_str(), &width_, &height_, &channel_, STBI_rgb_alpha );

            VkDeviceSize image_size = static_cast<uint32_t>( width_ * height_ * 4 );

            if( !pixels )
                throw exception{ "Failed to load texture image.", __FILE__, __LINE__ };

            VkBuffer staging_buffer;
            VkDeviceMemory staging_buffer_memory;

            create_buffer( logical_device, physical_device, image_size,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           staging_buffer, staging_buffer_memory );

            void* data;
            logical_device.map_memory( staging_buffer_memory, 0, image_size, 0, &data );
                memcpy( data, pixels, static_cast<size_t>( image_size ) );
            logical_device.unmap_memory( staging_buffer_memory );

            stbi_image_free( pixels );
        }

        void texture_image::create_buffer( const core::logical_device& logical_device,
                                           const core::physical_device& physical_device, VkDeviceSize& size,
                                           VkBufferUsageFlags&& usage, VkMemoryPropertyFlags&& properties,
                                           VkBuffer& buffer, VkDeviceMemory& buffer_memory )
        {
            VkBufferCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            create_info.size = size;
            create_info.usage = usage;
            create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            buffer = logical_device.create_buffer( create_info );

            VkMemoryRequirements mem_reqs = logical_device.get_buffer_memory_requirements( buffer );

            VkMemoryAllocateInfo allocate_info = {};
            allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocate_info.allocationSize = mem_reqs.size;
            allocate_info.memoryTypeIndex = find_memory_type( physical_device, mem_reqs.memoryTypeBits, properties );

            buffer_memory = logical_device.allocate_memory( allocate_info );

            VkDeviceSize offset = 0;
            logical_device.bind_buffer_memory( buffer, buffer_memory, offset );
        }

        uint32_t texture_image::find_memory_type( const core::physical_device& physical_device, uint32_t type_filter,
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