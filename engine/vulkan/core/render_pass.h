/*!
 *
 */

#ifndef PROJEKT_RENDER_PASS_H
#define PROJEKT_RENDER_PASS_H

#include <vulkan/vulkan.h>

#include "logical_device.h"

#include "../graphics/swapchain.h"

namespace vk
{
    namespace core
    {
        class render_pass
        {
        public:
            render_pass() = default;
            render_pass( const logical_device* p_logical_device );
            render_pass( const logical_device* p_logical_device, const graphics::swapchain& swapchain );
            render_pass( const render_pass& render_pass ) = delete;
            render_pass( render_pass&& render_pass ) noexcept;
            ~render_pass( );

            const VkRenderPass& get() const
            {
                return render_pass_handle_;
            }

            render_pass& operator=( const render_pass& renderPass ) = delete;
            render_pass& operator=( render_pass&& render_pass ) noexcept;

        private:
           const logical_device* p_logical_device_;

           VkRenderPass render_pass_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_RENDER_PASS_H
