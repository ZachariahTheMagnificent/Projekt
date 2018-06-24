/*!
 *
 */

#include "ShaderModule.h"

#include "../../Utils/File/Read.h"

namespace Vk
{
    namespace Core
    {
        ShaderModule::ShaderModule( const std::string& shader_location )
        {
            auto shader_code = read_from_binary_file( shader_location );

            VkShaderModuleCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize = shader_code.size();
            create_info.pCode = reinterpret_cast<const uint32_t*>( shader_code.data() );

            shader_module_handle_ = p_logical_device_->create_shader_module( create_info );
        }
        ShaderModule::ShaderModule( ShaderModule&& shader_module ) noexcept
        {
            *this = std::move( shader_module );
        }
        ShaderModule::~ShaderModule( )
        {
            shader_module_handle_ = p_logical_device_->destroy_shader_module( shader_module_handle_ );
        }

        VkPipelineShaderStageCreateInfo ShaderModule::create_shader_stage_info( VkShaderStageFlagBits& stage_flag )
        {
            VkPipelineShaderStageCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            create_info.stage = stage_flag;
            create_info.module = shader_module_handle_;
            create_info.pName = "main";

            return create_info;
        }

        ShaderModule& ShaderModule::operator=( ShaderModule&& shader_module ) noexcept
        {
            if( this != &shader_module )
            {
                if( shader_module_handle_ != VK_NULL_HANDLE )
                    shader_module_handle_ = p_logical_device_->destroy_shader_module( shader_module_handle_ );

                shader_module_handle_ = shader_module.shader_module_handle_;
                shader_module.shader_module_handle_ = VK_NULL_HANDLE;

                p_logical_device_ = shader_module.p_logical_device_;
            }

            return *this;
        }
    }
}
