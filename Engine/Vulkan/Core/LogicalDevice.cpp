/*!
 *
 */

#include <vector>
#include <set>
#include <iostream>

#include "LogicalDevice.h"
#include "Instance.h"

namespace Vk
{
    namespace Core
    {
        LogicalDevice::LogicalDevice( PhysicalDevice& physical_device,
                                      const std::vector<const char*>& validation_layers,
                                      const std::vector<const char*>& device_extensions )
        {
            std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
            auto unique_queue_families = physical_device.unique_queue_families();

            float queue_priority = 1.0f;
            for( const auto& queue_family : unique_queue_families )
            {
                VkDeviceQueueCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                create_info.queueFamilyIndex = queue_family;
                create_info.queueCount = 1;
                create_info.pQueuePriorities = &queue_priority;

                queue_create_infos.emplace_back( create_info );
            }

            auto& physical_device_features = physical_device.features();

            VkDeviceCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount = unique_queue_families.size();
            create_info.pQueueCreateInfos = queue_create_infos.data();
            create_info.pEnabledFeatures = &physical_device_features;
            create_info.enabledExtensionCount = static_cast<uint32_t>( device_extensions.size() );
            create_info.ppEnabledExtensionNames = device_extensions.data();

            if( enable_validation_layers )
            {
                create_info.enabledLayerCount = static_cast<uint32_t>( validation_layers.size() );
                create_info.ppEnabledLayerNames = validation_layers.data();
            }
            else
            {
                create_info.enabledLayerCount = 0;
            }

            device_handle_ = physical_device.create_device( create_info );
        }
        LogicalDevice::LogicalDevice( LogicalDevice &&logical_device ) noexcept
        {
            *this = std::move( logical_device );
        }
        LogicalDevice::~LogicalDevice( )
        {
            vkDestroyDevice( device_handle_, nullptr );

            std::cout << "Logical device destroyed." << std::endl;
        }

        LogicalDevice&
        LogicalDevice::operator=( LogicalDevice &&logical_device ) noexcept
        {
            if( this != &logical_device )
            {
                if( device_handle_ != VK_NULL_HANDLE )
                {
                    vkDestroyDevice( device_handle_, nullptr );
                    device_handle_ = VK_NULL_HANDLE;
                }

                device_handle_ = logical_device.device_handle_;
                logical_device.device_handle_ = VK_NULL_HANDLE;
            }

            return *this;
        }

        VkQueue
        LogicalDevice::get_queue( int32_t family_index, uint32_t queue_index ) const
        {
            VkQueue queue_handle;

            vkGetDeviceQueue( device_handle_, family_index, queue_index, &queue_handle );

            std::cout << "Queue acquired." << std::endl;

            return queue_handle;
        }

        VkCommandPool
        LogicalDevice::create_command_pool( VkCommandPoolCreateInfo& create_info ) const
        {
            VkCommandPool command_pool_handle;

            if( vkCreateCommandPool( device_handle_, &create_info, nullptr, &command_pool_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Command Pool." << std::endl;
            else
                std::cout << "Command Pool created successfully." << std::endl;

            return command_pool_handle;
        }
        VkCommandPool
        LogicalDevice::destroy_command_pool( VkCommandPool& command_pool ) const
        {
            vkDestroyCommandPool( device_handle_, command_pool, nullptr );

            std::cout << "Command Pool destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }


        VkCommandBuffer
        LogicalDevice::allocate_command_buffer( VkCommandBufferAllocateInfo& allocate_info ) const
        {
            VkCommandBuffer command_buffer_handle;

            if( vkAllocateCommandBuffers( device_handle_, &allocate_info, &command_buffer_handle ) != VK_SUCCESS )
                std::cerr << "Failed to allocate Command Buffer." << std::endl;
            else
                std::cout << "Command Buffer allocated successfully." << std::endl;

            return nullptr;
        }
        VkCommandBuffer
        LogicalDevice::free_command_buffer( const VkCommandPool& command_pool_handle,
                                                            VkCommandBuffer& command_buffer_handle ) const
        {
            vkFreeCommandBuffers( device_handle_, command_pool_handle, 1, &command_buffer_handle );

            return VK_NULL_HANDLE;
        }
        std::vector<VkCommandBuffer>
        LogicalDevice::allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t number ) const
        {
            std::vector<VkCommandBuffer> command_buffers( number );

            if( vkAllocateCommandBuffers( device_handle_, &allocate_info, command_buffers.data() ) != VK_SUCCESS )
                std::cerr << "Failed to allocate Command Buffers." << std::endl;
            else
                std::cout << "Command Buffers allocated successfully." << std::endl;

            return command_buffers;
        }
        std::vector<VkCommandBuffer>
        LogicalDevice::free_command_buffers( const VkCommandPool& command_pool_handle, std::vector<VkCommandBuffer>& command_buffer_handles ) const
        {
            vkFreeCommandBuffers( device_handle_, command_pool_handle,
                                  static_cast<uint32_t>( command_buffer_handles.size() ),
                                  command_buffer_handles.data() );

            std::cout << "Command Buffers freed." << std::endl;

            return std::vector<VkCommandBuffer>( );
        }

        VkSemaphore
        LogicalDevice::create_semaphore( VkSemaphoreCreateInfo& create_info ) const
        {
            VkSemaphore semaphore_handle = VK_NULL_HANDLE;

            if( vkCreateSemaphore( device_handle_, &create_info, nullptr, &semaphore_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create semaphore" << std::endl;
            else
                std::cout << "Semaphore created successfully." << std::endl;

            return semaphore_handle;
        }

        VkSemaphore
        LogicalDevice::destroy_semaphore( VkSemaphore& semaphore_handle ) const
        {
            vkDestroySemaphore( device_handle_, semaphore_handle, nullptr );

            std::cout << "Semaphore destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkFence
        LogicalDevice::create_fence( VkFenceCreateInfo& create_info ) const
        {
            VkFence fence_handle;

            if( vkCreateFence( device_handle_, &create_info, nullptr, &fence_handle ) != VK_NULL_HANDLE )
                std::cerr << "Failed to create fence." << std::endl;
            else
                std::cout << "Fence create successfully." << std::endl;

            return fence_handle;
        }

        VkFence
        LogicalDevice::destroy_fence( VkFence& fence_handle ) const
        {
            vkDestroyFence( device_handle_, fence_handle, nullptr );

            std::cout << "Fence destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        void
        LogicalDevice::wait_for_fences( VkFence* p_fence_handle, uint32_t fence_count, VkBool32 wait_all, uint64_t timeout ) const
        {
            vkWaitForFences( device_handle_, fence_count, p_fence_handle, wait_all, timeout );
        }

        void
        LogicalDevice::reset_fences( VkFence* p_fence_handle, uint32_t fence_count ) const
        {
            vkResetFences( device_handle_, fence_count, p_fence_handle );
        }

        VkShaderModule
        LogicalDevice::create_shader_module( VkShaderModuleCreateInfo& create_info ) const
        {
            VkShaderModule shader_module_handle;

            if( vkCreateShaderModule( device_handle_, &create_info, nullptr, &shader_module_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Shader Module." << std::endl;
            else
                std::cout << "Shader Module created successfully." << std::endl;

            return shader_module_handle;
        }

        VkShaderModule
        LogicalDevice::destroy_shader_module( VkShaderModule& shader_module_handle ) const
        {
            vkDestroyShaderModule( device_handle_, shader_module_handle, nullptr );

            std::cout << "Shader Module destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }
    }
}
