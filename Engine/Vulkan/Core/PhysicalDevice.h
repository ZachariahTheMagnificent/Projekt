/*!
 *
 */

#ifndef COMPUTE_PHYSICALDEVICE_H
#define COMPUTE_PHYSICALDEVICE_H

#include <set>

#include <vulkan/vulkan.h>

#include "Instance.h"
#include "../Helpers/QueueFamilyIndices.h"
#include "../Graphics/Surface.h"

namespace Vk
{
    namespace Core
    {
        class PhysicalDevice
        {
        public:
            PhysicalDevice() = default;
            explicit PhysicalDevice( Instance& p_instance );
            PhysicalDevice( Instance& instance, const Graphics::Surface& surface );
            PhysicalDevice( Instance& instance, VkPhysicalDeviceFeatures& physical_Device_features );
            PhysicalDevice( const PhysicalDevice& physical_device ) = delete;
            PhysicalDevice( PhysicalDevice&& physical_device ) noexcept;
            ~PhysicalDevice( ) = default;

            VkPhysicalDevice get() const
            {
                return physical_device_handle_;
            }

            void set_device_features( VkPhysicalDeviceFeatures& physical_device_features ) noexcept;

            Helpers::QueueFamilyIndices& get_queue_family_indices( ) noexcept;
            int32_t get_queue_family_index( const Helpers::QueueFamilyType& type ) const noexcept;
            std::set<int> unique_queue_families() noexcept;
            const VkPhysicalDeviceFeatures& features() noexcept;

            PhysicalDevice& operator=( const PhysicalDevice& physical_device ) = delete;
            PhysicalDevice& operator=( PhysicalDevice&& physical_device ) noexcept;

            VkDevice create_device( VkDeviceCreateInfo& create_info );

        private:
            bool is_device_suitable( const Graphics::Surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept;
            bool is_device_suitable_for_compute( VkPhysicalDevice &physical_device_handle ) noexcept;
            void find_queue_families( const Graphics::Surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept;
            void find_compute_queue_family( VkPhysicalDevice& physical_device_handle ) noexcept;


        private:
            VkPhysicalDevice physical_device_handle_ = VK_NULL_HANDLE;
            VkPhysicalDeviceFeatures physical_device_features_ = {};
            VkPhysicalDeviceProperties physical_device_properties_ = {};

            Helpers::QueueFamilyIndices queue_family_indices_;
        };
    }
}

#endif //COMPUTE_PHYSICALDEVICE_H
