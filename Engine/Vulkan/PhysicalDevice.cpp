/*!
 *
 */

#include <set>

#include "PhysicalDevice.h"
#include "../Utils/Exception/VulkanException.h"

namespace Vk
{
    PhysicalDevice::PhysicalDevice( const Instance* p_instance, Surface* p_surface )
        :
        p_surface_( p_surface )
    {
        auto available_devices = p_instance->enumerate_physical_devices();

        for( auto& device : available_devices )
        {
            if( is_device_suitable( device ) )
            {
                physical_device_handle_ = device;
                break;
            }
        }

        vkGetPhysicalDeviceProperties( physical_device_handle_, &physical_device_properties );
    }
    PhysicalDevice::PhysicalDevice( PhysicalDevice &&physical_device ) noexcept
    {
        *this = std::move( physical_device );
    }

    PhysicalDevice &PhysicalDevice::operator=( PhysicalDevice &&physical_device ) noexcept
    {
        if( this != &physical_device )
        {
            p_surface_ = physical_device.p_surface_;

            physical_device_handle_ = physical_device.physical_device_handle_;
            physical_device.physical_device_handle_ = VK_NULL_HANDLE;

            physical_device_properties = physical_device.physical_device_properties;
        }

        return *this;
    }

    VkDevice PhysicalDevice::create_device( const VkDeviceCreateInfo& create_info ) const
    {
        VkDevice device_handle = VK_NULL_HANDLE;

        if( vkCreateDevice( physical_device_handle_, &create_info, nullptr, &device_handle ) != VK_NULL_HANDLE )
            throw VulkanException{ "Failed to create logical device", __FILE__, __LINE__  };

        return device_handle;
    }

    Helpers::SwapchainSupportDetails PhysicalDevice::query_swapchain_support( )
    {
        return p_surface_->query_swapchain_support( physical_device_handle_ );
    }

    Helpers::QueueFamilyIndices PhysicalDevice::find_queue_family_indices( )
    {
        return p_surface_->find_queue_family_indices( physical_device_handle_ );
    }

    bool PhysicalDevice::is_device_suitable( VkPhysicalDevice &physical_device_handle )
    {
        auto queue_family_indices = find_queue_family_indices( physical_device_handle );

        bool extension_supported = check_device_extension_support( physical_device_handle );
        bool swapchain_adequate = false;

        if( extension_supported )
        {
            auto details = query_swapchain_support( physical_device_handle );
            swapchain_adequate = !details.formats.empty() && !details.present_modes.empty();
        }

        return queue_family_indices.is_complete() && swapchain_adequate && extension_supported;
    }

    bool PhysicalDevice::check_device_extension_support( VkPhysicalDevice &physical_device_handle )
    {
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties( physical_device_handle, nullptr, &extension_count, nullptr );

        std::vector<VkExtensionProperties> available_extensions( extension_count );
        vkEnumerateDeviceExtensionProperties( physical_device_handle, nullptr, &extension_count, available_extensions.data() );

        std::set<std::string> required_extensions( device_extensions.begin(), device_extensions.end() );

        for( const auto& extension : available_extensions )
        {
            required_extensions.erase( extension.extensionName );
        }

        return required_extensions.empty();
    }

    Helpers::SwapchainSupportDetails PhysicalDevice::query_swapchain_support( VkPhysicalDevice &physical_device_handle )
    {
        return p_surface_->query_swapchain_support( physical_device_handle );
    }

    Helpers::QueueFamilyIndices PhysicalDevice::find_queue_family_indices( VkPhysicalDevice &physical_device_handle )
    {
        return p_surface_->find_queue_family_indices( physical_device_handle );
    }

    VkPhysicalDeviceMemoryProperties PhysicalDevice::get_physical_device_memory_properties( ) const
    {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties( physical_device_handle_, &properties );

        return properties;
    }
}