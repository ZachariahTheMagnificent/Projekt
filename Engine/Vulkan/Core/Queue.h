/*!
 *
 */

#ifndef COMPUTE_QUEUE_H
#define COMPUTE_QUEUE_H

#include "LogicalDevice.h"

namespace Vk
{
    namespace Core
    {
        class Queue
        {
        public:
            Queue( ) = default;
            Queue( const LogicalDevice& logical_device,
                   const PhysicalDevice& physical_device,
                   const Helpers::QueueFamilyType& type,
                   uint32_t queue_index );
            Queue( const Queue& queue ) = delete;
            Queue( Queue&& queue ) noexcept;
            ~Queue( ) = default;

            void wait_idle();
            void submit( VkSubmitInfo& submit_info, VkFence fence_handle );
            VkResult present( VkPresentInfoKHR& present_info );

            Queue& operator=( const Queue& queue ) = delete;
            Queue& operator=( Queue&& queue ) noexcept;

        private:
            VkQueue queue_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_QUEUE_H
