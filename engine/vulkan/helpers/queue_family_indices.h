/*!
 *
 */

#ifndef COMPUTE_QUEUEFAMILYINDICES_H
#define COMPUTE_QUEUEFAMILYINDICES_H

#include <vulkan/vulkan.h>

namespace vk
{
    namespace helpers
    {
        enum class queue_family_type
        {
            e_graphics,
            e_present,
            e_compute
        };

        struct queue_family_indices
        {
            int32_t graphics_family = -1;
            int32_t present_family = -1;
            int32_t compute_family = -1;

            bool is_graphics_complete()
            {
                return graphics_family >= 0 && present_family >= 0;
            }
            bool is_compute_complete()
            {
                return compute_family >= 0;
            }
            bool is_complete()
            {
                return is_graphics_complete() && is_compute_complete();
            }
        };
    }
}

#endif //COMPUTE_QUEUEFAMILYINDICES_H
