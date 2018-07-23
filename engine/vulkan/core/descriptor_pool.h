/*!
 *
 */

#ifndef PROJEKT_DESCRIPTOR_POOL_H
#define PROJEKT_DESCRIPTOR_POOL_H

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        class descriptor_pool
        {
        public:
            descriptor_pool( ) = default;
            descriptor_pool( const logical_device* p_logical_device, uint32_t count );
            descriptor_pool( const descriptor_pool& descriptor_pool ) = delete;
            descriptor_pool( descriptor_pool&& descriptor_pool ) noexcept;
            ~descriptor_pool( );

            VkDescriptorPool& get()
            {
                return descriptor_pool_handle_;
            }

            VkDescriptorSet* allocate_descriptor_set( VkDescriptorSetAllocateInfo& allocate_info, uint32_t count ) const;
            VkDescriptorSet* free_descriptor_set( VkDescriptorSet* descriptor_set_handle, uint32_t count ) const;

            descriptor_pool& operator=( const descriptor_pool& descriptor_pool ) = delete;
            descriptor_pool& operator=( descriptor_pool&& descriptor_pool ) noexcept;

        private:
            const logical_device* p_logical_device_;

            VkDescriptorPool descriptor_pool_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_DESCRIPTOR_POOL_H
