/*!
 *
 */

#ifndef COMPUTE_FENCES_H
#define COMPUTE_FENCES_H

#include <array>

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"

namespace Vk
{
    namespace Core
    {
        template<size_t size>
        class Fences
        {
        public:
            Fences( ) = default;
            explicit Fences( const LogicalDevice* p_logical_device )
                    :
                    p_logical_device_( p_logical_device )
            {
                VkFenceCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                for( auto& fence : fence_handles_ )
                {
                    fence = p_logical_device_->create_fence( create_info );
                }
            }
            Fences( const Fences& fences ) = delete;
            Fences( Fences&& fences ) noexcept
            {
                *this = std::move( fences );
            }
            ~Fences( )
            {
                for( auto& fence : fence_handles_ )
                {
                    fence = p_logical_device_->destroy_fence( fence );
                }
            }

            void wait_for_fence( size_t fence_index, VkBool32 wait_all, uint64_t timeout )
            {
                p_logical_device_->wait_for_fences( &fence_handles_[fence_index], 1, wait_all, timeout );
            }
            void wait_for_fences( VkBool32 wait_all, uint64_t timeout )
            {
                p_logical_device_->wait_for_fences( fence_handles_, static_cast<uint32_t>( num_elems_ ), wait_all, timeout );
            }

            void reset_fence( size_t fence_index )
            {
                p_logical_device_->reset_fences( &fence_handles_[fence_index], 1 );
            }
            void reset_fences()
            {
                p_logical_device_->reset_fences( fence_handles_, static_cast<uint32_t>( num_elems_ ) );
            }

            Fences& operator=( const Fences& fences ) = delete;
            Fences& operator=( Fences&& fences ) noexcept
            {
                if( this != &fences )
                {
                    for( auto i = 0; i < num_elems_; ++i )
                    {
                        if( fence_handles_[i] != VK_NULL_HANDLE )
                            fence_handles_[i] = p_logical_device_->destroy_fence( fence_handles_[i] );

                        fence_handles_[i] = fences.fence_handles_[i];
                        fences.fence_handles_[i] = VK_NULL_HANDLE;

                        p_logical_device_ = fences.p_logical_device_;
                    }
                }

                return *this;
            }

        private:
            const LogicalDevice* p_logical_device_ = nullptr;

            VkFence fence_handles_[size] = {};
            size_t num_elems_ = size;
        };
    }
}

#endif //COMPUTE_FENCES_H
