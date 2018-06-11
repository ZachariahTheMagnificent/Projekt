/*!
 *
 */

#ifndef PROJEKT_QUEUEFAMILYINDICES_H
#define PROJEKT_QUEUEFAMILYINDICES_H

#include <vulkan/vulkan.h>

namespace Vk
{
    namespace Helpers
    {
        struct QueueFamilyIndices
        {
            int32_t graphics_family = -1;
            int32_t present_family = -1;

            bool is_complete()
            {
                return graphics_family >= 0 && present_family >= 0;
            }
        };
    }
}

#endif //PROJEKT_QUEUEFAMILYINDICES_H
