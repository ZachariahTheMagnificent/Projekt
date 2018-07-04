/*!
 *
 */

#include <vector>
#include <set>
#include <iostream>

#include "physical_device.h"
#include "../graphics/surface.h"

namespace vk
{
    namespace core
    {
        physical_device::physical_device( instance &instance )
        {
            auto devices = instance.enumerate_physical_devices();

            for( auto& device : devices )
            {
                if( is_device_suitable_for_compute( device ) )
                {
                    physical_device_handle_ = device;
                    break;
                }
            }

            std::cout << "Physical device found:" << std::endl;

            vkGetPhysicalDeviceProperties( physical_device_handle_, &physical_device_properties_ );

            std::cout << "\tName: " << physical_device_properties_.deviceName << "\n\tDriver: " << physical_device_properties_.driverVersion;
            std::cout << "\n\tVendor: " << physical_device_properties_.vendorID << std::endl;
        }
        physical_device::physical_device( instance& instance, const graphics::surface& surface )
        {
            auto devices = instance.enumerate_physical_devices();

            for( auto& device : devices )
            {
                if( is_device_suitable( surface, device ) )
                {
                    physical_device_handle_ = device;
                    break;
                }
            }

            std::cout << "Physical device found:" << std::endl;

            vkGetPhysicalDeviceProperties( physical_device_handle_, &physical_device_properties_ );

            std::cout << "\tName: " << physical_device_properties_.deviceName << "\n\tDriver: " << physical_device_properties_.driverVersion;
            std::cout << "\n\tVendor: " << physical_device_properties_.vendorID << std::endl;
        }
        physical_device::physical_device( instance& instance, VkPhysicalDeviceFeatures& physical_Device_features )
                :
                physical_device( instance )
        {
            set_device_features( physical_Device_features );
        }
        physical_device::physical_device( physical_device &&physical_device ) noexcept
        {
            *this = std::move( physical_device );
        }

        void
        physical_device::set_device_features( VkPhysicalDeviceFeatures& physical_device_features ) noexcept
        {
            physical_device_features_ = physical_device_features;
        }

        int32_t physical_device::get_queue_family_index( const helpers::queue_family_type& type ) const noexcept
        {
            if( type == helpers::queue_family_type::e_graphics )
                return queue_family_indices_.graphics_family;
            else if( type == helpers::queue_family_type::e_present )
                return queue_family_indices_.present_family;
            else if( type == helpers::queue_family_type::e_compute )
                return queue_family_indices_.compute_family;

            return -1;
        }
        std::set<int>
        physical_device::unique_queue_families( ) noexcept
        {
            std::set<int> unique_queue_family;

            if( queue_family_indices_.graphics_family >= 0 )
                unique_queue_family.emplace( queue_family_indices_.graphics_family );

            if( queue_family_indices_.present_family >= 0 )
                unique_queue_family.emplace( queue_family_indices_.present_family );

            if( queue_family_indices_.compute_family >= 0 )
                unique_queue_family.emplace( queue_family_indices_.compute_family );

            return unique_queue_family;
        }

        const VkPhysicalDeviceFeatures&
        physical_device::features( ) noexcept
        {
            return physical_device_features_;
        }

        physical_device&
        physical_device::operator=( physical_device &&physical_device ) noexcept
        {
            if( this != &physical_device );
            {
                physical_device_handle_ = physical_device.physical_device_handle_;
                physical_device.physical_device_handle_ = VK_NULL_HANDLE;

                physical_device_features_ = physical_device.physical_device_features_;

                physical_device_properties_ = physical_device.physical_device_properties_;

                queue_family_indices_ = physical_device.queue_family_indices_;
            }

            return *this;
        }

        VkDevice
        physical_device::create_device( VkDeviceCreateInfo &create_info )
        {
            VkDevice device_handle;

            if( vkCreateDevice( physical_device_handle_, &create_info, nullptr, &device_handle ) != VK_NULL_HANDLE )
                std::cerr << "Failed to create logical device" << std::endl;
            else
                std::cout << "Logical device created succesfully." << std::endl;

            return device_handle;
        }

        bool
        physical_device::is_device_suitable( const graphics::surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept
        {
            find_queue_families( surface, physical_device_handle );

            return queue_family_indices_.is_complete();
        }

        bool
        physical_device::is_device_suitable_for_compute( VkPhysicalDevice &physical_device_handle ) noexcept
        {
            find_compute_queue_family( physical_device_handle );

            return queue_family_indices_.is_compute_complete();
        }

        void
        physical_device::find_compute_queue_family( VkPhysicalDevice &physical_device_handle ) noexcept
        {
            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties( physical_device_handle, &queue_family_count, nullptr );

            std::vector<VkQueueFamilyProperties> queue_family_properties( queue_family_count );
            vkGetPhysicalDeviceQueueFamilyProperties( physical_device_handle, &queue_family_count, queue_family_properties.data() );

            int i = 0;
            for( const auto& queue_family_property : queue_family_properties )
            {
                if( queue_family_property.queueCount > 0 && queue_family_property.queueFlags & VK_QUEUE_COMPUTE_BIT )
                    queue_family_indices_.compute_family = i;

                if( queue_family_indices_.is_compute_complete() )
                    break;

                ++i;
            }
        }

        void
        physical_device::find_queue_families( const graphics::surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept
        {
            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties( physical_device_handle, &queue_family_count, nullptr );

            std::vector<VkQueueFamilyProperties> queue_family_properties( queue_family_count );
            vkGetPhysicalDeviceQueueFamilyProperties( physical_device_handle, &queue_family_count, queue_family_properties.data() );

            int i = 0;
            for( const auto& queue_family_property : queue_family_properties )
            {
                if( queue_family_property.queueCount > 0 && queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    queue_family_indices_.graphics_family = i;

                if( queue_family_property.queueCount > 0 && queue_family_property.queueFlags & VK_QUEUE_COMPUTE_BIT )
                    queue_family_indices_.compute_family = i;

                VkBool32 present_support = surface.get_physical_device_surface_support( physical_device_handle, i );

                if( queue_family_property.queueCount > 0 && present_support )
                    queue_family_indices_.present_family = i;

                if( queue_family_indices_.is_complete() )
                    break;

                ++i;
            }
        }

        helpers::queue_family_indices
        physical_device::get_queue_family_indices( ) const noexcept
        {
            return queue_family_indices_;
        }

        helpers::swapchain_support_details
        physical_device::query_swapchain_support( const graphics::surface& surface ) const noexcept
        {
            helpers::swapchain_support_details swapchain_support_details;

            swapchain_support_details.capabilities = surface.get_capabilities( physical_device_handle_ );
            swapchain_support_details.formats = surface.get_format( physical_device_handle_ );
            swapchain_support_details.present_modes = surface.get_present_mode( physical_device_handle_ );

            return swapchain_support_details;
        }

        VkPhysicalDeviceMemoryProperties physical_device::get_memory_properties( ) const
        {
            VkPhysicalDeviceMemoryProperties mem_properties;

            vkGetPhysicalDeviceMemoryProperties( physical_device_handle_, &mem_properties );

            return mem_properties;
        }
    }
}
