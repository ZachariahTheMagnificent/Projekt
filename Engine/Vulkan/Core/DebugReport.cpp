/*!
 *
 */

#include <iostream>

#include "DebugReport.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64_t obj, size_t location,
                                                        int32_t code, const char* layerPrefix,
                                                        const char* msg, void* userData )
{
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

namespace Vk
{
    namespace Core
    {
        DebugReport::DebugReport( Instance *p_instance )
                :
                p_instance_( p_instance )
        {
            VkDebugReportCallbackCreateInfoEXT create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            create_info.pfnCallback = debug_callback_function;

            debug_report_handle_ = p_instance_->create_debug_report( create_info );
        }
        DebugReport::DebugReport( DebugReport &&debug_report ) noexcept
        {
            *this = std::move( debug_report );
        }
        DebugReport::~DebugReport( )
        {
            if( debug_report_handle_ != VK_NULL_HANDLE )
                debug_report_handle_ = p_instance_->destroy_debug_report( debug_report_handle_ );
        }

        DebugReport&
        DebugReport::operator=( DebugReport &&debug_report ) noexcept
        {
            if( this != &debug_report )
            {
                if( debug_report_handle_ != VK_NULL_HANDLE )
                    debug_report_handle_ = p_instance_->destroy_debug_report( debug_report_handle_ );

                p_instance_ = debug_report.p_instance_;

                debug_report_handle_ = debug_report.debug_report_handle_;
                debug_report.debug_report_handle_ = VK_NULL_HANDLE;
            }

            return *this;
        }
    }
}