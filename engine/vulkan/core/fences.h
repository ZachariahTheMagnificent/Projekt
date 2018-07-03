/*!
 *
 */

#ifndef COMPUTE_FENCES_H
#define COMPUTE_FENCES_H

#include <array>

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        class fences
        {
        public:
            fences( ) = default;
            fences( const logical_device* p_logical_device, uint32_t count );
            fences( const fences& fences ) = delete;
            fences( fences&& fences ) noexcept;
            ~fences( );

            void wait_for_fence( size_t fence_index, VkBool32 wait_all, uint64_t timeout );
            void reset_fence( size_t fence_index );

            fences& operator=( const fences& fences ) = delete;
            fences& operator=( fences&& fences ) noexcept;

            const VkFence& operator[]( size_t i );

        private:
            const logical_device* p_logical_device_ = nullptr;

            VkFence* fence_handles_;
            uint32_t count_;
        };
    }
}

#endif //COMPUTE_FENCES_H
