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
            swapchain( const core::logical_device* p_logical_device, const core::physical_device& physical_device, const surface& surface );
            swapchain( const swapchain& swapchain ) = delete;
            swapchain( swapchain&& swapchain ) noexcept;
            ~swapchain( );

            VkSwapchainKHR& get()
            {
                return swapchain_handle_;
            }

            VkImageView& get_image_view( uint32_t i )
            {
                return image_view_handles_[i];
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

            swapchain& operator=( const swapchain& swapchain ) = delete;
            swapchain& operator=( swapchain&& swapchain ) noexcept;

        private:
            VkExtent2D choose_extent_2d( VkSurfaceCapabilitiesKHR& capabilities, const surface& surface ) const;
            VkSurfaceFormatKHR choose_surface_format( std::vector<VkSurfaceFormatKHR>& available_formats ) const;
            VkPresentModeKHR choose_present_mode( std::vector<VkPresentModeKHR>& available_present_modes ) const;

        private:
            const core::logical_device* p_logical_device_;

            VkSwapchainKHR swapchain_handle_ = VK_NULL_HANDLE;
            VkImage* image_handles_;
            VkImageView* image_view_handles_;

            VkFormat format_;
            VkExtent2D extent_;

            uint32_t image_count_;
        };
    }
}

#endif //PROJEKT_SWAPCHAIN_H
