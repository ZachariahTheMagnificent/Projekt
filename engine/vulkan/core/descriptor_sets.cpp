/*!
 *
 */

#include "descriptor_sets.h"

namespace vk
{
    namespace core
    {

        descriptor_sets::descriptor_sets( const logical_device& logical_device,
                                          const descriptor_pool* p_descriptor_pool,
                                          const descriptor_set_layout& set_layout,
                                          const VkBuffer* p_buffers,
                                          const VkDeviceSize buffer_range, uint32_t count )
            :
            p_descriptor_pool_( p_descriptor_pool ),
            count_( count )
        {
            std::vector<VkDescriptorSetLayout> layouts( count, set_layout.get() );

            VkDescriptorSetAllocateInfo allocate_info = {};
            allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocate_info.descriptorSetCount = count;
            allocate_info.pSetLayouts = layouts.data();

            descriptor_set_handles_ = p_descriptor_pool_->allocate_descriptor_set( allocate_info, count_ );

            for( auto i = 0; i < count_; ++i )
            {
                VkDescriptorBufferInfo buffer_info = {};
                buffer_info.buffer = p_buffers[i];
                buffer_info.range = buffer_range;
                buffer_info.offset = 0;

                VkWriteDescriptorSet descriptor_write = {};
                descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_write.dstSet = descriptor_set_handles_[i];
                descriptor_write.dstBinding = 0;
                descriptor_write.dstArrayElement = 0;
                descriptor_write.descriptorCount = 1;
                descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptor_write.pBufferInfo = &buffer_info;

                logical_device.update_descriptor_set( 1, &descriptor_write, 0, nullptr );
            }
        }
        descriptor_sets::descriptor_sets( descriptor_sets&& descriptor_sets ) noexcept
        {
            *this = std::move( descriptor_sets );
        }
        descriptor_sets::~descriptor_sets( )
        {
            /*
            if( descriptor_set_handles_ != VK_NULL_HANDLE )
                //descriptor_set_handles_ = p_descriptor_pool_->free_descriptor_set( descriptor_set_handles_, count_ );
            */
        }

        descriptor_sets& descriptor_sets::operator=( descriptor_sets&& descriptor_sets ) noexcept
        {
            if( this != &descriptor_sets )
            {
                /*
                if( descriptor_set_handles_ != VK_NULL_HANDLE )
                    descriptor_set_handles_ = p_descriptor_pool_->free_descriptor_set( descriptor_set_handles_, count_ );
                 */

                count_ = descriptor_sets.count_;
                descriptor_sets.count_ = 0;

                descriptor_set_handles_ = descriptor_sets.descriptor_set_handles_;
                descriptor_sets.descriptor_set_handles_ = VK_NULL_HANDLE;

                p_descriptor_pool_ = descriptor_sets.p_descriptor_pool_;
            }

            return *this;
        }
    }
}