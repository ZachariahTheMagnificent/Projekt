/*!
 *
 */

#include "texture_image.h"

namespace vk
{
    namespace graphics
    {
        texture_image::texture_image( const core::logical_device&, const core::physical_device& physical_device,
                                      const std::string& image_path )
        {
            stbi_uc* pixels = stbi_load( image_path.c_str(), &width_, &height_, &channel_, STBI_rgb_alpha );

            VkDeviceSize image_size = static_cast<uint32_t>( width_ * height_ * 4 );

            if( !pixels )
                throw exception{ "Failed to load texture image.", __FILE__, __LINE__ };

            VkBuffer staging_buffer;
            VkDeviceMemory staging_buffer_memory;


        }

        void texture_image::create_buffer( const core::physical_device& physical_device, VkDeviceSize& size,
                                           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                           VkDeviceMemory& buffer_memory )
        {
            /*
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
            */
        }
    }
}