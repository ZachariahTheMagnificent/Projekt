/*!
 *
 */

#ifndef PROJEKT_SWAPCHAINSUPPORTDETAILS_H
#define PROJEKT_SWAPCHAINSUPPORTDETAILS_H

#include <vector>

#include <vulkan/vulkan.h>

namespace Vk
{
    namespace Helpers
    {
        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;

            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> present_modes;
        };
    }
}

#endif //PROJEKT_SWAPCHAINSUPPORTDETAILS_H
