/*!
 *
 */

#ifndef PROJEKT_DEBUGREPORT_H
#define PROJEKT_DEBUGREPORT_H

#include <iostream>

#include <vulkan/vulkan.h>

#include "Instance.h"

namespace Vk
{
    class DebugReport
    {
    public:
        DebugReport() = default;
        explicit DebugReport( Instance *p_instance );
        DebugReport( const DebugReport& debug_report ) = delete;
        DebugReport( DebugReport&& debug_report ) noexcept ;
        ~DebugReport( );

        DebugReport& operator=( const DebugReport& debug_report ) = delete;
        DebugReport& operator=( DebugReport&& debug_report ) noexcept;

    private:
    private:
        Instance* p_instance_;

        VkDebugReportCallbackEXT debug_report_callback_handle_ = VK_NULL_HANDLE;
    };
}

#endif //PROJEKT_DEBUGREPORT_H
