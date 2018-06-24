/*!
 *
 */

#ifndef COMPUTE_SURFACE_H
#define COMPUTE_SURFACE_H

#include <utility>

#include "../Core/Instance.h"

namespace Vk
{
    namespace Graphics
    {
        class Surface
        {
        public:
            Surface( ) = default;
            Surface( const Core::Instance* p_instance, const Window& window );
            Surface( const Surface& surface ) = delete;
            Surface( Surface&& surface ) noexcept;
            ~Surface( );

            VkBool32 get_physical_device_surface_support( VkPhysicalDevice& physical_device_handle,
                                                          uint32_t queue_family_index ) const;

            VkSurfaceKHR get() const
            {
                return surface_handle_;
            }

            Surface& operator=( const Surface& surface ) = delete;
            Surface& operator=( Surface&& surface ) noexcept;

        private:
            const Core::Instance* p_instance_;

            VkSurfaceKHR surface_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_SURFACE_H
