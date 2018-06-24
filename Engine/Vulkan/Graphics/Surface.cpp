/*!
 *
 */

#include "Surface.h"

namespace Vk
{
    namespace Graphics
    {
        Surface::Surface( const Core::Instance* p_instance, const Window& window )
            :
            p_instance_( p_instance )
        {
            surface_handle_ = p_instance_->create_surface( window );
        }
        Surface::Surface( Surface&& surface ) noexcept
        {
            *this = std::move( surface );
        }
        Surface::~Surface( )
        {
            surface_handle_ = p_instance_->destroy_surface( surface_handle_ );
        }

        VkBool32 Surface::get_physical_device_surface_support( VkPhysicalDevice& physical_device_handle,
                                                               uint32_t queue_family_index ) const
        {
            VkBool32 present_support;
            vkGetPhysicalDeviceSurfaceSupportKHR( physical_device_handle, queue_family_index, surface_handle_, &present_support );

            return present_support;
        }

        Surface& Surface::operator=( Surface&& surface ) noexcept
        {
            if( this != &surface )
            {
                if( surface_handle_ != VK_NULL_HANDLE )
                    surface_handle_ = p_instance_->destroy_surface( surface_handle_ );

                surface_handle_ = surface.surface_handle_;
                surface.surface_handle_ = VK_NULL_HANDLE;

                p_instance_ = surface.p_instance_;
            }

            return *this;
        }
    }
}