/*!
 *
 */

#ifndef COMPUTE_DEBUGREPORT_H
#define COMPUTE_DEBUGREPORT_H

#include "Instance.h"

namespace Vk
{
    namespace Core
    {
        class DebugReport
        {
        public:
            DebugReport( ) = default;
            explicit DebugReport( Instance* p_instance );
            DebugReport( const DebugReport& debug_report ) = delete;
            DebugReport( DebugReport&& debug_report ) noexcept;
            ~DebugReport( );

            DebugReport& operator=( const DebugReport& debug_report ) = delete;
            DebugReport& operator=( DebugReport&& debug_report ) noexcept;

        private:
            Instance* p_instance_;

            VkDebugReportCallbackEXT debug_report_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_DEBUGREPORT_H
