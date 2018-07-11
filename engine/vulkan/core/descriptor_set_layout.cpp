/*!
 *
 */

#include "descriptor_set_layout.h"

namespace vk
{
    namespace core
    {
        descriptor_set_layout::descriptor_set_layout( const logical_device* p_logical_device, VkShaderStageFlags flags )
            :
            p_logical_device_( p_logical_device )
        {
            VkDescriptorSetLayoutBinding ubo_layout_binding = {};
            ubo_layout_binding.binding = 0;
            ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo_layout_binding.descriptorCount = 1;
            ubo_layout_binding.stageFlags = flags;

            VkDescriptorSetLayoutCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            create_info.bindingCount = 1;
            create_info.pBindings = &ubo_layout_binding;

            descriptor_set_layout_handle_ = p_logical_device_->create_descriptor_set_layout( create_info );
        }
        descriptor_set_layout::descriptor_set_layout( descriptor_set_layout&& descriptor_set_layout ) noexcept
        {
            *this = std::move( descriptor_set_layout );
        }
        descriptor_set_layout::~descriptor_set_layout( )
        {
            if( descriptor_set_layout_handle_ != VK_NULL_HANDLE )
                descriptor_set_layout_handle_ = p_logical_device_->destroy_descriptor_set_layout( descriptor_set_layout_handle_ );
        }

        descriptor_set_layout&
        descriptor_set_layout::operator=( descriptor_set_layout&& descriptor_set_layout ) noexcept
        {
            if( this != &descriptor_set_layout )
            {
                if( descriptor_set_layout_handle_ != VK_NULL_HANDLE )
                    descriptor_set_layout_handle_ = p_logical_device_->destroy_descriptor_set_layout( descriptor_set_layout_handle_ );

                descriptor_set_layout_handle_ = descriptor_set_layout.descriptor_set_layout_handle_;
                descriptor_set_layout.descriptor_set_layout_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = descriptor_set_layout.p_logical_device_;
            }

            return *this;
        }
    }
}