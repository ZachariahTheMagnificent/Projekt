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
        class semaphores
        {
        public:
            semaphores( ) = default;
            semaphores( const logical_device* p_logical_device, uint32_t count );
            semaphores( const semaphores& semaphores ) = delete;
            semaphores( semaphores&& semaphores ) noexcept;
            ~semaphores( );

            semaphores& operator=( const semaphores& semaphores ) = delete;
            semaphores& operator=( semaphores&& semaphores ) noexcept;

            const VkSemaphore& operator[]( size_t i );

        private:
            const logical_device* p_logical_device_ = nullptr;

            VkSemaphore* semaphore_handles_;
            uint32_t count_;
        };
    }
}

#endif //COMPUTE_SEMAPHORE_H
