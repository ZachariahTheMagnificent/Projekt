/*!
 *
 */

#ifndef PROJEKT_GRAPHICS_PIPELINE_H
#define PROJEKT_GRAPHICS_PIPELINE_H

#include "swapchain.h"
#include "../core/logical_device.h"
#include "../core/render_pass.h"
#include "../core/shader_module.h"
#include "../core/descriptor_set_layout.h"

namespace vk
{
    namespace graphics
    {
        class graphics_pipeline
        {
        public:
            graphics_pipeline( ) = default;
            graphics_pipeline( const core::logical_device* p_logical_device,
                               const core::render_pass& render_pass, const swapchain& swapchain,
                               const core::descriptor_set_layout& descriptor_set_layout,
                               core::shader_module& vertex_shader, core::shader_module& fragment_shader );
            graphics_pipeline( const graphics_pipeline& graphics_pipeline ) = delete;
            graphics_pipeline( graphics_pipeline&& graphics_pipeline ) noexcept;
            ~graphics_pipeline( );

            VkPipeline& get()
            {
                return pipeline_handle_;
            }

            VkPipelineLayout& get_layout()
            {
                return pipeline_layout_handle_;
            }

            graphics_pipeline& operator=( const graphics_pipeline& graphics_pipeline ) = delete;
            graphics_pipeline& operator=( graphics_pipeline&& graphics_pipeline ) noexcept;

        private:


        private:
            const core::logical_device* p_logical_device_;

            VkPipeline pipeline_handle_ = VK_NULL_HANDLE;
            VkPipelineLayout pipeline_layout_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_GRAPHICS_PIPELINE_H
