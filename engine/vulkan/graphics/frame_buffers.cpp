/*!
 *
 */

#include "frame_buffers.h"

namespace vk
{
    namespace graphics
    {

        frame_buffers::frame_buffers( const core::logical_device* p_logical_device,
                                      const core::render_pass& render_pass, const swapchain& swapchain, uint32_t count )
            :
            p_logical_device_( p_logical_device ),
            count_( count )
        {
            VkFramebufferCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            create_info.renderPass = render_pass.get();
            create_info.width = swapchain.get_extent().width;
            create_info.height = swapchain.get_extent().height;
            create_info.layers = 1;

            frame_buffer_handles_ = p_logical_device_->create_frame_buffers( swapchain.get_image_views(), create_info, count_ );
        }
        frame_buffers::frame_buffers( frame_buffers&& frame_buffers ) noexcept
        {
            *this = std::move( frame_buffers );
        }
        frame_buffers::~frame_buffers( )
        {
            if( frame_buffer_handles_ != VK_NULL_HANDLE )
                frame_buffer_handles_ = p_logical_device_->destroy_frame_buffers( frame_buffer_handles_, count_ );
        }

        frame_buffers& frame_buffers::operator=( frame_buffers&& frame_buffers ) noexcept
        {
            if( this != &frame_buffers )
            {
                if( frame_buffer_handles_ != VK_NULL_HANDLE )
                    frame_buffer_handles_ = p_logical_device_->destroy_frame_buffers( frame_buffer_handles_, count_ );

                count_ = frame_buffers.count_;
                frame_buffers.count_ = 0;

                frame_buffer_handles_ = frame_buffers.frame_buffer_handles_;
                frame_buffers.frame_buffer_handles_ = VK_NULL_HANDLE;

                p_logical_device_ = frame_buffers.p_logical_device_;
            }

            return *this;
        }
    }
}