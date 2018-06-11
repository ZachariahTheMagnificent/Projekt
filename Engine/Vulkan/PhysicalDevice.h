/*!
 *
 */

#ifndef PROJEKT_PHYSICALDEVICE_H
#define PROJEKT_PHYSICALDEVICE_H

#include "Instance.h"
#include "Surface.h"

namespace Vk
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice() = default;
        PhysicalDevice( const Instance* p_instance, Surface* p_surface );
        PhysicalDevice( const PhysicalDevice& physical_device ) = delete;
        PhysicalDevice( PhysicalDevice&& physical_device ) noexcept;

        PhysicalDevice& operator=( const PhysicalDevice& physical_device ) = delete;
        PhysicalDevice& operator=( PhysicalDevice&& physical_device ) noexcept;

        VkDevice create_device( const VkDeviceCreateInfo& create_info) const;

        VkPhysicalDeviceMemoryProperties get_physical_device_memory_properties() const;

        Helpers::SwapchainSupportDetails query_swapchain_support( );
        Helpers::QueueFamilyIndices find_queue_family_indices( );

    private:
        bool is_device_suitable( VkPhysicalDevice& physical_device_handle );
        bool check_device_extension_support( VkPhysicalDevice& physical_device_handle );

        Helpers::QueueFamilyIndices find_queue_family_indices( VkPhysicalDevice& physical_device_handle );
        Helpers::SwapchainSupportDetails query_swapchain_support( VkPhysicalDevice& physical_device_handle );

    private:
        Surface* p_surface_;

        VkPhysicalDevice physical_device_handle_ = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties physical_device_properties;

        const std::vector<const char*> device_extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };
}

#endif //PROJEKT_PHYSICALDEVICE_H
