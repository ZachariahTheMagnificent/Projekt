/*!
 *
 */

#ifndef PROJEKT_SWAPCHAINSUPPORTDETAILS_H
#define PROJEKT_SWAPCHAINSUPPORTDETAILS_H

#include <vector>

#include <vulkan/vulkan.h>

namespace vk
{
    namespace helpers
    {
        struct swapchain_support_details
        {
            VkSurfaceCapabilitiesKHR capabilities;

            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> present_modes;
        };
    }
}

#endif //PROJEKT_SWAPCHAINSUPPORTDETAILS_H
