/*!
 *
 */

#ifndef PROJEKT_SURFACE_H
#define PROJEKT_SURFACE_H

#include "Instance.h"
#include "Helpers/QueueFamilyIndices.h"
#include "Helpers/SwapchainSupportDetails.h"

namespace Vk
{
    class Surface
    {
    public:
        Surface() = default;
        Surface( Window& window, const Instance* instance );
        Surface( const Surface& surface ) = delete;
        Surface( Surface&& surface ) noexcept;
        ~Surface();

        Surface& operator=( const Surface& surface ) = delete;
        Surface& operator=( Surface&& surface ) noexcept;

        Helpers::QueueFamilyIndices find_queue_family_indices( VkPhysicalDevice &physical_device_handle );
        Helpers::SwapchainSupportDetails query_swapchain_support( VkPhysicalDevice &physical_device_handle );

        // TODO: Remove
        VkSurfaceKHR get() const {
            return surface_handle_;
        }

    private:
        const Instance* p_instance_;

        VkSurfaceKHR surface_handle_ = VK_NULL_HANDLE;
    };
}

#endif //PROJEKT_SURFACE_H
