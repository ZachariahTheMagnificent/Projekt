/*!
 *
 */

#include "semaphores.h"

namespace vk
{
    namespace core
    {
        semaphores::semaphores( const logical_device* p_logical_device, uint32_t count )
                :
                p_logical_device_( p_logical_device ),
                count_( count )
        {
            VkSemaphoreCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            semaphore_handles_ = p_logical_device_->create_semaphores( create_info, count_ );
        }

        semaphores::semaphores( semaphores&& semaphores ) noexcept
        {
            *this = std::move( semaphores );
        }

        semaphores::~semaphores( )
        {
            if( semaphore_handles_ != VK_NULL_HANDLE )
                semaphore_handles_ = p_logical_device_->destroy_semaphores( semaphore_handles_, count_ );
        }

        semaphores&
        semaphores::operator=( semaphores&& semaphores ) noexcept
        {
            if( this != &semaphores )
            {
                if( semaphore_handles_ != VK_NULL_HANDLE )
                    semaphore_handles_ = p_logical_device_->destroy_semaphores( semaphore_handles_, count_ );

                count_ = semaphores.count_;
                semaphores.count_;

                semaphore_handles_ = semaphores.semaphore_handles_;
                semaphores.semaphore_handles_ = VK_NULL_HANDLE;

                p_logical_device_ = semaphores.p_logical_device_;
            }

            return *this;
        }

        VkSemaphore&
        semaphores::operator[]( size_t i )
        {
            return semaphore_handles_[i];
        }
    }
}