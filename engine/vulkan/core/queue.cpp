/*!
 *
 */

#include "queue.h"

#include "../../utils/exception/vulkan_exception.h"

namespace vk
{
    namespace core
    {
        queue::queue( const logical_device& logical_device,
                      const physical_device& physical_device,
                      const helpers::queue_family_type& type,
                      uint32_t queue_index)
        {
            queue_handle_ = logical_device.get_queue( physical_device.get_queue_family_index( type ), queue_index );
        }
        queue::queue( queue&& queue ) noexcept
        {
            *this = std::move( queue );
        }

        void
        queue::wait_idle( )
        {
            vkQueueWaitIdle( queue_handle_ );
        }
        void
        queue::submit( VkSubmitInfo& submit_info, VkFence fence_handle )
        {
            if( vkQueueSubmit( queue_handle_, 1, &submit_info, fence_handle ) != VK_SUCCESS )
                throw vulkan_exception{ "Failed to submit queue", __FILE__, __LINE__ };
        }
        VkResult
        queue::present( VkPresentInfoKHR& present_info )
        {
            return vkQueuePresentKHR( queue_handle_, &present_info );
        }

        queue&
        queue::operator=( queue&& queue ) noexcept
        {
            if( this != &queue )
            {
                queue_handle_ = queue.queue_handle_;
                queue.queue_handle_ = VK_NULL_HANDLE;
            }

            return *this;
        }
    }
}