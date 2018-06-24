/*!
 *
 */

#include <vector>
#include <set>
#include <iostream>

#include "PhysicalDevice.h"
#include "../Graphics/Surface.h"

namespace Vk
{
    namespace Core
    {
        PhysicalDevice::PhysicalDevice( Instance &instance )
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
        PhysicalDevice::PhysicalDevice( Vk::Core::Instance& instance, const Vk::Graphics::Surface& surface )
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
        PhysicalDevice::PhysicalDevice( Instance& instance, VkPhysicalDeviceFeatures& physical_Device_features )
                :
                PhysicalDevice( instance )
        {
            set_device_features( physical_Device_features );
        }
        PhysicalDevice::PhysicalDevice( PhysicalDevice &&physical_device ) noexcept
        {
            *this = std::move( physical_device );
        }

        void
        PhysicalDevice::set_device_features( VkPhysicalDeviceFeatures& physical_device_features ) noexcept
        {
            physical_device_features_ = physical_device_features;
        }

        int32_t PhysicalDevice::get_queue_family_index( const Helpers::QueueFamilyType& type ) const noexcept
        {
            if( type == Helpers::QueueFamilyType::eGRAPHICS )
                return queue_family_indices_.graphics_family;
            else if( type == Helpers::QueueFamilyType::ePRESENT )
                return queue_family_indices_.present_family;
            else if( type == Helpers::QueueFamilyType::eCOMPUTE )
                return queue_family_indices_.compute_family;

            return -1;
        }
        std::set<int>
        PhysicalDevice::unique_queue_families( ) noexcept
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
        PhysicalDevice::features( ) noexcept
        {
            return physical_device_features_;
        }

        PhysicalDevice&
        PhysicalDevice::operator=( PhysicalDevice &&physical_device ) noexcept
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
        PhysicalDevice::create_device( VkDeviceCreateInfo &create_info )
        {
            VkDevice device_handle;

            if( vkCreateDevice( physical_device_handle_, &create_info, nullptr, &device_handle ) != VK_NULL_HANDLE )
                std::cerr << "Failed to create logical device" << std::endl;
            else
                std::cout << "Logical device created succesfully." << std::endl;

            return device_handle;
        }

        bool PhysicalDevice::is_device_suitable( const Graphics::Surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept
        {
            find_queue_families( surface, physical_device_handle );

            return queue_family_indices_.is_complete();
        }

        bool
        PhysicalDevice::is_device_suitable_for_compute( VkPhysicalDevice &physical_device_handle ) noexcept
        {
            find_compute_queue_family( physical_device_handle );

            return queue_family_indices_.is_compute_complete();
        }

        void
        PhysicalDevice::find_compute_queue_family( VkPhysicalDevice &physical_device_handle ) noexcept
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

        void PhysicalDevice::find_queue_families( const Graphics::Surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept
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

        Helpers::QueueFamilyIndices& PhysicalDevice::get_queue_family_indices( ) noexcept
        {
            return queue_family_indices_;
        }
    }
}
