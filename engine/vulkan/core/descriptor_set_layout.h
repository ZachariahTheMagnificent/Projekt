/*!
 *
 */

#ifndef PROJEKT_DESCRIPTOR_SET_LAYOUT_H
#define PROJEKT_DESCRIPTOR_SET_LAYOUT_H

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        class descriptor_set_layout
        {
        public:
            descriptor_set_layout() = default;
            descriptor_set_layout( const logical_device* p_logical_device, VkShaderStageFlags flags );
            descriptor_set_layout( const descriptor_set_layout& descriptor_set_layout ) = delete;
            descriptor_set_layout( descriptor_set_layout&& descriptor_set_layout ) noexcept;
            ~descriptor_set_layout( );

            const VkDescriptorSetLayout& get( ) const
            {
                return descriptor_set_layout_handle_;
            }

            descriptor_set_layout& operator=( const descriptor_set_layout& descriptor_set_layout ) = delete;
            descriptor_set_layout& operator=( descriptor_set_layout&& descriptor_set_layout ) noexcept;

        private:
            const logical_device* p_logical_device_;

            VkDescriptorSetLayout descriptor_set_layout_handle_ = VK_NULL_HANDLE;
        };

    }
}

#endif //PROJEKT_DESCRIPTOR_SET_LAYOUT_H
