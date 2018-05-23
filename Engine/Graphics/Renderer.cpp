/*!
 *
 */

#include <iostream>
#include <set>

#include "Renderer.h"

#include "../Utils/Exception/VulkanException.h"
#include "../Utils/File/Read.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64_t obj, size_t location,
                                                        int32_t code, const char* layerPrefix,
                                                        const char* msg, void* userData )
{
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT ( VkInstance instance,
                                                                const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator,
                                                                VkDebugReportCallbackEXT* pCallback )
{
    static auto func = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr ( instance, "vkCreateDebugReportCallbackEXT" );

    if ( func != nullptr )
    {
        return func ( instance, pCreateInfo, pAllocator, pCallback );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT ( VkInstance instance,
                                                             VkDebugReportCallbackEXT callback,
                                                             const VkAllocationCallbacks* pAllocator )
{
    static auto func = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr ( instance, "vkDestroyDebugReportCallbackEXT" );

    if ( func != nullptr )
    {
        func ( instance, callback, pAllocator );
    }
}

Renderer::Renderer( Window &window )
    :
    window_( window )
{
    instance_handle_                = create_instance();

    if( enable_validation_layers )
        debug_callback_handle_      = create_debug_callback();

    surface_handle_                 = create_surface();
    gpu_handle_                     = pick_gpu();
    device_handle_                  = create_device();
    graphics_queue_handle_          = get_queue( indices_.graphics_family,  0 );
    present_queue_handle_           = get_queue( indices_.present_family, 0 );
    command_pool_handle_            = create_command_pool();

    recreate_swapchain();

    image_available_semaphore_handle_ = create_semaphore();
    render_finished_semaphore_handle_ = create_semaphore();
}

Renderer::~Renderer()
{
    device_handle_.destroySemaphore( render_finished_semaphore_handle_ );
    device_handle_.destroySemaphore( image_available_semaphore_handle_ );

    for( auto framebuffer : swapchain_framebuffer_handles_ )
    {
        device_handle_.destroyFramebuffer( framebuffer );
    }

    device_handle_.destroyPipeline( pso_handle_ );
    device_handle_.destroyPipelineLayout( pso_layout_handle_ );
    device_handle_.destroyRenderPass( render_pass_handle_ );

    for( auto image_view : swapchain_image_view_handles_ )
    {
        device_handle_.destroyImageView( image_view );
    }

    device_handle_.destroySwapchainKHR( swapchain_handle_ );

    device_handle_.destroyCommandPool( command_pool_handle_ );
    device_handle_.destroy();

    instance_handle_.destroySurfaceKHR( surface_handle_ );

    if( enable_validation_layers )
        instance_handle_.destroyDebugReportCallbackEXT( debug_callback_handle_ );

    instance_handle_.destroy();
}

void
Renderer::draw_frame( )
{
    uint32_t image_index;
    auto result = device_handle_.acquireNextImageKHR( swapchain_handle_, std::numeric_limits<uint64_t>::max(),
                                                      image_available_semaphore_handle_, nullptr, &image_index );

    if( result == vk::Result::eErrorOutOfDateKHR )
    {
        recreate_swapchain();
    }
    else if( result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR )
    {
        throw VulkanException{ "Failed to acquire swapchain image", __FILE__, __LINE__ };
    }

    vk::Semaphore wait_semaphores[] = { image_available_semaphore_handle_ };
    vk::Semaphore signal_semaphores[] = { render_finished_semaphore_handle_ };
    vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submit_info;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_handles[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    graphics_queue_handle_.submit( submit_info, nullptr );

    vk::SwapchainKHR swapchains[] = { swapchain_handle_ };
    vk::PresentInfoKHR present_info;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    result = present_queue_handle_.presentKHR( present_info );

    if( result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR )
    {
        recreate_swapchain();
    }
    else if( result != vk::Result::eSuccess )
    {
        throw VulkanException{ "Failed to present swapchain image", __FILE__, __LINE__ };
    }

    if( enable_validation_layers )
        graphics_queue_handle_.waitIdle( );
}

void
Renderer::recreate_swapchain( )
{
    swapchain_handle_               = create_swapchain();
    swapchain_image_view_handles_   = create_image_views();
    render_pass_handle_             = create_render_pass();

    create_pso();

    swapchain_framebuffer_handles_  = create_framebuffers();
    command_buffer_handles          = create_command_buffers();

    record_commands();
}

vk::Instance
Renderer::create_instance( ) const
{
    auto extensions = window_.get_required_extensions();

    if( enable_validation_layers && !check_validation_layer_support() )
    {
        throw VulkanException{ "Validation layers requested, but not available", __FILE__, __LINE__ };
    }

    vk::ApplicationInfo app_info = {};
    app_info.pApplicationName = window_.get_title().c_str();
    app_info.applicationVersion = VK_MAKE_VERSION( 0, 0, 1 );
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION( 0, 0, 1 );
    app_info.apiVersion = VK_API_VERSION_1_1;

    vk::InstanceCreateInfo create_info = {};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
    create_info.ppEnabledExtensionNames = extensions.data();

    if( enable_validation_layers )
    {
        create_info.enabledLayerCount = static_cast<uint32_t>( validation_layers.size() );
        create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    return vk::createInstance( create_info );
}
vk::DebugReportCallbackEXT
Renderer::create_debug_callback( ) const
{
    vk::DebugReportCallbackCreateInfoEXT create_info = { };
    create_info.flags = vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError;
    create_info.pfnCallback = debug_callback_function;

    return instance_handle_.createDebugReportCallbackEXT( create_info );
}
vk::SurfaceKHR
Renderer::create_surface() const
{
    return vk::SurfaceKHR( window_.create_surface( instance_handle_ ) );
}
vk::PhysicalDevice
Renderer::pick_gpu( )
{
    vk::PhysicalDevice gpu_handle;

    auto available_devices = instance_handle_.enumeratePhysicalDevices();

    for( auto& device : available_devices )
    {
        if( is_device_suitable( device ) )
        {
            gpu_handle = device;
            break;
        }
    }

    return gpu_handle;
}
vk::Device
Renderer::create_device( ) const
{
    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    std::set<int> unique_queue_families = { indices_.graphics_family, indices_.present_family };

    float queue_priority = 1.0f;
    for( const auto& queue_family : unique_queue_families )
    {
        vk::DeviceQueueCreateInfo create_info = {};
        create_info.queueFamilyIndex = queue_family;
        create_info.queueCount = 1;
        create_info.pQueuePriorities = &queue_priority;

        queue_infos.emplace_back( create_info );
    }

    vk::PhysicalDeviceFeatures gpu_features = {};

    vk::DeviceCreateInfo create_info = {};
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

    return gpu_handle_.createDevice( create_info );
}
vk::Queue
Renderer::get_queue( int32_t family_index, uint32_t queue_index ) const
{
    return device_handle_.getQueue( static_cast<uint32_t>( family_index ), queue_index );
}
vk::CommandPool
Renderer::create_command_pool( ) const
{
    vk::CommandPoolCreateInfo create_info;
    create_info.queueFamilyIndex = static_cast<uint32_t>( indices_.graphics_family );

    return device_handle_.createCommandPool( create_info );
}
vk::Semaphore
Renderer::create_semaphore( ) const
{
    vk::SemaphoreCreateInfo create_info;

    return device_handle_.createSemaphore( create_info );
}

vk::SwapchainKHR
Renderer::create_swapchain( )
{
    auto support_details = query_swapchain_support( gpu_handle_ );

    auto surface_format = choose_surface_format( support_details.formats );
    auto present_mode = choose_present_mode( support_details.present_modes );
    auto extent = choose_extent( support_details.capabilities );

    uint32_t image_count = support_details.capabilities.minImageCount + 1;
    if( support_details.capabilities.maxImageCount > 0 &&
        image_count > support_details.capabilities.maxImageCount )
    {
        image_count = support_details.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR create_info;
    create_info.surface = surface_handle_;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queue_family_indices[] =
    {
        static_cast<uint32_t>( indices_.graphics_family ),
        static_cast<uint32_t>( indices_.present_family )
    };

    if( indices_.graphics_family != indices_.present_family )
    {
        create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = vk::SharingMode::eExclusive;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = support_details.capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    swapchain_format_handle_ = surface_format.format;
    swapchain_extent_handle_ = extent;

    auto old_swapchain_handle_ = swapchain_handle_;
    create_info.oldSwapchain = old_swapchain_handle_;

    auto new_swapchain = device_handle_.createSwapchainKHR( create_info );

    swapchain_images_ = device_handle_.getSwapchainImagesKHR( new_swapchain );

    return new_swapchain;
}
vk::SwapchainKHR
Renderer::create_swapchain( vk::SwapchainKHR& old_swapchain )
{
    auto support_details = query_swapchain_support( gpu_handle_ );

    auto surface_format = choose_surface_format( support_details.formats );
    auto present_mode = choose_present_mode( support_details.present_modes );
    auto extent = choose_extent( support_details.capabilities );

    uint32_t image_count = support_details.capabilities.minImageCount + 1;
    if( support_details.capabilities.maxImageCount > 0 &&
        image_count > support_details.capabilities.maxImageCount )
    {
        image_count = support_details.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR create_info;
    create_info.surface = surface_handle_;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queue_family_indices[] =
            {
                    static_cast<uint32_t>( indices_.graphics_family ),
                    static_cast<uint32_t>( indices_.present_family )
            };

    if( indices_.graphics_family != indices_.present_family )
    {
        create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = vk::SharingMode::eExclusive;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = support_details.capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = old_swapchain;

    swapchain_format_handle_ = surface_format.format;
    swapchain_extent_handle_ = extent;

    auto new_swapchain = device_handle_.createSwapchainKHR( create_info );

    swapchain_images_ = device_handle_.getSwapchainImagesKHR( swapchain_handle_ );

    return new_swapchain;
}
std::vector<vk::ImageView>
Renderer::create_image_views( )
{
    std::vector<vk::ImageView> image_views( swapchain_images_.size() );

    for( auto i = 0; i < swapchain_images_.size(); ++i )
    {
        vk::ImageViewCreateInfo create_info;
        create_info.image = swapchain_images_[i];
        create_info.format = swapchain_format_handle_;
        create_info.viewType = vk::ImageViewType::e2D;
        create_info.components.r = vk::ComponentSwizzle::eIdentity;
        create_info.components.g = vk::ComponentSwizzle::eIdentity;
        create_info.components.b = vk::ComponentSwizzle::eIdentity;
        create_info.components.a = vk::ComponentSwizzle::eIdentity;
        create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        image_views[i] = device_handle_.createImageView( create_info );
    }

    return image_views;
}
std::vector<vk::Framebuffer>
Renderer::create_framebuffers( )
{
    std::vector<vk::Framebuffer> framebuffers( swapchain_image_view_handles_.size() );

    for( auto i = 0; i < swapchain_image_view_handles_.size(); ++i )
    {
        vk::ImageView attachments[] =
        {
            swapchain_image_view_handles_[i]
        };

        vk::FramebufferCreateInfo create_info;
        create_info.renderPass = render_pass_handle_;
        create_info.attachmentCount = 1;
        create_info.pAttachments = attachments;
        create_info.width = swapchain_extent_handle_.width;
        create_info.height = swapchain_extent_handle_.height;
        create_info.layers = 1;

        framebuffers[i] = device_handle_.createFramebuffer( create_info );
    }

    return framebuffers;
}
std::vector<vk::CommandBuffer>
Renderer::create_command_buffers( )
{
    vk::CommandBufferAllocateInfo allocate_info;
    allocate_info.commandPool = command_pool_handle_;
    allocate_info.level = vk::CommandBufferLevel::ePrimary;
    allocate_info.commandBufferCount = static_cast<uint32_t>( swapchain_framebuffer_handles_.size() );

    return device_handle_.allocateCommandBuffers( allocate_info );
}

vk::RenderPass
Renderer::create_render_pass( )
{
    vk::AttachmentDescription colour_attachment;
    colour_attachment.format = swapchain_format_handle_;
    colour_attachment.samples = vk::SampleCountFlagBits::e1;
    colour_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    colour_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    colour_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colour_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colour_attachment.initialLayout = vk::ImageLayout::eUndefined;
    colour_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colour_attachment_reference;
    colour_attachment_reference.attachment = 0;
    colour_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass_description;
    subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &colour_attachment_reference;

    vk::SubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo create_info;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &colour_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass_description;
    create_info.dependencyCount = 1;
    create_info.pDependencies = &dependency;

    return device_handle_.createRenderPass( create_info );
}
void
Renderer::create_pso( )
{
    // Shaders
    auto vert_shader_code = read_from_binary_file( "../Game/Shaders/vert.spv" );
    auto frag_shader_code = read_from_binary_file( "../Game/Shaders/frag.spv" );

    vk::ShaderModule vert_shader_module = create_shader_module( vert_shader_code );
    vk::ShaderModule frag_shader_module = create_shader_module( frag_shader_code );

    vk::PipelineShaderStageCreateInfo vert_shader_stage_info;
    vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    vk::PipelineShaderStageCreateInfo frag_shader_stage_info;
    frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
    frag_shader_stage_info.module = frag_shader_module;
    frag_shader_stage_info.pName = "main";

    vk::PipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

    vk::PipelineVertexInputStateCreateInfo vertex_input_state_info;
    vertex_input_state_info.vertexBindingDescriptionCount = 0;
    vertex_input_state_info.pVertexBindingDescriptions = nullptr;
    vertex_input_state_info.vertexAttributeDescriptionCount = 0;
    vertex_input_state_info.pVertexAttributeDescriptions = nullptr;

    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_info;
    input_assembly_state_info.topology = vk::PrimitiveTopology::eTriangleList;
    input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>( swapchain_extent_handle_.width );
    viewport.height = static_cast<float>( swapchain_extent_handle_.height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D( 0, 0 );
    scissor.extent = swapchain_extent_handle_;

    vk::PipelineViewportStateCreateInfo viewport_state_info;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = &viewport;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterization_state_info;
    rasterization_state_info.depthClampEnable = VK_FALSE;
    rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_info.polygonMode = vk::PolygonMode::eFill;
    rasterization_state_info.lineWidth = 1.0f;
    rasterization_state_info.cullMode = vk::CullModeFlagBits::eBack;
    rasterization_state_info.frontFace = vk::FrontFace::eClockwise;
    rasterization_state_info.depthBiasEnable = VK_FALSE;
    rasterization_state_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_info.depthBiasClamp = 0.0f;
    rasterization_state_info.depthBiasSlopeFactor = 0.0f;

    vk::PipelineMultisampleStateCreateInfo multisample_state_info;
    multisample_state_info.sampleShadingEnable = VK_FALSE;
    multisample_state_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisample_state_info.minSampleShading = 1.0f;
    multisample_state_info.pSampleMask = nullptr;
    multisample_state_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_info.alphaToOneEnable = VK_FALSE;

    vk::PipelineColorBlendAttachmentState colour_blend_attachment_state;
    colour_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR
                                                   | vk::ColorComponentFlagBits::eG
                                                   | vk::ColorComponentFlagBits::eB
                                                   | vk::ColorComponentFlagBits::eA;
    colour_blend_attachment_state.blendEnable = VK_FALSE;
    colour_blend_attachment_state.srcColorBlendFactor = vk::BlendFactor::eOne;
    colour_blend_attachment_state.dstColorBlendFactor = vk::BlendFactor::eZero;
    colour_blend_attachment_state.colorBlendOp = vk::BlendOp::eAdd;
    colour_blend_attachment_state.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colour_blend_attachment_state.dstAlphaBlendFactor = vk::BlendFactor ::eZero;
    colour_blend_attachment_state.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colour_blend_state_info;
    colour_blend_state_info.logicOpEnable = VK_FALSE;
    colour_blend_state_info.logicOp = vk::LogicOp::eCopy;
    colour_blend_state_info.attachmentCount = 1;
    colour_blend_state_info.pAttachments = &colour_blend_attachment_state;
    colour_blend_state_info.blendConstants[0] = 0.0f;
    colour_blend_state_info.blendConstants[1] = 0.0f;
    colour_blend_state_info.blendConstants[2] = 0.0f;
    colour_blend_state_info.blendConstants[3] = 0.0f;

    vk::DynamicState dynamic_state[] =
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    dynamic_state_info.dynamicStateCount = 2;
    dynamic_state_info.pDynamicStates = dynamic_state;

    vk::PipelineLayoutCreateInfo pipeline_layout_info;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    pso_layout_handle_ = device_handle_.createPipelineLayout( pipeline_layout_info );

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.stageCount = 1;
    create_info.pStages = shader_stages;
    create_info.pVertexInputState = &vertex_input_state_info;
    create_info.pInputAssemblyState = &input_assembly_state_info;
    create_info.pViewportState = &viewport_state_info;
    create_info.pRasterizationState = &rasterization_state_info;
    create_info.pMultisampleState = &multisample_state_info;
    create_info.pDepthStencilState = nullptr;
    create_info.pColorBlendState = &colour_blend_state_info;
    create_info.pDynamicState = &dynamic_state_info;
    create_info.layout = pso_layout_handle_;
    create_info.renderPass = render_pass_handle_;
    create_info.subpass = 0;

    pso_handle_ = device_handle_.createGraphicsPipeline( nullptr, create_info );

    device_handle_.destroyShaderModule( frag_shader_module );
    device_handle_.destroyShaderModule( vert_shader_module );
}
vk::ShaderModule
Renderer::create_shader_module( const std::string &shader_code )
{
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = shader_code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>( shader_code.data() );

    return device_handle_.createShaderModule( createInfo );
}

void
Renderer::record_commands( )
{
    for( auto i = 0; i < command_buffer_handles.size(); ++i )
    {
        vk::CommandBufferBeginInfo command_buffer_begin_info;
        command_buffer_begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        command_buffer_begin_info.pInheritanceInfo = nullptr;

        command_buffer_handles[i].begin( command_buffer_begin_info );

            vk::RenderPassBeginInfo render_pass_begin_info;
            render_pass_begin_info.renderPass = render_pass_handle_;
            render_pass_begin_info.framebuffer = swapchain_framebuffer_handles_[i];
            render_pass_begin_info.renderArea.offset = vk::Offset2D( 0, 0 );
            render_pass_begin_info.renderArea.extent = swapchain_extent_handle_;

            vk::ClearValue clear_colour;
            clear_colour.color.float32[0] = 0.0f;
            clear_colour.color.float32[1] = 0.0f;
            clear_colour.color.float32[2] = 0.0f;
            clear_colour.color.float32[3] = 1.0f;

            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.pClearValues = &clear_colour;

            command_buffer_handles[i].beginRenderPass( render_pass_begin_info, vk::SubpassContents::eInline );

                command_buffer_handles[i].bindPipeline( vk::PipelineBindPoint::eGraphics, pso_handle_ );

                command_buffer_handles[i].draw( 3, 1, 0, 0 );

            command_buffer_handles[i].endRenderPass();

        command_buffer_handles[i].end();
    }
}

bool
Renderer::check_validation_layer_support( ) const
{
    auto available_layers = vk::enumerateInstanceLayerProperties();

    for( const char* layer_name : validation_layers )
    {
        bool layer_found = false;

        for( const auto& layer_property : available_layers )
        {
            if( strcmp( layer_name, layer_property.layerName ) == 0 )
            {
                layer_found = true;
                break;
            }
        }

        if( !layer_found )
            return false;
    }

    return true;
}
bool
Renderer::check_device_extension_support( vk::PhysicalDevice &gpu_handle ) const
{
    auto available_extensions = gpu_handle.enumerateDeviceExtensionProperties( );

    std::set<std::string> required_extensions( device_extensions.begin(), device_extensions.end() );

    for( const auto& extension : available_extensions )
    {
        required_extensions.erase( extension.extensionName );
    }

    return required_extensions.empty();
}
bool
Renderer::is_device_suitable( vk::PhysicalDevice &gpu_handle )
{
    indices_ = find_queue_family_indices( gpu_handle );

    bool extension_supported = check_device_extension_support( gpu_handle );
    bool swapchain_adequate = false;

    if( extension_supported )
    {
        SwapchainSupportDetails details = query_swapchain_support( gpu_handle );
        swapchain_adequate = !details.formats.empty() && !details.present_modes.empty();
    }

    return indices_.is_complete() && swapchain_adequate && extension_supported;
}

Renderer::QueueFamilyIndices
Renderer::find_queue_family_indices( vk::PhysicalDevice &gpu_handle ) const
{
    QueueFamilyIndices indices;

    auto queue_family_properties = gpu_handle.getQueueFamilyProperties();

    int i = 0;
    for( const auto& queue_family_property : queue_family_properties )
    {
        if( queue_family_property.queueCount > 0 && queue_family_property.queueFlags & vk::QueueFlagBits::eGraphics )
            indices.graphics_family = i;

        auto present_support = gpu_handle.getSurfaceSupportKHR( i, surface_handle_ );

        if( queue_family_property.queueCount > 0 && present_support )
            indices.present_family = i;

        ++i;
    }

    return indices;
}

Renderer::SwapchainSupportDetails
Renderer::query_swapchain_support( vk::PhysicalDevice &gpu_handle ) const
{
    SwapchainSupportDetails details;

    details.capabilities = gpu_handle.getSurfaceCapabilitiesKHR( surface_handle_ );
    details.formats = gpu_handle.getSurfaceFormatsKHR( surface_handle_ );
    details.present_modes = gpu_handle.getSurfacePresentModesKHR( surface_handle_ );

    return details;
}

vk::SurfaceFormatKHR
Renderer::choose_surface_format( const std::vector<vk::SurfaceFormatKHR> &available_formats ) const
{
    if( available_formats.size() == 1 && available_formats[0].format == vk::Format::eUndefined )
        return { vk::Format::eB8G8R8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

    for( const auto& available_format : available_formats )
    {
        if( available_format.format == vk::Format::eB8G8R8Unorm && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
        {
            return available_format;
        }
    }

    return available_formats[0];
}
vk::PresentModeKHR
Renderer::choose_present_mode( const std::vector<vk::PresentModeKHR> &available_present_modes ) const
{
    for( const auto& available_present_mode : available_present_modes )
    {
        if( available_present_mode == vk::PresentModeKHR::eMailbox )
            return vk::PresentModeKHR::eMailbox;
    }

    return vk::PresentModeKHR::eFifo;
}
vk::Extent2D
Renderer::choose_extent( vk::SurfaceCapabilitiesKHR &capabilities ) const
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
