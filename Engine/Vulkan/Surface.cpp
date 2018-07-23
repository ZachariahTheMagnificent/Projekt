/*!
 *
 */

#include "Surface.h"

namespace Vk
{
    Surface::Surface( Window &window, const Instance* instance )
        :
        p_instance_( instance )
    {
        surface_handle_ = p_instance_->create_surface( window );
    }
    Surface::Surface( Surface &&surface ) noexcept
    {
        *this = std::move( surface );
    }

    Surface::~Surface( )
    {
        if( surface_handle_ != VK_NULL_HANDLE )
            surface_handle_ = p_instance_->destroy_surface( surface_handle_ );
    }

    Surface& Surface::operator=( Surface && surface ) noexcept
    {
        if( this != &surface )
        {
            if( surface_handle_ != VK_NULL_HANDLE )
                surface_handle_ = p_instance_->destroy_surface( surface_handle_ );

            surface_handle_ = surface.surface_handle_;
            surface.surface_handle_ = VK_NULL_HANDLE;

            p_instance_ = surface.p_instance_;
        }

        *this;
    }

    Helpers::QueueFamilyIndices Surface::find_queue_family_indices( VkPhysicalDevice &physical_device_handle )
    {
        Helpers::QueueFamilyIndices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device_handle, &queue_family_count, nullptr );

        std::vector<VkQueueFamilyProperties> queue_family_properties( queue_family_count );
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device_handle, &queue_family_count, queue_family_properties.data() );

        int i = 0;
        for( const auto& queue_family_property : queue_family_properties )
        {
            if( queue_family_property.queueCount > 0 && queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT )
                indices.graphics_family = i;

            VkBool32 present_support;
            vkGetPhysicalDeviceSurfaceSupportKHR( physical_device_handle, i, surface_handle_, &present_support );

            if( queue_family_property.queueCount > 0 && present_support )
                indices.present_family = i;

            if( indices.is_complete() )
                break;

            ++i;
        }

        return indices;
    }

    Helpers::SwapchainSupportDetails Surface::query_swapchain_support( VkPhysicalDevice &physical_device_handle )
    {
        Helpers::SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physical_device_handle, surface_handle_, &details.capabilities );

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device_handle, surface_handle_, &format_count, nullptr );

        if( format_count != 0 )
        {
            details.formats.resize( format_count );
            vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device_handle, surface_handle_, &format_count, details.formats.data() );
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device_handle, surface_handle_, &present_mode_count, nullptr );

        if( present_mode_count != 0 )
        {
            details.present_modes.resize( present_mode_count );
            vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device_handle, surface_handle_, &present_mode_count, details.present_modes.data() );
        }

        return details;
    }
}
