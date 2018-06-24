/*!
 *
 */

#ifndef COMPUTE_SEMAPHORE_H
#define COMPUTE_SEMAPHORE_H

#include <array>

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"

namespace Vk
{
    namespace Core
    {
        template<size_t size>
        class Semaphores
        {
        public:
            Semaphores( ) = default;
            explicit Semaphores( const LogicalDevice* p_logical_device )
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
            Semaphores( const Semaphores& semaphores ) = delete;
            Semaphores( Semaphores&& semaphores ) noexcept
            {
                *this = std::move( semaphores );
            }
            ~Semaphores( )
            {
                for( auto& semaphore_handle : semaphore_handles_ )
                    semaphore_handle = p_logical_device_->destroy_semaphore( semaphore_handle );
            }

            Semaphores& operator=( const Semaphores& semaphores ) = delete;
            Semaphores& operator=( Semaphores&& semaphores ) noexcept
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
            const LogicalDevice* p_logical_device_ = nullptr;

            VkSemaphore semaphore_handles_[size] = {};
            size_t num_elems_ = size;
        };
    }
}

#endif //COMPUTE_SEMAPHORE_H
