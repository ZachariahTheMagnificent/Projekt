/*!
 *
 */

#include "render_pass.h"

namespace vk
{
    namespace core
    {
        render_pass::render_pass( const logical_device* p_logical_device )
            :
            p_logical_device_( p_logical_device )
        {
            VkSubpassDescription subpass_description = {};
            subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass_description.colorAttachmentCount = 0;

            VkSubpassDependency subpass_dependency = {};
            subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpass_dependency.dstSubpass = 0;
            subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.srcAccessMask = 0;
            subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            create_info.attachmentCount = 0;
            create_info.subpassCount = 1;
            create_info.pSubpasses = &subpass_description;
            create_info.dependencyCount = 1;
            create_info.pDependencies = &subpass_dependency;

            render_pass_handle_ = p_logical_device_->create_render_pass( create_info );
        }
        render_pass::render_pass( const logical_device* p_logical_device, const graphics::swapchain& swapchain )
            :
            p_logical_device_( p_logical_device )
        {
            VkAttachmentDescription colour_attachment = {};
            colour_attachment.format = swapchain.get_format();
            colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colour_attachment_ref = {};
            colour_attachment_ref.attachment = 0;
            colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass_description = {};
            subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass_description.colorAttachmentCount = 1;
            subpass_description.pColorAttachments = &colour_attachment_ref;

            VkSubpassDependency subpass_dependency = {};
            subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpass_dependency.dstSubpass = 0;
            subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.srcAccessMask = 0;
            subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            create_info.attachmentCount = 1;
            create_info.pAttachments = &colour_attachment;
            create_info.subpassCount = 1;
            create_info.pSubpasses = &subpass_description;
            create_info.dependencyCount = 1;
            create_info.pDependencies = &subpass_dependency;

            render_pass_handle_ = p_logical_device_->create_render_pass( create_info );
        }
        render_pass::render_pass( render_pass&& render_pass ) noexcept
        {
            *this = std::move( render_pass );
        }

        render_pass::~render_pass( )
        {
            if( render_pass_handle_ != VK_NULL_HANDLE )
                render_pass_handle_ = p_logical_device_->destroy_render_pass( render_pass_handle_ );
        }

        render_pass& render_pass::operator=( render_pass&& render_pass ) noexcept
        {
            if( this != &render_pass )
            {
                if( render_pass_handle_ != VK_NULL_HANDLE )
                    render_pass_handle_ = p_logical_device_->destroy_render_pass( render_pass_handle_ );

                render_pass_handle_ = render_pass.render_pass_handle_;
                render_pass.render_pass_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = render_pass.p_logical_device_;
            }

            return *this;
        }
    }
}