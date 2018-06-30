/*!
 *
 */

#include "surface.h"

namespace vk
{
    namespace graphics
    {
        surface::surface( const core::instance* p_instance, const window& window )
            :
            width_( window.get_width() ),
            height_( window.get_height() ),
            p_instance_( p_instance )
        {
            surface_handle_ = p_instance_->create_surface( window );
        }
        surface::surface( surface&& surface ) noexcept
        {
            *this = std::move( surface );
        }
        surface::~surface( )
        {
            surface_handle_ = p_instance_->destroy_surface( surface_handle_ );
        }

        VkBool32
        surface::get_physical_device_surface_support( VkPhysicalDevice& physical_device_handle,
                                                               uint32_t queue_family_index ) const
        {
            VkBool32 present_support;
            vkGetPhysicalDeviceSurfaceSupportKHR( physical_device_handle, queue_family_index, surface_handle_, &present_support );

            return present_support;
        }

        VkSurfaceCapabilitiesKHR
        surface::get_capabilities( const VkPhysicalDevice& physical_device_handle ) const noexcept
        {
            VkSurfaceCapabilitiesKHR capabilities;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physical_device_handle, surface_handle_, &capabilities );

            return capabilities;
        }
        std::vector<VkSurfaceFormatKHR>
        surface::get_format( const VkPhysicalDevice& physical_device_handle ) const noexcept
        {
            std::vector<VkSurfaceFormatKHR> formats;

            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device_handle, surface_handle_, &format_count, nullptr );

            if( format_count != 0 )
            {
                formats.resize( format_count );
                vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device_handle, surface_handle_, &format_count, formats.data() );
            }

            return formats;
        }
        std::vector<VkPresentModeKHR>
        surface::get_present_mode( const VkPhysicalDevice& physical_device_handle ) const noexcept
        {
            std::vector<VkPresentModeKHR> present_modes;

            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device_handle, surface_handle_, &present_mode_count, nullptr );

            if( present_mode_count != 0 )
            {
                present_modes.resize( present_mode_count );
                vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device_handle, surface_handle_, &present_mode_count, present_modes.data() );
            }

            return present_modes;
        }

        const uint32_t surface::get_width( ) const noexcept
        {
            return width_;
        }

        const uint32_t surface::get_height( ) const noexcept
        {
            return height_;
        }

        surface&
        surface::operator=( surface&& surface ) noexcept
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