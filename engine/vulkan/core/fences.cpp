/*!
 *
 */

#include "fences.h"

namespace vk
{
    namespace core
    {
        fences::fences( const logical_device* p_logical_device, uint32_t count )
                : p_logical_device_( p_logical_device ), count_( count )
        {
            VkFenceCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            fence_handles_ = p_logical_device_->create_fences( create_info, count_ );
        }

        fences::fences( fences&& fences ) noexcept
        {
            *this = std::move( fences );
        }

        fences::~fences( )
        {
            if( fence_handles_ != VK_NULL_HANDLE )
                fence_handles_ = p_logical_device_->destroy_fences( fence_handles_, count_ );
        }

        void
        fences::wait_for_fence( size_t fence_index, VkBool32 wait_all, uint64_t timeout )
        {
            p_logical_device_->wait_for_fences( &fence_handles_[fence_index], 1, wait_all, timeout );
        }

        void
        fences::reset_fence( size_t fence_index )
        {
            p_logical_device_->reset_fences( &fence_handles_[fence_index], 1 );
        }

        fences&
        fences::operator=( fences&& fences ) noexcept
        {
            if ( this != &fences )
            {
                count_ = fences.count_;
                fences.count_ = 0;

                fence_handles_ = fences.fence_handles_;
                fences.fence_handles_ = nullptr;

                p_logical_device_ = fences.p_logical_device_;
            }

            return *this;
        }

        VkFence
        const& fences::operator[]( size_t i )
        {
            return fence_handles_[i];
        }
    }
}