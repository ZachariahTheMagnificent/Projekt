/*!
 *
 */

#ifndef COMPUTE_SHADERMODULE_H
#define COMPUTE_SHADERMODULE_H

#include "logical_device.h"

namespace vk
{
    namespace core
    {
        class shader_module
        {
        public:
            shader_module( ) = default;
            shader_module( const logical_device* p_logical_device, const std::string& shader_location );
            shader_module( const shader_module& shader_module ) = delete;
            shader_module( shader_module&& shader_module ) noexcept;
            ~shader_module( );

            VkPipelineShaderStageCreateInfo create_shader_stage_info( VkShaderStageFlagBits stage_flag );

            shader_module& operator=( const shader_module& shader_module ) = delete;
            shader_module& operator=( shader_module&& shader_module ) noexcept;

        private:
            const logical_device* p_logical_device_;

            VkShaderModule shader_module_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_SHADERMODULE_H
