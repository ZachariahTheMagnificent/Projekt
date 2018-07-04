/*!
 *
 */

#include <cstring>
#include <iostream>

#include "instance.h"

namespace vk
{
    namespace core
    {
        instance::instance( const std::string &name, const std::vector<const char*>& validation_layers, std::vector<const char*> extensions )
        {
            if( enable_validation_layers && !check_validation_layer_support( validation_layers ) )
                std::cerr << "Validation layers requested, but not available" << std::endl;

            if( enable_validation_layers )
                extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

            VkApplicationInfo app_info = {};
            app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            app_info.pApplicationName = name.c_str();
            app_info.applicationVersion = VK_MAKE_VERSION( 0, 0, 1 );
            app_info.pEngineName = "No engine";
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
                std::cerr << "Failed to create instance." << std::endl;
            else
                std::cout << "instance created successfully." << std::endl;
        }

        instance::instance( instance &&instance ) noexcept
        {
            *this = std::move( instance );
        }

        instance::~instance( )
        {
            if( instance_handle_ != VK_NULL_HANDLE )
            {
                vkDestroyInstance( instance_handle_, nullptr );

                std::cout << "instance destroyed." << std::endl;
            }
        }

        instance&
        instance::operator=( instance &&instance ) noexcept
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

        VkDebugReportCallbackEXT
        instance::create_debug_report( const VkDebugReportCallbackCreateInfoEXT &create_info )
        {
            VkDebugReportCallbackEXT debug_report;

            if( vk_create_debug_report_callback_EXT( instance_handle_, &create_info, nullptr, &debug_report ) != VK_SUCCESS )
                std::cerr << "Failed to create debug report callback." << std::endl;
            else
                std::cout << "Debug report callback created successfully." << std::endl;

            return debug_report;
        }

        VkDebugReportCallbackEXT
        instance::destroy_debug_report( VkDebugReportCallbackEXT& debug_report_handle )
        {
            vk_destroy_debug_report_callback_EXT( instance_handle_, debug_report_handle, nullptr );

            std::cout << "Debug report callback destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkSurfaceKHR
        instance::create_surface( const window& window ) const
        {
            return window.create_surface( instance_handle_ );
        }

        VkSurfaceKHR
        instance::destroy_surface( VkSurfaceKHR& surface_handle ) const
        {
            vkDestroySurfaceKHR( instance_handle_, surface_handle, nullptr );

            std::cout << "surface destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        std::vector<VkPhysicalDevice>
        instance::enumerate_physical_devices( ) noexcept
        {
            uint32_t device_count;
            vkEnumeratePhysicalDevices( instance_handle_, &device_count, nullptr );

            if( device_count == 0 )
                std::cerr << "Failed to find any physical devices with vulkan support" << std::endl;

            std::vector<VkPhysicalDevice> available_devices( device_count );
            vkEnumeratePhysicalDevices( instance_handle_, &device_count, available_devices.data() );

            return available_devices;
        }

        bool
        instance::check_validation_layer_support( const std::vector<const char*>& validation_layers ) noexcept
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

        VkResult
        instance::vk_create_debug_report_callback_EXT( VkInstance instance,
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
        void
        instance::vk_destroy_debug_report_callback_EXT( VkInstance instance, VkDebugReportCallbackEXT callback,
                                                        const VkAllocationCallbacks *pAllocator )
        {
            static auto func = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr ( instance, "vkDestroyDebugReportCallbackEXT" );

            if ( func != nullptr )
            {
                func ( instance, callback, pAllocator );
            }
        }

    }
}