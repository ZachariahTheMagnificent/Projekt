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

    instance_                   = vk::core::instance( window_.get_title(), validation_layers, extensions );

    if constexpr ( enable_validation_layers )
        debug_report_           = vk::core::debug_report( &instance_ );

    surface_                    = vk::graphics::surface( &instance_, window_ );
    gpu_                        = vk::core::physical_device( instance_, surface_ );
    logical_device_             = vk::core::logical_device( gpu_, validation_layers, device_extensions );
    graphics_queue_             = vk::core::queue( logical_device_, gpu_, vk::helpers::queue_family_type::e_graphics, 0 );
    present_queue_              = vk::core::queue( logical_device_, gpu_, vk::helpers::queue_family_type::e_present, 0 );
    command_pool_               = vk::core::command_pool( gpu_, &logical_device_, vk::helpers::queue_family_type::e_graphics );

    vertex_shader_              = vk::core::shader_module( &logical_device_, "../game/shaders/vert.spv" );
    fragment_shader_            = vk::core::shader_module( &logical_device_, "../game/shaders/frag.spv" );

    image_available_semaphores_ = vk::core::semaphores( &logical_device_, MAX_FRAMES_IN_FLIGHT );
    render_finished_semaphores_ = vk::core::semaphores( &logical_device_, MAX_FRAMES_IN_FLIGHT );
    fences_                     = vk::core::fences( &logical_device_, MAX_FRAMES_IN_FLIGHT );

    create_vertex_buffer();
    create_index_buffer();

    recreate_swapchain();
}

renderer::~renderer()
{
    graphics_queue_.wait_idle();

    vkDestroyBuffer( logical_device_.get(), index_buffer_handle_, nullptr );
    vkFreeMemory( logical_device_.get(), index_buffer_memory_handle_, nullptr );

    vkDestroyBuffer( logical_device_.get(), vertex_buffer_handle_, nullptr );
    vkFreeMemory( logical_device_.get(), vertex_buffer_memory_handle_, nullptr );
}

void
renderer::draw_frame( )
{
    fences_.wait_for_fence( current_frame, VK_TRUE, std::numeric_limits<uint64_t>::max() );
    fences_.reset_fence( current_frame );

    uint32_t image_index;
    auto result = vkAcquireNextImageKHR( logical_device_.get(), swapchain_.get(), std::numeric_limits<uint64_t>::max(),
                                         image_available_semaphores_[current_frame], VK_NULL_HANDLE, &image_index );

    if( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        recreate_swapchain();
    }
    else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    {
        throw vulkan_exception{ "Failed to acquire swapchain image", __FILE__, __LINE__ };
    }

    VkSemaphore wait_semaphores[] = { image_available_semaphores_[current_frame] };
    VkSemaphore signal_semaphores[] = { render_finished_semaphores_[current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers_[image_index];
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
    swapchain_ = vk::graphics::swapchain( &logical_device_, gpu_, surface_ );
    render_pass_ = vk::core::render_pass( &logical_device_, swapchain_ );

    graphics_pipeline_ = vk::graphics::graphics_pipeline( &logical_device_, render_pass_, swapchain_, vertex_shader_, fragment_shader_ );

    frame_buffers_ = vk::graphics::frame_buffers( &logical_device_, render_pass_, swapchain_, swapchain_.get_count() );
    command_buffers_ = vk::core::command_buffers( &command_pool_, frame_buffers_.get_count() );

    record_commands();
}

void
renderer::record_commands( )
{
    for( auto i = 0; i < command_buffers_.get_count(); ++i )
    {
        command_buffers_.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, i );

        {
            VkClearValue clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };

            VkRenderPassBeginInfo render_pass_begin_info = {};
            render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_begin_info.renderPass = render_pass_.get();
            render_pass_begin_info.framebuffer = frame_buffers_[i];
            render_pass_begin_info.renderArea.offset = { 0, 0 };
            render_pass_begin_info.renderArea.extent = swapchain_.get_extent();
            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.pClearValues = &clear_colour;

            command_buffers_.begin_render_pass( render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE, i );

            {
                command_buffers_.bind_pipeline( VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_.get(), i );


                VkBuffer vertex_buffers[] = { vertex_buffer_handle_ };
                VkDeviceSize offsets[] = { 0 };

                command_buffers_.bind_vertex_buffers( 0, 1, vertex_buffers, offsets, i );
                command_buffers_.bind_index_buffer( index_buffer_handle_, 0, VK_INDEX_TYPE_UINT16, i );

                command_buffers_.draw_indexed( static_cast<uint32_t>( indices_.size() ), 1, 0, 0, 0, i );
            }

            command_buffers_.end_render_pass( i );
        }

        command_buffers_.end( i );
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

    command_pool_.free_command_buffers( &command_buffer, 1 );
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