/*!
 *
 */

#ifndef PROJEKT_COMPUTE_PIPELINE_H
#define PROJEKT_COMPUTE_PIPELINE_H

#include "logical_device.h"
#include "shader_module.h"

namespace vk
{
    namespace core
    {
        class compute_pipeline
        {
        public:
            compute_pipeline( ) = default;
            compute_pipeline( const logical_device* p_logical_device, shader_module& computer_shader );

        private:
            const logical_device* p_logical_device_;

            VkPipeline pipeline_handle_ = VK_NULL_HANDLE;
            VkPipelineLayout pipeline_layout_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //PROJEKT_COMPUTE_PIPELINE_H
