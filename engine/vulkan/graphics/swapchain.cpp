/*!
 *
 */

#include "swapchain.h"

namespace vk
{
    namespace graphics
    {
        swapchain::swapchain( const core::logical_device* p_logical_device, const core::physical_device& physical_device,
                              const surface& surface, uint32_t width, uint32_t height, VkSwapchainKHR old_swapchain_handle )
            :
            p_logical_device_( p_logical_device )
        {
            auto support_details = physical_device.query_swapchain_support( surface );

            auto surface_format = choose_surface_format( support_details.formats );
            auto present_mode = choose_present_mode( support_details.present_modes );
            auto extent = choose_extent_2d( support_details.capabilities, width, height );

            uint32_t image_count = support_details.capabilities.minImageCount + 1;
            if( support_details.capabilities.maxImageCount > 0 &&
                image_count > support_details.capabilities.maxImageCount )
            {
                image_count = support_details.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            create_info.surface = surface.get();
            create_info.minImageCount = image_count;
            create_info.imageFormat = surface_format.format;
            create_info.imageColorSpace = surface_format.colorSpace;
            create_info.imageExtent = extent;
            create_info.imageArrayLayers = 1;
            create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            auto queue_family_indices_ = physical_device.get_queue_family_indices();

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

            format_ = surface_format.format;
            extent_ = extent;

            create_info.oldSwapchain = old_swapchain_handle;

            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = format_;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            swapchain_handle_   = p_logical_device_->create_swapchain( create_info );
            image_handles_      = p_logical_device_->create_images( swapchain_handle_, image_count );
            image_view_handles_ = p_logical_device_->create_image_views( image_handles_, image_view_create_info, image_count );
            image_count_        = image_count;
        }
        swapchain::~swapchain( )
        {
            if( image_view_handles_ != VK_NULL_HANDLE )
                image_view_handles_ = p_logical_device_->destroy_image_views( image_view_handles_, image_count_ );

            if( image_handles_ != VK_NULL_HANDLE )
                image_handles_ = p_logical_device_->destroy_images( image_handles_ );

            if( swapchain_handle_ != VK_NULL_HANDLE )
                swapchain_handle_ = p_logical_device_->destroy_swapchain( swapchain_handle_ );
        }
        swapchain::swapchain( swapchain&& swapchain ) noexcept
        {
            *this = std::move( swapchain );
        }

        void
        swapchain::destroy( )
        {
            if( image_view_handles_ != VK_NULL_HANDLE )
                image_view_handles_ = p_logical_device_->destroy_image_views( image_view_handles_, image_count_ );

            if( image_handles_ != VK_NULL_HANDLE )
                image_handles_ = p_logical_device_->destroy_images( image_handles_ );

            if( swapchain_handle_ != VK_NULL_HANDLE )
                swapchain_handle_ = p_logical_device_->destroy_swapchain( swapchain_handle_ );
        }

        VkResult
        swapchain::acquire_next_image( uint64_t timeout, VkSemaphore& semaphore_handle, VkFence fence_handle,
                                       uint32_t* p_image_index )
        {
            return p_logical_device_->acquire_next_image( swapchain_handle_, timeout, semaphore_handle, fence_handle, p_image_index );
        }

        swapchain&
        swapchain::operator=( swapchain&& swapchain ) noexcept
        {
            if( this != &swapchain )
            {
                if( swapchain_handle_ != VK_NULL_HANDLE )
                    swapchain_handle_ = p_logical_device_->destroy_swapchain( swapchain_handle_ );

                if( image_view_handles_ != VK_NULL_HANDLE )
                    image_view_handles_ = p_logical_device_->destroy_image_views( image_view_handles_, image_count_ );

                swapchain_handle_ = swapchain.swapchain_handle_;
                swapchain.swapchain_handle_ = VK_NULL_HANDLE;

                image_handles_ = swapchain.image_handles_;
                swapchain.image_handles_ = VK_NULL_HANDLE;

                image_view_handles_ = swapchain.image_view_handles_;
                swapchain.image_view_handles_ = VK_NULL_HANDLE;

                image_count_ = swapchain.image_count_;
                swapchain.image_count_ = 0;

                format_ = swapchain.format_;
                extent_ = swapchain.extent_;

                p_logical_device_ = swapchain.p_logical_device_;
            }

            return *this;
        }

        VkExtent2D
        swapchain::choose_extent_2d( VkSurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height ) const
        {
            if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
            {
                return capabilities.currentExtent;
            }
            else
            {
                VkExtent2D actual_extent = { width, height };

                actual_extent.width = std::max( capabilities.minImageExtent.width,
                                                std::min( capabilities.minImageExtent.width, actual_extent.width ) );

                actual_extent.height = std::max( capabilities.minImageExtent.height,
                                                 std::min( capabilities.minImageExtent.height, actual_extent.height ) );

                return actual_extent;
            }
        }
        VkSurfaceFormatKHR
        swapchain::choose_surface_format( std::vector<VkSurfaceFormatKHR>& available_formats ) const
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
        swapchain::choose_present_mode( std::vector<VkPresentModeKHR>& available_present_modes ) const
        {
            for( const auto& available_present_mode : available_present_modes )
            {

                if( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
                    return VK_PRESENT_MODE_MAILBOX_KHR;
                else if( available_present_mode == VK_PRESENT_MODE_FIFO_KHR )
                    return VK_PRESENT_MODE_FIFO_KHR;
                else if( available_present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR )
                    return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            }

            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
}