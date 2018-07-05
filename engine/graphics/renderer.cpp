/*!
 *
 */

#include <cstring>
#include <iostream>
#include <set>
#include <thread>

#include "renderer.h"

#include "../vulkan/helpers/queue_family_indices.h"

#include "../utils/exception/vulkan_exception.h"
#include "../utils/file_io/read.h"

constexpr const int MAX_FRAMES_IN_FLIGHT = 2;

renderer::renderer( const window &window )
    :
    window_( window )
{
    instance_                   = vk::core::instance( window_.get_title(), validation_layers, window_.get_required_extensions() );

    if constexpr ( enable_validation_layers )
        debug_report_           = vk::core::debug_report( &instance_ );

    surface_                    = vk::graphics::surface( &instance_, window_ );
    gpu_                        = vk::core::physical_device( instance_, surface_ );
    logical_device_             = vk::core::logical_device( gpu_, validation_layers, device_extensions );
    graphics_queue_             = vk::core::queue( logical_device_, gpu_, vk::helpers::queue_family_type::e_graphics, 0 );
    present_queue_              = vk::core::queue( logical_device_, gpu_, vk::helpers::queue_family_type::e_present, 0 );
    command_pool_               = vk::core::command_pool( gpu_, &logical_device_, vk::helpers::queue_family_type::e_graphics );

    image_available_semaphores_ = vk::core::semaphores( &logical_device_, MAX_FRAMES_IN_FLIGHT );
    render_finished_semaphores_ = vk::core::semaphores( &logical_device_, MAX_FRAMES_IN_FLIGHT );
    fences_                     = vk::core::fences( &logical_device_, MAX_FRAMES_IN_FLIGHT );

    swapchain_                  = vk::graphics::swapchain( &logical_device_, gpu_, surface_, window_.get_width(), window_.get_height(), swapchain_.get() );
    render_pass_                = vk::core::render_pass( &logical_device_, swapchain_ );

    vertex_shader_              = vk::core::shader_module( &logical_device_, "../game/shaders/vert.spv" );
    fragment_shader_            = vk::core::shader_module( &logical_device_, "../game/shaders/frag.spv" );
    graphics_pipeline_          = vk::graphics::graphics_pipeline( &logical_device_, render_pass_, swapchain_, vertex_shader_, fragment_shader_ );

    frame_buffers_              = vk::graphics::frame_buffers( &logical_device_, render_pass_, swapchain_, swapchain_.get_count() );
    command_buffers_            = vk::core::command_buffers( &command_pool_, frame_buffers_.get_count() );
}

renderer::~renderer()
{
    graphics_queue_.wait_idle();
}

void renderer::prepare_for_rendering( const std::vector<vk::graphics::vertex>& vertices, const std::vector<std::uint16_t>& indices )
{
    create_vertex_buffer( vertices );
    create_index_buffer( indices );

    record_commands( );
}

void
renderer::recreate_swapchain( )
{
    logical_device_.wait_idle( );

    swapchain_ = vk::graphics::swapchain( &logical_device_, gpu_, surface_, window_.get_width(), window_.get_height(), swapchain_.get() );
    render_pass_ = vk::core::render_pass( &logical_device_, swapchain_ );

    graphics_pipeline_ = vk::graphics::graphics_pipeline( &logical_device_, render_pass_, swapchain_, vertex_shader_, fragment_shader_ );

    frame_buffers_ = vk::graphics::frame_buffers( &logical_device_, render_pass_, swapchain_, swapchain_.get_count() );
    command_buffers_ = vk::core::command_buffers( &command_pool_, frame_buffers_.get_count() );

    record_commands( );
}

void
renderer::record_commands( )
{
    VkViewport viewport = { 0, 0, static_cast<uint32_t>( window_.get_width() ), static_cast<uint32_t>( window_.get_height() ), 0, 0 };
    VkRect2D scissor = { { 0, 0 }, swapchain_.get_extent() };


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
                command_buffers_.set_viewport( 0, 1, &viewport, i );
                command_buffers_.set_scissor( 0, 1, &scissor, i );

                command_buffers_.bind_pipeline( VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_.get(), i );

                VkDeviceSize offsets[] = { 0 };

                command_buffers_.bind_vertex_buffers( 0, 1, &vertex_buffer_.get(), offsets, i );
                command_buffers_.bind_index_buffer( index_buffer_.get(), 0, VK_INDEX_TYPE_UINT16, i );

                command_buffers_.draw_indexed( index_buffer_.get_count(), 1, 0, 0, 0, i );
            }

            command_buffers_.end_render_pass( i );
        }

        command_buffers_.end( i );
    }
}

void
renderer::prepare_frame( std::vector<event>& events )
{
    fences_.wait_for_fence( current_frame_, VK_TRUE, std::numeric_limits<uint64_t>::max() );
    fences_.reset_fence( current_frame_ );

    auto result = vkAcquireNextImageKHR( logical_device_.get(), swapchain_.get(), std::numeric_limits<uint64_t>::max(),
                                         image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index_ );

    if( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        bool is_resized = false;

        if( !events.empty() )
        {
            for( auto& e : events )
            {
                if( e.event_type == event::type::window_resized )
                {
                    is_resized = true;
                }
            }
        }

        if ( is_resized )
        {
            handle_window_resizing();
        }
        else
        {
            std::cerr << "graphics pipeline recreated." << std::endl;

            recreate_swapchain( );
        }
    }
    else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    {
        throw vulkan_exception{ "Failed to acquire swapchain image", __FILE__, __LINE__ };
    }
}

void
renderer::submit_frame( std::vector<event>& events )
{
    VkSemaphore wait_semaphores[] = { image_available_semaphores_[current_frame_] };
    VkSemaphore signal_semaphores[] = { render_finished_semaphores_[current_frame_] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers_[image_index_];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    graphics_queue_.submit( submit_info, fences_[current_frame_] );

    VkSwapchainKHR swapchains[] = { swapchain_.get() };
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index_;

    auto result = present_queue_.present( present_info );

    if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
    {
        bool is_resized = false;

        for( auto& e : events )
        {
            if( e.event_type == event::type::window_resized )
            {
                is_resized = true;
            }
        }

        if ( is_resized )
        {
            handle_window_resizing();
        }
        else
        {
            std::cerr << "graphics pipeline recreated." << std::endl;

            recreate_swapchain( );
        }
    }
    else if( result != VK_SUCCESS )
    {
        throw vulkan_exception{ "Failed to present swapchain image", __FILE__, __LINE__ };
    }

    current_frame_ = ( current_frame_ + 1 ) % MAX_FRAMES_IN_FLIGHT;
}

void
renderer::create_vertex_buffer( const std::vector<vk::graphics::vertex>& vertices )
{
    vertex_buffer_ = vk::core::vertex_buffer( &logical_device_, gpu_, command_pool_, graphics_queue_, vertices );
}
void
renderer::create_index_buffer( const std::vector<std::uint16_t>& indices )
{
    index_buffer_ = vk::core::index_buffer( &logical_device_, gpu_, command_pool_, graphics_queue_, indices );
}

void renderer::handle_window_resizing( )
{
    logical_device_.wait_idle( );

    swapchain_.destroy( );

    surface_ = vk::graphics::surface( &instance_, window_ );

    gpu_.check_surface_present_support( surface_ );

    swapchain_ = vk::graphics::swapchain( &logical_device_, gpu_, surface_, window_.get_width( ), window_.get_height( ), swapchain_.get( ) );
    frame_buffers_ = vk::graphics::frame_buffers( &logical_device_, render_pass_, swapchain_, swapchain_.get_count( ) );
    command_buffers_ = vk::core::command_buffers( &command_pool_, frame_buffers_.get_count( ) );

    record_commands( );
}
