/*!
 *
 */

#include "graphics_pipeline.h"
#include "vertex.h"

namespace vk
{
    namespace graphics
    {
        graphics_pipeline::graphics_pipeline( const core::logical_device* p_logical_device,
                                              const core::render_pass& render_pass,
                                              const swapchain& swapchain,
                                              core::shader_module& vertex_shader,
                                              core::shader_module& fragment_shader )
            :
            p_logical_device_( p_logical_device )
        {
            auto vert_shader_stage_info = vertex_shader.create_shader_stage_info( VK_SHADER_STAGE_VERTEX_BIT );
            auto frag_shader_stage_info = fragment_shader.create_shader_stage_info( VK_SHADER_STAGE_FRAGMENT_BIT );

            VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

            auto binding_description = vertex::get_binding_description();
            auto attribute_description = vertex::get_attribute_descriptions();

            VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {};
            vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_state_info.vertexBindingDescriptionCount = 1;
            vertex_input_state_info.pVertexBindingDescriptions = &binding_description;
            vertex_input_state_info.vertexAttributeDescriptionCount = static_cast<uint32_t>( attribute_description.size() );
            vertex_input_state_info.pVertexAttributeDescriptions = attribute_description.data();

            VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info = {};
            input_assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            input_assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>( swapchain.get_extent().width );
            viewport.height = static_cast<float>( swapchain.get_extent().height );
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor = {};
            scissor.offset = { 0, 0 };
            scissor.extent = swapchain.get_extent();

            VkPipelineViewportStateCreateInfo viewport_state_info = {};
            viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewport_state_info.viewportCount = 1;
            viewport_state_info.pViewports = &viewport;
            viewport_state_info.scissorCount = 1;
            viewport_state_info.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterization_state_info = {};
            rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterization_state_info.depthClampEnable = VK_FALSE;
            rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
            rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
            rasterization_state_info.lineWidth = 1.0f;
            rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterization_state_info.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisample_state_info = {};
            multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisample_state_info.sampleShadingEnable = VK_FALSE;
            multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState colour_blend_attachment_state = {};
            colour_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colour_blend_attachment_state.blendEnable = VK_FALSE;
            colour_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colour_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colour_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
            colour_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colour_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colour_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

            VkPipelineColorBlendStateCreateInfo colour_blend_state_info = {};
            colour_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colour_blend_state_info.logicOpEnable = VK_FALSE;
            colour_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
            colour_blend_state_info.attachmentCount = 1;
            colour_blend_state_info.pAttachments = &colour_blend_attachment_state;
            colour_blend_state_info.blendConstants[0] = 0.0f;
            colour_blend_state_info.blendConstants[1] = 0.0f;
            colour_blend_state_info.blendConstants[2] = 0.0f;
            colour_blend_state_info.blendConstants[3] = 0.0f;

            VkDynamicState dynamic_state[] =
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
            };

            VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
            dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic_state_info.dynamicStateCount = 2;
            dynamic_state_info.pDynamicStates = dynamic_state;

            VkPipelineLayoutCreateInfo pipeline_layout_info = {};
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.setLayoutCount = 0;
            pipeline_layout_info.pSetLayouts = nullptr;
            pipeline_layout_info.pushConstantRangeCount = 0;
            pipeline_layout_info.pPushConstantRanges = nullptr;

            pipeline_layout_handle_ = p_logical_device_->create_pipeline_layout( pipeline_layout_info );

            VkGraphicsPipelineCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            create_info.stageCount = 2;
            create_info.pStages = shader_stages;
            create_info.pVertexInputState = &vertex_input_state_info;
            create_info.pInputAssemblyState = &input_assembly_state_info;
            create_info.pViewportState = &viewport_state_info;
            create_info.pRasterizationState = &rasterization_state_info;
            create_info.pMultisampleState = &multisample_state_info;
            create_info.pDepthStencilState = nullptr;
            create_info.pColorBlendState = &colour_blend_state_info;
            create_info.pDynamicState = &dynamic_state_info;
            create_info.layout = pipeline_layout_handle_;
            create_info.renderPass = render_pass.get();
            create_info.subpass = 0;

            pipeline_handle_ = p_logical_device_->create_graphics_pipeline( VK_NULL_HANDLE, create_info );
        }
        graphics_pipeline::graphics_pipeline( graphics_pipeline&& graphics_pipeline ) noexcept
        {
            *this = std::move( graphics_pipeline );
        }
        graphics_pipeline::~graphics_pipeline( )
        {
            if( pipeline_handle_ != VK_NULL_HANDLE )
                pipeline_handle_ = p_logical_device_->destroy_pipeline( pipeline_handle_ );

            if( pipeline_layout_handle_ != VK_NULL_HANDLE )
                pipeline_layout_handle_ = p_logical_device_->destroy_pipeline_layout( pipeline_layout_handle_ );
        }

        graphics_pipeline&
        graphics_pipeline::operator=( graphics_pipeline&& graphics_pipeline ) noexcept
        {
            if( this != &graphics_pipeline )
            {
                if( pipeline_handle_ != VK_NULL_HANDLE )
                    pipeline_handle_ = p_logical_device_->destroy_pipeline( pipeline_handle_ );

                if( pipeline_layout_handle_ != VK_NULL_HANDLE )
                    pipeline_layout_handle_ = p_logical_device_->destroy_pipeline_layout( pipeline_layout_handle_ );

                pipeline_handle_ = graphics_pipeline.pipeline_handle_;
                graphics_pipeline.pipeline_handle_ = VK_NULL_HANDLE;

                pipeline_layout_handle_ = graphics_pipeline.pipeline_layout_handle_;
                graphics_pipeline.pipeline_layout_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = graphics_pipeline.p_logical_device_;
            }

            return *this;
        }
    }
}