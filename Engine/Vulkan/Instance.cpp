/*!
 *
 */

#include <cstring>

#include "Instance.h"

#include "../Utils/Exception/VulkanException.h"

namespace Vk
{
    Instance::Instance( Window &window )
    {
        auto extensions = window.get_required_extensions();

        if( enable_validation_layers && !check_validation_layer_support() )
        {
            throw VulkanException{ "Validation layers requested, but not available", __FILE__, __LINE__ };
        }

        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = window.get_title().c_str();
        app_info.applicationVersion = VK_MAKE_VERSION( 0, 0, 1 );
        app_info.pEngineName = "No Engine";
        app_info.engineVersion = VK_MAKE_VERSION( 0, 0, 1 );
        app_info.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
        create_info.ppEnabledExtensionNames = extensions.data();

        if( enable_validation_layers )
        {
            create_info.enabledLayerCount = static_cast<uint32_t>( validation_layers.size() );
            create_info.ppEnabledLayerNames = validation_layers.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        if( vkCreateInstance( &create_info, nullptr, &instance_handle_ ) != VK_SUCCESS )
            throw VulkanException{ "Failed to create instance", __FILE__, __LINE__ };
    }
    Instance::Instance( Instance &&instance ) noexcept
    {
        *this = std::move( instance );
    }
    Instance::~Instance( )
    {
        vkDestroyInstance( instance_handle_, nullptr );
    }
    Instance &Instance::operator=( Instance &&instance ) noexcept
    {
        if( this != &instance )
        {
            if( instance_handle_ != VK_NULL_HANDLE )
                vkDestroyInstance( instance_handle_, nullptr );

            instance_handle_ = instance.instance_handle_;
            instance.instance_handle_ = VK_NULL_HANDLE;
        }

        return *this;
    }


    VkDebugReportCallbackEXT Instance::create_debug_report_callback( const VkDebugReportCallbackCreateInfoEXT& create_info )
    {
        VkDebugReportCallbackEXT debug_report_callback_handle;

        if( vk_create_debug_report_callback_EXT( instance_handle_, &create_info, nullptr, &debug_report_callback_handle ) != VK_SUCCESS )
            throw VulkanException{ "Failed to create debug report callback", __FILE__, __LINE__ };

        return debug_report_callback_handle;
    }
    VkDebugReportCallbackEXT Instance::destroy_debug_report_callback( VkDebugReportCallbackEXT &debug_report_callback )
    {
        vk_destroy_debug_report_callback_EXT( instance_handle_, debug_report_callback, nullptr );

        return VK_NULL_HANDLE;
    }

    VkSurfaceKHR Instance::create_surface( Window& window ) const
    {
        return window.create_surface( instance_handle_ );
    }
    VkSurfaceKHR Instance::destroy_surface( VkSurfaceKHR& surface ) const
    {
        vkDestroySurfaceKHR( instance_handle_, surface, nullptr );

        return VK_NULL_HANDLE;
    }

    std::vector<VkPhysicalDevice>
    Instance::enumerate_physical_devices( ) const
    {
        uint32_t device_count;
        vkEnumeratePhysicalDevices( instance_handle_, &device_count, nullptr );

        if( device_count == 0 )
            throw VulkanException{ "Failed to find any GPUs with Vulkan support", __FILE__, __LINE__ };

        std::vector<VkPhysicalDevice> available_devices( device_count );
        vkEnumeratePhysicalDevices( instance_handle_, &device_count, available_devices.data() );

        return available_devices;
    }

    bool
    Instance::check_validation_layer_support( )
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties( &layer_count, nullptr );

        std::vector<VkLayerProperties> available_layers( layer_count );
        vkEnumerateInstanceLayerProperties( &layer_count, available_layers.data( ));

        for ( const char *layer_name : validation_layers )
        {
            bool layer_found = false;

            for ( const auto &layer_property : available_layers )
            {
                if ( strcmp( layer_name, layer_property.layerName ) == 0 )
                {
                    layer_found = true;
                    break;
                }
            }

            if ( !layer_found )
                return false;
        }

        return true;
    }

    VkResult Instance::vk_create_debug_report_callback_EXT( VkInstance instance,
                                                            const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                                            const VkAllocationCallbacks *pAllocator,
                                                            VkDebugReportCallbackEXT *pCallback )
    {

        static auto func = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr( instance,
                                                                                         "vkCreateDebugReportCallbackEXT" );

        if ( func != nullptr )
        {
            return func( instance, pCreateInfo, pAllocator, pCallback );
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    void Instance::vk_destroy_debug_report_callback_EXT( VkInstance instance, VkDebugReportCallbackEXT callback,
                                                         const VkAllocationCallbacks *pAllocator )
    {
        static auto func = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr ( instance, "vkDestroyDebugReportCallbackEXT" );

        if ( func != nullptr )
        {
            func ( instance, callback, pAllocator );
        }
    }
}