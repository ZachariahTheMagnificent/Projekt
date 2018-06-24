/*!
 *
 */

#ifndef COMPUTE_SHADERMODULE_H
#define COMPUTE_SHADERMODULE_H

#include "LogicalDevice.h"

namespace Vk
{
    namespace Core
    {
        class ShaderModule
        {
        public:
            ShaderModule( ) = default;
            explicit ShaderModule( const std::string& shader_location );
            ShaderModule( const ShaderModule& shader_module ) = delete;
            ShaderModule( ShaderModule&& shader_module ) noexcept;
            ~ShaderModule( );

            VkPipelineShaderStageCreateInfo create_shader_stage_info( VkShaderStageFlagBits& stage_flag );

            ShaderModule& operator=( const ShaderModule& shader_module ) = delete;
            ShaderModule& operator=( ShaderModule&& shader_module ) noexcept;

        private:
            const LogicalDevice* p_logical_device_;

            VkShaderModule shader_module_handle_ = VK_NULL_HANDLE;
        };
    }
}

#endif //COMPUTE_SHADERMODULE_H
