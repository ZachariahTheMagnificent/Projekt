/*!
 *
 */

#ifndef PROJEKT_FRAME_BUFFERS_H
#define PROJEKT_FRAME_BUFFERS_H

#include <cstddef>

#include "swapchain.h"
#include "../core/logical_device.h"
#include "../core/render_pass.h"

namespace vk
{
    namespace graphics
    {
        class frame_buffers
        {
        public:
            frame_buffers( ) = default;
            frame_buffers( const core::logical_device* p_logical_device, const core::render_pass& render_pass,
                           const swapchain& swapchain, uint32_t count );
            frame_buffers( const frame_buffers& frame_buffers ) = delete;
            frame_buffers( frame_buffers&& frame_buffers ) noexcept;
            ~frame_buffers( );

            const VkFramebuffer& operator[]( size_t i )
            {
                return frame_buffer_handles_[i];
            }
            const size_t get_count()
            {
                return count_;
            }

            frame_buffers& operator=( const frame_buffers& frame_buffers ) = delete;
            frame_buffers& operator=( frame_buffers&& frame_buffers ) noexcept;

        private:
            const core::logical_device* p_logical_device_;

            VkFramebuffer* frame_buffer_handles_ = VK_NULL_HANDLE;
            size_t count_;
        };
    }
}

#endif //PROJEKT_FRAME_BUFFERS_H
