/*!
 *
 */

#ifndef PROJEKT_TEXTURE_IMAGE_H
#define PROJEKT_TEXTURE_IMAGE_H

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "../core/logical_device.h"

#include "../../utils/exception/exception.h"

namespace vk
{
    namespace graphics
    {
        class texture_image
        {
        public:
            texture_image( const core::logical_device& logical_device, const core::physical_device& physical_device,
                           const std::string& image_path );

        private:
            void create_buffer( const core::logical_device& logical_device, const core::physical_device& physical_device,
                                VkDeviceSize& size, VkBufferUsageFlags&& usage,
                                VkMemoryPropertyFlags&& properties, VkBuffer& buffer,
                                VkDeviceMemory& buffer_memory );

            uint32_t find_memory_type( const core::physical_device& physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties );

        private:
            int width_;
            int height_;
            int channel_;

            VkImage texture_image_handle_ = VK_NULL_HANDLE;
            VkDeviceMemory texture_image_memory_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_TEXTURE_IMAGE_H
