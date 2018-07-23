/*!
 *
 */

#ifndef COMPUTE_QUEUE_H
#define COMPUTE_QUEUE_H

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        class queue
        {
        public:
            queue( ) = default;
            queue( const logical_device& logical_device,
                   const physical_device& physical_device,
                   const helpers::queue_family_type& type,
                   uint32_t queue_index );
            queue( const queue& queue ) = delete;
            queue( queue&& queue ) noexcept;
            ~queue( ) = default;

            void wait_idle();
            void submit( VkSubmitInfo& submit_info, VkFence fence_handle );
            VkResult present( VkPresentInfoKHR& present_info );

            queue& operator=( const queue& queue ) = delete;
            queue& operator=( queue&& queue ) noexcept;

        private:
            VkQueue queue_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_QUEUE_H
