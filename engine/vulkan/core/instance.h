/*!
 *
 */

#ifndef COMPUTE_INSTANCE_H
#define COMPUTE_INSTANCE_H

#include <string>
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

#include "../../window/window.h"

namespace vk
{
    namespace core
    {
        class instance
        {
        public:
            instance() = default;
            instance( const std::string& name, const std::vector<const char*>& validation_layers, std::vector<const char*>& extensions );
            instance( const instance& instance ) = delete;
            instance( instance&& instance ) noexcept;
            ~instance( );

            instance& operator=( const instance& instance ) = delete;
            instance& operator=( instance&& instance ) noexcept;

            VkDebugReportCallbackEXT create_debug_report( const VkDebugReportCallbackCreateInfoEXT& create_info );
            VkDebugReportCallbackEXT destroy_debug_report( VkDebugReportCallbackEXT& debug_report_handle );

            VkSurfaceKHR create_surface( const window& window ) const;
            VkSurfaceKHR destroy_surface( VkSurfaceKHR& surface_handle ) const;

            std::vector<VkPhysicalDevice> enumerate_physical_devices( ) noexcept;

        private:
            bool check_validation_layer_support( const std::vector<const char*>& validation_layers ) noexcept;

            VKAPI_ATTR VkResult VKAPI_CALL vk_create_debug_report_callback_EXT( VkInstance instance,
                                                                                const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                                                                const VkAllocationCallbacks* pAllocator,
                                                                                VkDebugReportCallbackEXT* pCallback );
            VKAPI_ATTR void VKAPI_CALL vk_destroy_debug_report_callback_EXT( VkInstance instance,
                                                                             VkDebugReportCallbackEXT callback,
                                                                             const VkAllocationCallbacks* pAllocator );

        private:
            VkInstance instance_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_INSTANCE_H
