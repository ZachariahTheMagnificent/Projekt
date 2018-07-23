/*!
 *
 */

#ifndef PROJEKT_INSTANCE_H
#define PROJEKT_INSTANCE_H

#include <vulkan/vulkan.h>

#include "../Window/Window.h"

namespace Vk
{
    class Instance
    {
    public:
        Instance() = default;
        explicit Instance( Window& window );
        Instance( const Instance& instance ) = delete;
        Instance( Instance&& instance ) noexcept;
        ~Instance();

        Instance& operator=( const Instance& instance ) = delete;
        Instance& operator=( Instance&& instance ) noexcept;

        VkDebugReportCallbackEXT create_debug_report_callback( const VkDebugReportCallbackCreateInfoEXT& create_info );
        VkDebugReportCallbackEXT destroy_debug_report_callback( VkDebugReportCallbackEXT& debug_report_callback );

        VkSurfaceKHR create_surface( Window& window ) const;
        VkSurfaceKHR destroy_surface( VkSurfaceKHR& surface ) const;

        std::vector<VkPhysicalDevice> enumerate_physical_devices() const;

    private:
        bool check_validation_layer_support();

        VKAPI_ATTR VkResult VKAPI_CALL vk_create_debug_report_callback_EXT( VkInstance instance,
                                                                            const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                                                            const VkAllocationCallbacks* pAllocator,
                                                                            VkDebugReportCallbackEXT* pCallback );
        VKAPI_ATTR void VKAPI_CALL vk_destroy_debug_report_callback_EXT( VkInstance instance,
                                                                         VkDebugReportCallbackEXT callback,
                                                                         const VkAllocationCallbacks* pAllocator );

    private:
        VkInstance instance_handle_ = VK_NULL_HANDLE;

        const std::vector<const char*> validation_layers = {
                "VK_LAYER_LUNARG_standard_validation"
        };
    };
}

#endif //PROJEKT_INSTANCE_H
