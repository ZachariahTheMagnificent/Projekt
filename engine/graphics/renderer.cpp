/*!
 *
 */

#include <cstring>
#include <iostream>
#include <set>

#include "renderer.h"

#include "../vulkan/helpers/queue_family_indices.h"

#include "../utils/exception/vulkan_exception.h"
#include "../utils/file_io/read.h"

constexpr const int MAX_FRAMES_IN_FLIGHT = 2;

renderer::renderer( const window &window )
    :
    window_( window )
{
    auto extensions = window_.get_required_extensions();

    instance_           = vk::core::instance( window_.get_title(), validation_layers, extensions );

    if constexpr ( enable_validation_layers )
        debug_report_   = vk::core::debug_report( &instance_ );

    surface_            = vk::graphics::surface( &instance_, window_ );
    gpu_                = vk::core::physical_device( instance_, surface_ );
    logical_device_     = vk::core::logical_device( gpu_, validation_layers, device_extensions );
    graphics_queue_     = vk::core::queue( logical_device_, gpu_, vk::helpers::queue_family_type::e_graphics, 0 );
    present_queue_      = vk::core::queue( logical_device_, gpu_, vk::helpers::queue_family_type::e_present, 0 );
    command_pool_       = vk::core::command_pool( gpu_, &logical_device_, vk::helpers::queue_family_type::e_graphics );

    vertex_shader_      = vk::core::shader_module( &logical_device_, "../game/shaders/vert.spv" );
    fragment_shader_    = vk::core::shader_module( &logical_device_, "../game/shaders/frag.spv" );

    /*
    instance_handle_                = create_instance();

    if( enable_validation_layers )
        debug_callback_handle_      = create_debug_callback();

    surface_handle_                 = create_surface();
    gpu_handle_                     = pick_gpu();
    device_handle_                  = create_device();
    graphics_queue_handle_          = get_queue( queue_family_indices_.graphics_family,  0 );
    present_queue_handle_           = get_queue( queue_family_indices_.present_family, 0 );
    command_pool_handle_            = create_command_pool();
     */

    create_vertex_buffer();
    create_index_buffer();

    recreate_swapchain();

    image_available_semaphore_handles_ = create_semaphores();
    render_finished_semaphore_handles_ = create_semaphores();
    fences_ = create_fences();
}

renderer::~renderer()
{
    graphics_queue_.wait_idle();

    vkDestroyBuffer( logical_device_.get(), index_buffer_handle_, nullptr );
    vkFreeMemory( logical_device_.get(), index_buffer_memory_handle_, nullptr );

    vkDestroyBuffer( logical_device_.get(), vertex_buffer_handle_, nullptr );
    vkFreeMemory( logical_device_.get(), vertex_buffer_memory_handle_, nullptr );

    cleanup_swapchain();

    for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
    {
        vkDestroySemaphore( logical_device_.get(), image_available_semaphore_handles_[i], nullptr );
        vkDestroySemaphore( logical_device_.get(), render_finished_semaphore_handles_[i], nullptr );
        vkDestroyFence( logical_device_.get(), fences_[i], nullptr );
    }
}

void
renderer::draw_frame( )
{
    vkWaitForFences( logical_device_.get(), 1, &fences_[current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max() );
    vkResetFences( logical_device_.get(), 1, &fences_[current_frame] );

    uint32_t image_index;
    auto result = vkAcquireNextImageKHR( logical_device_.get(), swapchain_.get(), std::numeric_limits<uint64_t>::max(),
                                         image_available_semaphore_handles_[current_frame], VK_NULL_HANDLE, &image_index );

    if( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        recreate_swapchain();
    }
    else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    {
        throw vulkan_exception{ "Failed to acquire swapchain image", __FILE__, __LINE__ };
    }

    VkSemaphore wait_semaphores[] = { image_available_semaphore_handles_[current_frame] };
    VkSemaphore signal_semaphores[] = { render_finished_semaphore_handles_[current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_handles_[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    graphics_queue_.submit( submit_info, fences_[current_frame] );

    VkSwapchainKHR swapchains[] = { swapchain_.get() };
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    result = present_queue_.present( present_info );

    if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
    {
        recreate_swapchain();
    }
    else if( result != VK_SUCCESS )
    {
        throw vulkan_exception{ "Failed to present swapchain image", __FILE__, __LINE__ };
    }

    current_frame = ( current_frame + 1 ) % MAX_FRAMES_IN_FLIGHT;
}

void
renderer::recreate_swapchain( )
{
    cleanup_swapchain();

    swapchain_ = vk::graphics::swapchain( &logical_device_, gpu_, surface_ );
    render_pass_ = vk::core::render_pass( &logical_device_, swapchain_ );

    graphics_pipeline_ = vk::graphics::graphics_pipeline( &logical_device_, render_pass_, swapchain_, vertex_shader_, fragment_shader_ );
    //create_pso();

    swapchain_framebuffer_handles_  = create_framebuffers();
    command_buffer_handles_         = create_command_buffers();

    record_commands();
}

void
renderer::cleanup_swapchain()
{
    for( auto framebuffer : swapchain_framebuffer_handles_ )
    {
        vkDestroyFramebuffer( logical_device_.get(), framebuffer, nullptr );
    }

    command_pool_.free_command_buffers( command_buffer_handles_ );

    /*
    vkDestroyPipeline( logical_device_.get(), pso_handle_, nullptr );
    vkDestroyPipelineLayout( logical_device_.get(), pso_layout_handle_, nullptr );
    */
}

VkSemaphore
renderer::create_semaphore( ) const
{
    VkSemaphore semaphore;

    VkSemaphoreCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    semaphore = logical_device_.create_semaphore( create_info );

    return semaphore;
}
VkFence
renderer::create_fence() const
{
    VkFence fence;

    VkFenceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    fence = logical_device_.create_fence( create_info );

    return fence;
}

std::vector<VkSemaphore>
renderer::create_semaphores() const
{
    std::vector<VkSemaphore> semaphores( MAX_FRAMES_IN_FLIGHT );

    for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
    {
        semaphores[i] = create_semaphore();
    }

    return semaphores;
}
std::vector<VkFence>
renderer::create_fences() const
{
    std::vector<VkFence> fences( MAX_FRAMES_IN_FLIGHT );

    for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
    {
        fences[i] = create_fence();
    }

    return fences;
}

std::vector<VkFramebuffer >
renderer::create_framebuffers( )
{
    std::vector<VkFramebuffer> framebuffers( swapchain_.get_count() );

    for( auto i = 0; i < swapchain_.get_count(); ++i )
    {
        VkImageView attachments[] =
        {
            swapchain_.get_image_view( i )
        };

        VkFramebufferCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = render_pass_.get();
        create_info.attachmentCount = 1;
        create_info.pAttachments = attachments;
        create_info.width = swapchain_.get_extent().width;
        create_info.height = swapchain_.get_extent().height;
        create_info.layers = 1;

        if( vkCreateFramebuffer( logical_device_.get(), &create_info, nullptr, &framebuffers[i] ) != VK_SUCCESS )
            throw vulkan_exception{ "Failed to create framebuffer", __FILE__, __LINE__ };
    }

    return framebuffers;
}
std::vector<VkCommandBuffer>
renderer::create_command_buffers( )
{
    std::vector<VkCommandBuffer> command_buffers( swapchain_.get_count() );

    VkCommandBufferAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = command_pool_.get();
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = static_cast<uint32_t>( swapchain_framebuffer_handles_.size() );

    if( vkAllocateCommandBuffers( logical_device_.get(), &allocate_info, command_buffers.data() ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to create command buffers", __FILE__, __LINE__ };

    return command_buffers;
}

/*
void
renderer::create_pso( )
{
    // shaders
    auto vert_shader_code = read_from_binary_file( "../game/shaders/vert.spv" );
    auto frag_shader_code = read_from_binary_file( "../game/shaders/frag.spv" );

    VkShaderModule vert_shader_module = create_shader_module( vert_shader_code );
    VkShaderModule frag_shader_module = create_shader_module( frag_shader_code );

    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = frag_shader_module;
    frag_shader_stage_info.pName = "main";

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
    viewport.width = static_cast<float>( swapchain_.get_extent().width );
    viewport.height = static_cast<float>( swapchain_.get_extent().height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain_.get_extent();

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

    vk::DynamicState dynamic_state[] =
    {
        vk::DynamicState::eViewport
    };

    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    dynamic_state_info.dynamicStateCount = 1;
    dynamic_state_info.pDynamicStates = dynamic_state;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if( vkCreatePipelineLayout( logical_device_.get(), &pipeline_layout_info, nullptr, &pso_layout_handle_ ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to create pipeline layout", __FILE__, __LINE__ };

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
    //create_info.pDynamicState = &dynamic_state_info;
    create_info.layout = pso_layout_handle_;
    create_info.renderPass = render_pass_.get();
    create_info.subpass = 0;

    if( vkCreateGraphicsPipelines( logical_device_.get(), VK_NULL_HANDLE, 1, &create_info, nullptr, &pso_handle_ ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to create pipeline", __FILE__, __LINE__ };

    vkDestroyShaderModule( logical_device_.get(), frag_shader_module, nullptr );
    vkDestroyShaderModule( logical_device_.get(), vert_shader_module, nullptr );
}
VkShaderModule
renderer::create_shader_module( const std::string &shader_code )
{
    VkShaderModule shader;

    VkShaderModuleCreateInfo create_info = { };
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.size( );
    create_info.pCode = reinterpret_cast<const uint32_t *>( shader_code.data( ));

    if ( vkCreateShaderModule( logical_device_.get(), &create_info, nullptr, &shader ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to create shader module", __FILE__, __LINE__ };

    return shader;
}
*/

void
renderer::record_commands( )
{
    for( auto i = 0; i < command_buffer_handles_.size(); ++i )
    {
        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if( vkBeginCommandBuffer( command_buffer_handles_[i], &command_buffer_begin_info ) != VK_SUCCESS )
            throw vulkan_exception{ "Failed to begin recording command buffer", __FILE__, __LINE__ };

            VkRenderPassBeginInfo render_pass_begin_info = {};
            render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_begin_info.renderPass = render_pass_.get();
            render_pass_begin_info.framebuffer = swapchain_framebuffer_handles_[i];
            render_pass_begin_info.renderArea.offset = { 0, 0 };
            render_pass_begin_info.renderArea.extent = swapchain_.get_extent();

            VkClearValue clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };
            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.pClearValues = &clear_colour;

            vkCmdBeginRenderPass( command_buffer_handles_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );

                vkCmdBindPipeline( command_buffer_handles_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_.get() );

                VkBuffer vertex_buffers[] = { vertex_buffer_handle_ };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers( command_buffer_handles_[i], 0, 1, vertex_buffers, offsets );
                vkCmdBindIndexBuffer( command_buffer_handles_[i], index_buffer_handle_, 0, VK_INDEX_TYPE_UINT16 );

                vkCmdDrawIndexed( command_buffer_handles_[i], static_cast<uint32_t>( indices_.size() ), 1, 0, 0, 0 );

            vkCmdEndRenderPass( command_buffer_handles_[i] );

        if( vkEndCommandBuffer( command_buffer_handles_[i] ) != VK_SUCCESS )
            throw vulkan_exception{ "Failed to record command buffer", __FILE__, __LINE__ };
    }
}

void
renderer::create_buffer( VkDeviceSize& size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                         VkBuffer& buffer, VkDeviceMemory& buffer_memory )
{
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if( vkCreateBuffer( logical_device_.get(), &create_info, nullptr, &buffer ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to create vertex buffer", __FILE__, __LINE__ };

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements( logical_device_.get(), buffer, &mem_reqs );

    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = mem_reqs.size;
    allocate_info.memoryTypeIndex = find_memory_type( mem_reqs.memoryTypeBits, properties );

    if( vkAllocateMemory( logical_device_.get(), &allocate_info, nullptr, &buffer_memory ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to allocate device memory", __FILE__, __LINE__ };

    vkBindBufferMemory( logical_device_.get(), buffer, buffer_memory, 0 );
}
void
renderer::copy_buffer( VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize &size )
{
    VkCommandBufferAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandPool = command_pool_.get();
    allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers( logical_device_.get(), &allocate_info, &command_buffer );

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer( command_buffer, &begin_info );

        VkBufferCopy copy_region = {};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = size;

        vkCmdCopyBuffer( command_buffer, src_buffer, dst_buffer, 1, &copy_region );

    vkEndCommandBuffer( command_buffer );

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    graphics_queue_.submit( submit_info, VK_NULL_HANDLE );
    graphics_queue_.wait_idle();

    command_pool_.free_command_buffer( command_buffer );
}
void
renderer::create_vertex_buffer()
{
    VkDeviceSize buffer_size = sizeof( vertices[0] ) * vertices.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   staging_buffer, staging_buffer_memory );

    void* data;
    vkMapMemory( logical_device_.get(), staging_buffer_memory, 0, buffer_size, 0, &data );
    memcpy( data, vertices.data(), static_cast<size_t>( buffer_size ) );
    vkUnmapMemory( logical_device_.get(), staging_buffer_memory );

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   vertex_buffer_handle_, vertex_buffer_memory_handle_ );

    copy_buffer( staging_buffer, vertex_buffer_handle_, buffer_size );

    vkDestroyBuffer( logical_device_.get(), staging_buffer, nullptr );
    vkFreeMemory( logical_device_.get(), staging_buffer_memory, nullptr );
}
void
renderer::create_index_buffer()
{
    VkDeviceSize buffer_size = sizeof( indices_[0] ) * indices_.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   staging_buffer, staging_buffer_memory );

    void* data;
    vkMapMemory( logical_device_.get(), staging_buffer_memory, 0, buffer_size, 0, &data );
    memcpy( data, indices_.data(), static_cast<size_t>( buffer_size ) );
    vkUnmapMemory( logical_device_.get(), staging_buffer_memory );

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   index_buffer_handle_, index_buffer_memory_handle_ );

    copy_buffer( staging_buffer, index_buffer_handle_, buffer_size );

    vkDestroyBuffer( logical_device_.get(), staging_buffer, nullptr );
    vkFreeMemory( logical_device_.get(), staging_buffer_memory, nullptr );
}
uint32_t
renderer::find_memory_type( uint32_t type_filter, VkMemoryPropertyFlags properties )
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties( gpu_.get(), &mem_properties );

    for( uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i )
    {
        if( ( type_filter & ( 1 << i ) ) && ( mem_properties.memoryTypes[i].propertyFlags & properties ) == properties )
        {
            return i;
        }
    }

    throw vulkan_exception{ "Failed to find a suitable memory type", __FILE__, __LINE__ };
}

renderer::SwapchainSupportDetails
renderer::query_swapchain_support( VkPhysicalDevice &gpu_handle ) const
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( gpu_handle, surface_.get(), &details.capabilities );

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR( gpu_handle, surface_.get(), &format_count, nullptr );

    if( format_count != 0 )
    {
        details.formats.resize( format_count );
        vkGetPhysicalDeviceSurfaceFormatsKHR( gpu_handle, surface_.get(), &format_count, details.formats.data() );
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR( gpu_handle, surface_.get(), &present_mode_count, nullptr );

    if( present_mode_count != 0 )
    {
        details.present_modes.resize( present_mode_count );
        vkGetPhysicalDeviceSurfacePresentModesKHR( gpu_handle, surface_.get(), &present_mode_count, details.present_modes.data() );
    }

    return details;
}

VkSurfaceFormatKHR
renderer::choose_surface_format( const std::vector<VkSurfaceFormatKHR> &available_formats ) const
{
    if( available_formats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED )
        return { VK_FORMAT_B8G8R8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    for( const auto& available_format : available_formats )
    {
        if( available_format.format == VK_FORMAT_B8G8R8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return available_format;
        }
    }

    return available_formats[0];
}
VkPresentModeKHR
renderer::choose_present_mode( const std::vector<VkPresentModeKHR> &available_present_modes ) const
{
    for( const auto& available_present_mode : available_present_modes )
    {
        if( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
            return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D
renderer::choose_extent( VkSurfaceCapabilitiesKHR &capabilities ) const
{
    if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actual_extent = { static_cast<uint32_t>( window_.get_width() ),
                                     static_cast<uint32_t>( window_.get_height() ) };

        actual_extent.width = std::max( capabilities.minImageExtent.width,
                                        std::min( capabilities.minImageExtent.width, actual_extent.width ) );

        actual_extent.height = std::max( capabilities.minImageExtent.height,
                                         std::min( capabilities.minImageExtent.height, actual_extent.height ) );

        return actual_extent;
    }
}
