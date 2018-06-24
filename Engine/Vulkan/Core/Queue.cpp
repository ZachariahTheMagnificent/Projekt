/*!
 *
 */

#include "Queue.h"
#include "../../Utils/Exception/VulkanException.h"

namespace Vk
{
    namespace Core
    {
        Queue::Queue( const LogicalDevice& logical_device,
                      const PhysicalDevice& physical_device,
                      const Helpers::QueueFamilyType& type,
                      uint32_t queue_index)
        {
            queue_handle_ = logical_device.get_queue( physical_device.get_queue_family_index( type ), queue_index );
        }
        Queue::Queue( Queue&& queue ) noexcept
        {
            *this = std::move( queue );
        }

        void Queue::wait_idle( )
        {
            vkQueueWaitIdle( queue_handle_ );
        }
        void Queue::submit( VkSubmitInfo& submit_info, VkFence fence_handle )
        {
            if( vkQueueSubmit( queue_handle_, 1, &submit_info, fence_handle ) != VK_SUCCESS )
                throw VulkanException{ "Failed to submit queue", __FILE__, __LINE__ };
        }
        VkResult Queue::present( VkPresentInfoKHR& present_info )
        {
            return vkQueuePresentKHR( queue_handle_, &present_info );
        }

        Queue& Queue::operator=( Queue&& queue ) noexcept
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