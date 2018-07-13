/*!
 *
 */

#ifndef PROJEKT_SWAPCHAIN_H
#define PROJEKT_SWAPCHAIN_H

#include <vulkan/vulkan.h>

#include "../core/physical_device.h"
#include "../core/logical_device.h"

namespace vk
{
    namespace graphics
    {
        class swapchain
        {
        public:
            swapchain() = default;
            swapchain( const core::logical_device* p_logical_device, const core::physical_device& physical_device,
                       const surface& surface, uint32_t width, uint32_t height, VkSwapchainKHR old_swapchain_handle = VK_NULL_HANDLE );
            swapchain( const swapchain& swapchain ) = delete;
            swapchain( swapchain&& swapchain ) noexcept;
            ~swapchain( );

            void destroy();

            VkSwapchainKHR& get()
            {
                return swapchain_handle_;
            }

            VkImageView& get_image_view( uint32_t i )
            {
                return image_view_handles_[i];
            }

            const VkImageView* get_image_views() const
            {
                return image_view_handles_;
            }

            uint32_t get_count()
            {
                return image_count_;
            }

            const VkFormat& get_format() const
            {
                return format_;
            }
            const VkExtent2D& get_extent() const
            {
                return extent_;
            }

            VkResult acquire_next_image( uint64_t timeout, VkSemaphore& semaphore_handle, VkFence fence_handle, uint32_t* p_image_index );

            swapchain& operator=( const swapchain& swapchain ) = delete;
            swapchain& operator=( swapchain&& swapchain ) noexcept;

        private:
            VkExtent2D choose_extent_2d( VkSurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height ) const;
            VkSurfaceFormatKHR choose_surface_format( std::vector<VkSurfaceFormatKHR>& available_formats ) const;
            VkPresentModeKHR choose_present_mode( std::vector<VkPresentModeKHR>& available_present_modes ) const;

        private:
            const core::logical_device* p_logical_device_;

            VkSwapchainKHR swapchain_handle_ = VK_NULL_HANDLE;
            VkImage* image_handles_ = VK_NULL_HANDLE;
            VkImageView* image_view_handles_ = VK_NULL_HANDLE;

            VkFormat format_;
            VkExtent2D extent_;

            uint32_t image_count_;
        };
    }
}

#endif //PROJEKT_SWAPCHAIN_H
