/*!
 *
 */

#include <cstring>
#include <iostream>
#include <set>

#include "Renderer.h"

#include "../Utils/Exception/VulkanException.h"
#include "../Utils/File/Read.h"

constexpr const int MAX_FRAMES_IN_FLIGHT = 2;

Renderer::Renderer( Window &window )
    :
    window_( window )
{
    instance_ = Vk::Instance( window_ );

    if( enable_validation_layers )
        debug_report_ = Vk::DebugReport( &instance_ );

    surface_ = Vk::Surface( window_, &instance_ );
    gpu_ = Vk::PhysicalDevice( &instance_, &surface_ );

    queue_family_indices_ = gpu_.find_queue_family_indices();

    device_handle_                  = create_device( );
    graphics_queue_handle_          = get_queue( queue_family_indices_.graphics_family,  0 );
    present_queue_handle_           = get_queue( queue_family_indices_.present_family, 0 );
    command_pool_handle_            = create_command_pool();

    create_vertex_buffer();
    create_index_buffer();

    recreate_swapchain();

    image_available_semaphore_handles_ = create_semaphores();
    render_finished_semaphore_handles_ = create_semaphores();
    fences_ = create_fences();
}

Renderer::~Renderer()
{
    vkQueueWaitIdle( graphics_queue_handle_ );

    vkDestroyBuffer( device_handle_, index_buffer_handle_, nullptr );
    vkFreeMemory( device_handle_, index_buffer_memory_handle_, nullptr );

    vkDestroyBuffer( device_handle_, vertex_buffer_handle_, nullptr );
    vkFreeMemory( device_handle_, vertex_buffer_memory_handle_, nullptr );

    cleanup_swapchain();

    for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
    {
        vkDestroySemaphore( device_handle_, image_available_semaphore_handles_[i], nullptr );
        vkDestroySemaphore( device_handle_, render_finished_semaphore_handles_[i], nullptr );
        vkDestroyFence( device_handle_, fences_[i], nullptr );
    }

    vkDestroySwapchainKHR( device_handle_, swapchain_handle_, nullptr );

    vkDestroyCommandPool( device_handle_, command_pool_handle_, nullptr );

    vkDestroyDevice( device_handle_, nullptr );
}

void
Renderer::draw_frame( )
{
    vkWaitForFences( device_handle_, 1, &fences_[current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max() );
    vkResetFences( device_handle_, 1, &fences_[current_frame] );

    uint32_t image_index;
    auto result = vkAcquireNextImageKHR( device_handle_, swapchain_handle_, std::numeric_limits<uint64_t>::max(),
                                         image_available_semaphore_handles_[current_frame], VK_NULL_HANDLE, &image_index );

    if( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        recreate_swapchain();
    }
    else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    {
        throw VulkanException{ "Failed to acquire swapchain image", __FILE__, __LINE__ };
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

    if( vkQueueSubmit( graphics_queue_handle_, 1, &submit_info, fences_[current_frame] ) != VK_SUCCESS )
        throw VulkanException{ "Failed to submit graphics queue", __FILE__, __LINE__ };

    VkSwapchainKHR swapchains[] = { swapchain_handle_ };
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    result = vkQueuePresentKHR( present_queue_handle_, &present_info );

    if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
    {
        recreate_swapchain();
    }
    else if( result != VK_SUCCESS )
    {
        throw VulkanException{ "Failed to present swapchain image", __FILE__, __LINE__ };
    }

    current_frame = ( current_frame + 1 ) % MAX_FRAMES_IN_FLIGHT;
}

void
Renderer::recreate_swapchain( )
{
    cleanup_swapchain();

    swapchain_handle_               = create_swapchain();
    swapchain_image_view_handles_   = create_image_views();
    render_pass_handle_             = create_render_pass();

    create_pso();

    swapchain_framebuffer_handles_  = create_framebuffers();
    command_buffer_handles_         = create_command_buffers();

    record_commands();
}

void
Renderer::cleanup_swapchain()
{
    for( auto framebuffer : swapchain_framebuffer_handles_ )
    {
        vkDestroyFramebuffer( device_handle_, framebuffer, nullptr );
    }

    vkFreeCommandBuffers( device_handle_, command_pool_handle_, static_cast<uint32_t>( command_buffer_handles_.size() ),
                          command_buffer_handles_.data() );

    vkDestroyPipeline( device_handle_, pso_handle_, nullptr );
    vkDestroyPipelineLayout( device_handle_, pso_layout_handle_, nullptr );
    vkDestroyRenderPass( device_handle_, render_pass_handle_, nullptr );

    for( auto image_view : swapchain_image_view_handles_ )
    {
        vkDestroyImageView( device_handle_, image_view, nullptr );
    }
}

VkDevice
Renderer::create_device( )
{
    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    std::set<int> unique_queue_families = { queue_family_indices_.graphics_family, queue_family_indices_.present_family };

    float queue_priority = 1.0f;
    for( const auto& queue_family : unique_queue_families )
    {
        VkDeviceQueueCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        create_info.queueFamilyIndex = queue_family;
        create_info.queueCount = 1;
        create_info.pQueuePriorities = &queue_priority;

        queue_infos.emplace_back( create_info );
    }

    VkPhysicalDeviceFeatures gpu_features = {};

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<std::uint32_t>( queue_infos.size() );
    create_info.pQueueCreateInfos = queue_infos.data();
    create_info.pEnabledFeatures = &gpu_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>( device_extensions.size() );
    create_info.ppEnabledExtensionNames = device_extensions.data();

    if( enable_validation_layers )
    {
        create_info.enabledLayerCount = static_cast<uint32_t>( validation_layers.size() );
        create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    return gpu_.create_device( create_info );
}
VkQueue
Renderer::get_queue( int32_t family_index, uint32_t queue_index )
{
    VkQueue queue;

    vkGetDeviceQueue( device_handle_, static_cast<uint32_t>( family_index ), queue_index, &queue );

    return queue;
}
VkCommandPool
Renderer::create_command_pool( )
{
    VkCommandPool command_pool;

    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = static_cast<uint32_t>( queue_family_indices_.graphics_family );

    if( vkCreateCommandPool( device_handle_, &create_info, nullptr, &command_pool ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create command pool", __FILE__, __LINE__ };

    return command_pool;
}
VkSemaphore
Renderer::create_semaphore( )
{
    VkSemaphore semaphore;

    VkSemaphoreCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if( vkCreateSemaphore( device_handle_, &create_info, nullptr, &semaphore ) != VK_SUCCESS )
        throw VulkanException{ "failed to create semaphore", __FILE__, __LINE__ };

    return semaphore;
}
VkFence
Renderer::create_fence()
{
    VkFence fence;

    VkFenceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if( vkCreateFence( device_handle_, &create_info, nullptr, &fence ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create fence", __FILE__, __LINE__ };

    return fence;
}

std::vector<VkSemaphore>
Renderer::create_semaphores()
{
    std::vector<VkSemaphore> semaphores( MAX_FRAMES_IN_FLIGHT );

    for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
    {
        semaphores[i] = create_semaphore();
    }

    return semaphores;
}
std::vector<VkFence>
Renderer::create_fences()
{
    std::vector<VkFence> fences( MAX_FRAMES_IN_FLIGHT );

    for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
    {
        fences[i] = create_fence();
    }

    return fences;
}

VkSwapchainKHR
Renderer::create_swapchain( )
{
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    auto support_details = gpu_.query_swapchain_support( );

    auto surface_format = choose_surface_format( support_details.formats );
    auto present_mode = choose_present_mode( support_details.present_modes );
    auto extent = choose_extent( support_details.capabilities );

    uint32_t image_count = support_details.capabilities.minImageCount + 1;
    if( support_details.capabilities.maxImageCount > 0 &&
        image_count > support_details.capabilities.maxImageCount )
    {
        image_count = support_details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_.get();
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] =
    {
        static_cast<uint32_t>( queue_family_indices_.graphics_family ),
        static_cast<uint32_t>( queue_family_indices_.present_family )
    };

    if( queue_family_indices_.graphics_family != queue_family_indices_.present_family )
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = support_details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    swapchain_format_handle_ = surface_format.format;
    swapchain_extent_handle_ = extent;

    auto old_swapchain_handle_ = swapchain_handle_;
    create_info.oldSwapchain = old_swapchain_handle_;

    if( vkCreateSwapchainKHR( device_handle_, &create_info, nullptr, &swapchain ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create swapchain", __FILE__, __LINE__ };

    vkGetSwapchainImagesKHR( device_handle_, swapchain, &image_count, nullptr );
    swapchain_images_.resize( image_count );
    vkGetSwapchainImagesKHR( device_handle_, swapchain, &image_count, swapchain_images_.data() );

    return swapchain;
}
VkSwapchainKHR
Renderer::create_swapchain( VkSwapchainKHR& old_swapchain )
{
    VkSwapchainKHR swapchain;

    auto support_details = gpu_.query_swapchain_support( );

    auto surface_format = choose_surface_format( support_details.formats );
    auto present_mode = choose_present_mode( support_details.present_modes );
    auto extent = choose_extent( support_details.capabilities );

    uint32_t image_count = support_details.capabilities.minImageCount + 1;
    if( support_details.capabilities.maxImageCount > 0 &&
        image_count > support_details.capabilities.maxImageCount )
    {
        image_count = support_details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_.get();
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] =
    {
        static_cast<uint32_t>( queue_family_indices_.graphics_family ),
        static_cast<uint32_t>( queue_family_indices_.present_family )
    };

    if( queue_family_indices_.graphics_family != queue_family_indices_.present_family )
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = support_details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    swapchain_format_handle_ = surface_format.format;
    swapchain_extent_handle_ = extent;

    create_info.oldSwapchain = old_swapchain;

    if( vkCreateSwapchainKHR( device_handle_, &create_info, nullptr, &swapchain ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create swapchain", __FILE__, __LINE__ };

    vkGetSwapchainImagesKHR( device_handle_, swapchain, &image_count, nullptr );
    swapchain_images_.resize( image_count );
    vkGetSwapchainImagesKHR( device_handle_, swapchain, &image_count, swapchain_images_.data() );

    return swapchain;
}
std::vector<VkImageView>
Renderer::create_image_views( )
{
    std::vector<VkImageView> image_views( swapchain_images_.size() );

    for( auto i = 0; i < swapchain_images_.size(); ++i )
    {
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = swapchain_images_[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swapchain_format_handle_;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if( vkCreateImageView( device_handle_, &create_info, nullptr, &image_views[i] ) != VK_SUCCESS )
            throw VulkanException{ "Failed to create image view", __FILE__, __LINE__ };
    }

    return image_views;
}
std::vector<VkFramebuffer >
Renderer::create_framebuffers( )
{
    std::vector<VkFramebuffer> framebuffers( swapchain_image_view_handles_.size() );

    for( auto i = 0; i < swapchain_image_view_handles_.size(); ++i )
    {
        VkImageView attachments[] =
        {
            swapchain_image_view_handles_[i]
        };

        VkFramebufferCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = render_pass_handle_;
        create_info.attachmentCount = 1;
        create_info.pAttachments = attachments;
        create_info.width = swapchain_extent_handle_.width;
        create_info.height = swapchain_extent_handle_.height;
        create_info.layers = 1;

        if( vkCreateFramebuffer( device_handle_, &create_info, nullptr, &framebuffers[i] ) != VK_SUCCESS )
            throw VulkanException{ "Failed to create framebuffer", __FILE__, __LINE__ };
    }

    return framebuffers;
}
std::vector<VkCommandBuffer>
Renderer::create_command_buffers( )
{
    std::vector<VkCommandBuffer> command_buffers( swapchain_image_view_handles_.size() );

    VkCommandBufferAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = command_pool_handle_;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = static_cast<uint32_t>( swapchain_framebuffer_handles_.size() );

    if( vkAllocateCommandBuffers( device_handle_, &allocate_info, command_buffers.data() ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create command buffers", __FILE__, __LINE__ };

    return command_buffers;
}

VkRenderPass
Renderer::create_render_pass( )
{
    VkRenderPass render_pass;

    VkAttachmentDescription colour_attachment = {};
    colour_attachment.format = swapchain_format_handle_;
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

    if( vkCreateRenderPass( device_handle_, &create_info, nullptr, &render_pass ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create render pass", __FILE__, __LINE__ };

    return render_pass;
}
void
Renderer::create_pso( )
{
    // Shaders
    auto vert_shader_code = read_from_binary_file( "../Game/Shaders/vert.spv" );
    auto frag_shader_code = read_from_binary_file( "../Game/Shaders/frag.spv" );

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

    auto binding_description = Vertex::get_binding_description();
    auto attribute_description = Vertex::get_attribute_descriptions();

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
    viewport.width = static_cast<float>( swapchain_extent_handle_.width );
    viewport.height = static_cast<float>( swapchain_extent_handle_.height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain_extent_handle_;

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

    /*
    vk::DynamicState dynamic_state[] =
    {
        vk::DynamicState::eViewport
    };

    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    dynamic_state_info.dynamicStateCount = 1;
    dynamic_state_info.pDynamicStates = dynamic_state;
    */

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if( vkCreatePipelineLayout( device_handle_, &pipeline_layout_info, nullptr, &pso_layout_handle_ ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create pipeline layout", __FILE__, __LINE__ };

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
    create_info.renderPass = render_pass_handle_;
    create_info.subpass = 0;

    if( vkCreateGraphicsPipelines( device_handle_, VK_NULL_HANDLE, 1, &create_info, nullptr, &pso_handle_ ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create pipeline", __FILE__, __LINE__ };

    vkDestroyShaderModule( device_handle_, frag_shader_module, nullptr );
    vkDestroyShaderModule( device_handle_, vert_shader_module, nullptr );
}
VkShaderModule
Renderer::create_shader_module( const std::string &shader_code )
{
    VkShaderModule shader;

    VkShaderModuleCreateInfo create_info = { };
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.size( );
    create_info.pCode = reinterpret_cast<const uint32_t *>( shader_code.data( ));

    if ( vkCreateShaderModule( device_handle_, &create_info, nullptr, &shader ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create shader module", __FILE__, __LINE__ };

    return shader;
}

void
Renderer::record_commands( )
{
    for( auto i = 0; i < command_buffer_handles_.size(); ++i )
    {
        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if( vkBeginCommandBuffer( command_buffer_handles_[i], &command_buffer_begin_info ) != VK_SUCCESS )
            throw VulkanException{ "Failed to begin recording command buffer", __FILE__, __LINE__ };

            VkRenderPassBeginInfo render_pass_begin_info = {};
            render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_begin_info.renderPass = render_pass_handle_;
            render_pass_begin_info.framebuffer = swapchain_framebuffer_handles_[i];
            render_pass_begin_info.renderArea.offset = { 0, 0 };
            render_pass_begin_info.renderArea.extent = swapchain_extent_handle_;

            VkClearValue clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };
            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.pClearValues = &clear_colour;

            vkCmdBeginRenderPass( command_buffer_handles_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );

                vkCmdBindPipeline( command_buffer_handles_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pso_handle_ );

                VkBuffer vertex_buffers[] = { vertex_buffer_handle_ };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers( command_buffer_handles_[i], 0, 1, vertex_buffers, offsets );
                vkCmdBindIndexBuffer( command_buffer_handles_[i], index_buffer_handle_, 0, VK_INDEX_TYPE_UINT16 );

                vkCmdDrawIndexed( command_buffer_handles_[i], static_cast<uint32_t>( indices_.size() ), 1, 0, 0, 0 );

            vkCmdEndRenderPass( command_buffer_handles_[i] );

        if( vkEndCommandBuffer( command_buffer_handles_[i] ) != VK_SUCCESS )
            throw VulkanException{ "Failed to record command buffer", __FILE__, __LINE__ };
    }
}

void
Renderer::create_buffer( VkDeviceSize& size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                         VkBuffer& buffer, VkDeviceMemory& buffer_memory )
{
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if( vkCreateBuffer( device_handle_, &create_info, nullptr, &buffer ) != VK_SUCCESS )
        throw VulkanException{ "Failed to create vertex buffer", __FILE__, __LINE__ };

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements( device_handle_, buffer, &mem_reqs );

    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = mem_reqs.size;
    allocate_info.memoryTypeIndex = find_memory_type( mem_reqs.memoryTypeBits, properties );

    if( vkAllocateMemory( device_handle_, &allocate_info, nullptr, &buffer_memory ) != VK_SUCCESS )
        throw VulkanException{ "Failed to allocate device memory", __FILE__, __LINE__ };

    vkBindBufferMemory( device_handle_, buffer, buffer_memory, 0 );
}
void
Renderer::copy_buffer( VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize &size )
{
    VkCommandBufferAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandPool = command_pool_handle_;
    allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers( device_handle_, &allocate_info, &command_buffer );

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

    vkQueueSubmit( graphics_queue_handle_, 1, &submit_info, VK_NULL_HANDLE );
    vkQueueWaitIdle( graphics_queue_handle_ );

    vkFreeCommandBuffers( device_handle_, command_pool_handle_, 1, &command_buffer );
}
void
Renderer::create_vertex_buffer()
{
    VkDeviceSize buffer_size = sizeof( vertices[0] ) * vertices.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   staging_buffer, staging_buffer_memory );

    void* data;
    vkMapMemory( device_handle_, staging_buffer_memory, 0, buffer_size, 0, &data );
    memcpy( data, vertices.data(), static_cast<size_t>( buffer_size ) );
    vkUnmapMemory( device_handle_, staging_buffer_memory );

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   vertex_buffer_handle_, vertex_buffer_memory_handle_ );

    copy_buffer( staging_buffer, vertex_buffer_handle_, buffer_size );

    vkDestroyBuffer( device_handle_, staging_buffer, nullptr );
    vkFreeMemory( device_handle_, staging_buffer_memory, nullptr );
}
void
Renderer::create_index_buffer()
{
    VkDeviceSize buffer_size = sizeof( indices_[0] ) * indices_.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   staging_buffer, staging_buffer_memory );

    void* data;
    vkMapMemory( device_handle_, staging_buffer_memory, 0, buffer_size, 0, &data );
    memcpy( data, indices_.data(), static_cast<size_t>( buffer_size ) );
    vkUnmapMemory( device_handle_, staging_buffer_memory );

    create_buffer( buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   index_buffer_handle_, index_buffer_memory_handle_ );

    copy_buffer( staging_buffer, index_buffer_handle_, buffer_size );

    vkDestroyBuffer( device_handle_, staging_buffer, nullptr );
    vkFreeMemory( device_handle_, staging_buffer_memory, nullptr );
}
uint32_t
Renderer::find_memory_type( uint32_t type_filter, VkMemoryPropertyFlags properties )
{
    auto mem_properties = gpu_.get_physical_device_memory_properties();

    for( uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i )
    {
        if( ( type_filter & ( 1 << i ) ) && ( mem_properties.memoryTypes[i].propertyFlags & properties ) == properties )
        {
            return i;
        }
    }

    throw VulkanException{ "Failed to find a suitable memory type", __FILE__, __LINE__ };
}

VkSurfaceFormatKHR
Renderer::choose_surface_format( const std::vector<VkSurfaceFormatKHR> &available_formats ) const
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
Renderer::choose_present_mode( const std::vector<VkPresentModeKHR> &available_present_modes ) const
{
    for( const auto& available_present_mode : available_present_modes )
    {
        if( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
            return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D
Renderer::choose_extent( VkSurfaceCapabilitiesKHR &capabilities ) const
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
