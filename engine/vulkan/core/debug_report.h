/*!
 *
 */

#ifndef COMPUTE_DEBUGREPORT_H
#define COMPUTE_DEBUGREPORT_H

#include "instance.h"

namespace vk
{
    namespace core
    {
        class debug_report
        {
        public:
            debug_report( ) = default;
            explicit debug_report( instance* p_instance );
            debug_report( const debug_report& debug_report ) = delete;
            debug_report( debug_report&& debug_report ) noexcept;
            ~debug_report( );

            debug_report& operator=( const debug_report& debug_report ) = delete;
            debug_report& operator=( debug_report&& debug_report ) noexcept;

        private:
            instance* p_instance_;

            VkDebugReportCallbackEXT debug_report_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_DEBUGREPORT_H
