/*!
 *
 */

#ifndef COMPUTE_SURFACE_H
#define COMPUTE_SURFACE_H

#include <utility>

#include "../core/instance.h"

namespace vk
{
    namespace graphics
    {
        class surface
        {
        public:
            surface( ) = default;
            surface( const core::instance* p_instance, const window& window );
            surface( const surface& surface ) = delete;
            surface( surface&& surface ) noexcept;
            ~surface( );

            VkBool32 get_physical_device_surface_support( VkPhysicalDevice& physical_device_handle,
                                                          uint32_t queue_family_index ) const;

            VkSurfaceKHR get() const
            {
                return surface_handle_;
            }

            VkSurfaceCapabilitiesKHR get_capabilities( const VkPhysicalDevice& physical_device_handle ) const noexcept;
            std::vector<VkSurfaceFormatKHR> get_format( const VkPhysicalDevice& physical_device_handle ) const noexcept;
            std::vector<VkPresentModeKHR> get_present_mode( const VkPhysicalDevice& physical_device_handle ) const noexcept;

            surface& operator=( const surface& surface ) = delete;
            surface& operator=( surface&& surface ) noexcept;

        private:
            const core::instance* p_instance_;

            VkSurfaceKHR surface_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_SURFACE_H
