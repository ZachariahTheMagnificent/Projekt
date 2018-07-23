/*!
 *
 */

#include "descriptor_pool.h"

namespace vk
{
    namespace core
    {
        descriptor_pool::descriptor_pool( const logical_device* p_logical_device, uint32_t count )
            :
            p_logical_device_( p_logical_device )
        {
            VkDescriptorPoolSize pool_size = {};
            pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pool_size.descriptorCount = count;

            VkDescriptorPoolCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            create_info.poolSizeCount = 1;
            create_info.pPoolSizes = &pool_size;
            create_info.maxSets = count;

            descriptor_pool_handle_ = p_logical_device_->create_descriptor_pool( create_info );
        }
        descriptor_pool::descriptor_pool( descriptor_pool&& descriptor_pool ) noexcept
        {
            *this = std::move( descriptor_pool );
        }
        descriptor_pool::~descriptor_pool( )
        {
            if( descriptor_pool_handle_ != VK_NULL_HANDLE )
                descriptor_pool_handle_ = p_logical_device_->destroy_descriptor_pool( descriptor_pool_handle_ );
        }

        VkDescriptorSet*
        descriptor_pool::allocate_descriptor_set( VkDescriptorSetAllocateInfo& allocate_info, uint32_t count ) const
        {
            allocate_info.descriptorPool = descriptor_pool_handle_;

            return p_logical_device_->allocate_descriptor_sets_( allocate_info, count );
        }
        VkDescriptorSet*
        descriptor_pool::free_descriptor_set( VkDescriptorSet* descriptor_set_handle, uint32_t count ) const
        {
            return p_logical_device_->free_descriptor_sets_( descriptor_pool_handle_, descriptor_set_handle, count );
        }

        descriptor_pool&
        descriptor_pool::operator=( descriptor_pool&& descriptor_pool ) noexcept
        {
            if( this != &descriptor_pool )
            {
                if( descriptor_pool_handle_ != VK_NULL_HANDLE )
                    descriptor_pool_handle_ = p_logical_device_->destroy_descriptor_pool( descriptor_pool_handle_ );

                descriptor_pool_handle_ = descriptor_pool.descriptor_pool_handle_;
                descriptor_pool.descriptor_pool_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = descriptor_pool.p_logical_device_;
            }

            return *this;
        }
    }
}