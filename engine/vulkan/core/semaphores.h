/*!
 *
 */

#ifndef COMPUTE_SEMAPHORE_H
#define COMPUTE_SEMAPHORE_H

#include <array>

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        template<size_t size>
        class semaphores
        {
        public:
            semaphores( ) = default;
            explicit semaphores( const logical_device* p_logical_device )
                    :
                    p_logical_device_( p_logical_device )
            {
                VkSemaphoreCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                for( auto& semaphore_handle : semaphore_handles_ )
                {
                    semaphore_handle = p_logical_device_->create_semaphore( create_info );
                }
            }
            semaphores( const semaphores& semaphores ) = delete;
            semaphores( semaphores&& semaphores ) noexcept
            {
                *this = std::move( semaphores );
            }
            ~semaphores( )
            {
                for( auto& semaphore_handle : semaphore_handles_ )
                    semaphore_handle = p_logical_device_->destroy_semaphore( semaphore_handle );
            }

            semaphores& operator=( const semaphores& semaphores ) = delete;
            semaphores& operator=( semaphores&& semaphores ) noexcept
            {
                if( this != &semaphores )
                {
                    for( auto i = 0; i < num_elems_; ++i )
                    {
                        if( semaphore_handles_[i] != VK_NULL_HANDLE )
                            semaphore_handles_[i] = p_logical_device_->destroy_semaphore( semaphore_handles_[i] );

                        semaphore_handles_[i] = semaphores.semaphore_handles_[i];
                        semaphores.semaphore_handles_[i] = VK_NULL_HANDLE;

                        p_logical_device_ = semaphores.p_logical_device_;
                    }
                }

                return *this;
            }

        private:
            const logical_device* p_logical_device_ = nullptr;

            VkSemaphore semaphore_handles_[size] = {};
            size_t num_elems_ = size;
        };
    }
}

#endif //COMPUTE_SEMAPHORE_H
