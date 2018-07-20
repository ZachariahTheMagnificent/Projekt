/*!
 *
 */

#ifndef PROJEKT_TEXTURE_IMAGE_H
#define PROJEKT_TEXTURE_IMAGE_H

#include <vulkan/vulkan.h>
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
            texture_image( const core::logical_device&, const core::physical_device& physical_device,
                           const std::string& image_path );

        private:
            void create_buffer( const core::physical_device& physical_device, VkDeviceSize& size,
                                VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                VkDeviceMemory& buffer_memory );

        private:
            int width_;
            int height_;
            int channel_;
        };
    }
}

#endif //PROJEKT_TEXTURE_IMAGE_H
