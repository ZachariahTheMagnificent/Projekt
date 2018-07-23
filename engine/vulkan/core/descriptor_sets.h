/*!
 *
 */

#ifndef PROJEKT_DESCRIPTOR_SET_H
#define PROJEKT_DESCRIPTOR_SET_H

#include "logical_device.h"
#include "descriptor_set_layout.h"
#include "descriptor_pool.h"

namespace vk
{
    namespace core
    {
        class descriptor_sets
        {
        public:
            descriptor_sets( ) = default;
            descriptor_sets( const logical_device& logical_device,
                             const descriptor_pool* p_descriptor_pool, const descriptor_set_layout& set_layout,
                             const VkBuffer* p_buffers, const VkDeviceSize buffer_range, uint32_t count );
            descriptor_sets( const descriptor_sets& descriptor_sets ) = delete;
            descriptor_sets( descriptor_sets&& descriptor_sets ) noexcept;
            ~descriptor_sets( );

            VkDescriptorSet& operator[]( uint32_t index )
            {
                return descriptor_set_handles_[index];
            }

            descriptor_sets& operator=( const descriptor_sets& descriptor_sets ) = delete;
            descriptor_sets& operator=( descriptor_sets&& descriptor_sets ) noexcept;

        private:
            const descriptor_pool* p_descriptor_pool_;
            uint32_t count_;

            VkDescriptorSet* descriptor_set_handles_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_DESCRIPTOR_SET_H
