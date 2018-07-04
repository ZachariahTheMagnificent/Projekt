/*!
 *
 */

#ifndef COMPUTE_PHYSICALDEVICE_H
#define COMPUTE_PHYSICALDEVICE_H

#include <set>

#include <vulkan/vulkan.h>

#include "instance.h"
#include "../graphics/surface.h"
#include "../helpers/queue_family_indices.h"
#include "../helpers/swapchain_support_details.h"
#include "../graphics/surface.h"

namespace vk
{
    namespace core
    {
        class physical_device
        {
        public:
            physical_device() = default;
            explicit physical_device( instance& p_instance );
            physical_device( instance& instance, const graphics::surface& surface );
            physical_device( instance& instance, VkPhysicalDeviceFeatures& physical_Device_features );
            physical_device( const physical_device& physical_device ) = delete;
            physical_device( physical_device&& physical_device ) noexcept;
            ~physical_device( ) = default;

            VkPhysicalDevice get() const
            {
                return physical_device_handle_;
            }

            helpers::swapchain_support_details query_swapchain_support( const graphics::surface& surface ) const noexcept;

            void set_device_features( VkPhysicalDeviceFeatures& physical_device_features ) noexcept;

            helpers::queue_family_indices get_queue_family_indices( ) const noexcept;

            int32_t get_queue_family_index( const helpers::queue_family_type& type ) const noexcept;

            std::set<int> unique_queue_families() noexcept;

            const VkPhysicalDeviceFeatures& features() noexcept;

            physical_device& operator=( const physical_device& physical_device ) = delete;
            physical_device& operator=( physical_device&& physical_device ) noexcept;

            VkDevice create_device( VkDeviceCreateInfo& create_info );

            VkPhysicalDeviceMemoryProperties get_memory_properties( ) const;

            void check_surface_present_support( const graphics::surface& surface );

        private:
            bool is_device_suitable( const graphics::surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept;
            bool is_device_suitable_for_compute( VkPhysicalDevice &physical_device_handle ) noexcept;
            void find_queue_families( const graphics::surface& surface, VkPhysicalDevice& physical_device_handle ) noexcept;
            void find_compute_queue_family( VkPhysicalDevice& physical_device_handle ) noexcept;


        private:
            VkPhysicalDevice physical_device_handle_ = VK_NULL_HANDLE;
            VkPhysicalDeviceFeatures physical_device_features_ = {};
            VkPhysicalDeviceProperties physical_device_properties_ = {};

            helpers::queue_family_indices queue_family_indices_;
        };
    }
}

#endif //COMPUTE_PHYSICALDEVICE_H
