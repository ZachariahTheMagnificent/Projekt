/*!
 *
 */

#include <vector>
#include <set>
#include <iostream>

#include "logical_device.h"
#include "instance.h"

namespace vk
{
    namespace core
    {
        logical_device::logical_device( physical_device& physical_device,
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

            if ( enable_validation_layers )
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
        logical_device::logical_device( logical_device &&logical_device ) noexcept
        {
            *this = std::move( logical_device );
        }
        logical_device::~logical_device( )
        {
            if( device_handle_ != VK_NULL_HANDLE )
            {
                vkDestroyDevice( device_handle_, nullptr );

                std::cout << "Logical device destroyed." << std::endl;
            }
        }

        void
        logical_device::wait_idle( )
        {
            vkDeviceWaitIdle( device_handle_ );
        }

        logical_device&
        logical_device::operator=( logical_device &&logical_device ) noexcept
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
        logical_device::get_queue( int32_t family_index, uint32_t queue_index ) const
        {
            VkQueue queue_handle;

            vkGetDeviceQueue( device_handle_, family_index, queue_index, &queue_handle );

            std::cout << "queue acquired." << std::endl;

            return queue_handle;
        }

        VkCommandPool
        logical_device::create_command_pool( VkCommandPoolCreateInfo& create_info ) const
        {
            VkCommandPool command_pool_handle;

            if( vkCreateCommandPool( device_handle_, &create_info, nullptr, &command_pool_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Command Pool." << std::endl;
            else
                std::cout << "Command Pool created successfully." << std::endl;

            return command_pool_handle;
        }
        VkCommandPool
        logical_device::destroy_command_pool( VkCommandPool& command_pool ) const
        {
            vkDestroyCommandPool( device_handle_, command_pool, nullptr );

            std::cout << "Command Pool destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkCommandBuffer*
        logical_device::allocate_command_buffers( VkCommandBufferAllocateInfo& allocate_info, uint32_t count ) const
        {
            auto* command_buffer_handles = new VkCommandBuffer[count];

            if( vkAllocateCommandBuffers( device_handle_, &allocate_info, command_buffer_handles ) != VK_SUCCESS )
                std::cerr << "Failed to allocate Command Buffers." << std::endl;
            else
                std::cout << "Command Buffers allocated successfully." << std::endl;

            return command_buffer_handles;
        }
        VkCommandBuffer*
        logical_device::free_command_buffers( const VkCommandPool& command_pool_handle,
                                                               VkCommandBuffer* command_buffer_handles,
                                                               uint32_t count ) const
        {
            vkFreeCommandBuffers( device_handle_, command_pool_handle, count, command_buffer_handles );

            std::cout << "Command Buffers freed." << std::endl;

            return nullptr;
        }

        VkSemaphore*
        logical_device::create_semaphores( VkSemaphoreCreateInfo& create_info, uint32_t count ) const
        {
            auto* semaphore_handles = new VkSemaphore[count];

            for( auto i = 0; i < count; ++i )
            {
                if( vkCreateSemaphore( device_handle_, &create_info, nullptr, &semaphore_handles[i] ) != VK_SUCCESS )
                    std::cerr << "Failed to create semaphore." << std::endl;
                else
                    std::cout << "Semaphore created successfully." << std::endl;
            }

            return semaphore_handles;
        }
        VkSemaphore*
        logical_device::destroy_semaphores( VkSemaphore* semaphore_handles, uint32_t count ) const
        {
            for( auto i = 0; i < count; ++i )
            {
                vkDestroySemaphore( device_handle_, semaphore_handles[i], nullptr );

                std::cout << "Semaphore destroyed." << std::endl;
            }

            return nullptr;
        }

        VkFence*
        logical_device::create_fences( VkFenceCreateInfo& create_info, uint32_t count ) const
        {
            auto* fence_handles = new VkFence[count];

            for( auto i = 0; i < count; ++i )
            {
                if( vkCreateFence( device_handle_, &create_info, nullptr, &fence_handles[i] ) != VK_SUCCESS )
                    std::cerr << "Failed to create fence." << std::endl;
                else
                    std::cout << "Fence create successfully." << std::endl;
            }

            return fence_handles;
        }
        VkFence*
        logical_device::destroy_fences( VkFence* fence_handle, uint32_t count ) const
        {
            for( auto i = 0; i < count; ++i )
            {
                vkDestroyFence( device_handle_, fence_handle[i], nullptr );

                std::cout << "Fence destroyed." << std::endl;
            }

            return nullptr;
        }

        void
        logical_device::wait_for_fences( VkFence* p_fence_handle, uint32_t fence_count, VkBool32 wait_all, uint64_t timeout ) const
        {
            vkWaitForFences( device_handle_, fence_count, p_fence_handle, wait_all, timeout );
        }
        void
        logical_device::reset_fences( VkFence* p_fence_handle, uint32_t fence_count ) const
        {
            vkResetFences( device_handle_, fence_count, p_fence_handle );
        }

        VkSwapchainKHR
        logical_device::create_swapchain( VkSwapchainCreateInfoKHR& create_info ) const
        {
            VkSwapchainKHR swapchain_handle;

            if( vkCreateSwapchainKHR( device_handle_, &create_info, nullptr, &swapchain_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create swapchain." << std::endl;
            else
                std::cout << "swapchain created successfully." << std::endl;

            return swapchain_handle;
        }
        VkSwapchainKHR
        logical_device::destroy_swapchain( VkSwapchainKHR& swapchain_handle ) const
        {
            vkDestroySwapchainKHR( device_handle_, swapchain_handle, nullptr );

            return VK_NULL_HANDLE;
        }

        VkImage*
        logical_device::create_images( VkSwapchainKHR& swapchain_handle, uint32_t& count ) const
        {
            vkGetSwapchainImagesKHR( device_handle_, swapchain_handle, &count, nullptr );
            VkImage *images_handle = new VkImage[count];
            vkGetSwapchainImagesKHR( device_handle_, swapchain_handle, &count, images_handle );

            return images_handle;
        }
        VkImage*
        logical_device::destroy_images( VkImage* image_handles ) const
        {
            delete[] image_handles;

            return nullptr;
        }

        VkImageView*
        logical_device::create_image_views( VkImage* image_handles, VkImageViewCreateInfo& create_info, uint32_t count ) const
        {
            VkImageView* image_view_handles = new VkImageView[count];

            for( auto i = 0; i < count; ++i )
            {
                create_info.image = image_handles[i];

                if( vkCreateImageView( device_handle_, &create_info, nullptr, &image_view_handles[i] ) != VK_SUCCESS )
                    std::cerr << "Failed to create Image View." << std::endl;
                else
                    std::cout << "Image View created successfully." << std::endl;
            }

            return image_view_handles;
        }
        VkImageView*
        logical_device::destroy_image_views( VkImageView* image_view_handles, uint32_t count ) const
        {
            for( auto i = 0; i < count; ++i )
            {
                vkDestroyImageView( device_handle_, image_view_handles[i], nullptr );

                std::cout << "Image View destroyed." << std::endl;
            }

            delete[] image_view_handles;

            return nullptr;
        }

        VkRenderPass
        logical_device::create_render_pass( VkRenderPassCreateInfo& create_info ) const
        {
            VkRenderPass render_pass_handle;

            if( vkCreateRenderPass( device_handle_, &create_info, nullptr, &render_pass_handle ) != VK_NULL_HANDLE )
                std::cerr << "Failed to create Render Pass." << std::endl;
            else
                std::cout << "Render Pass created successfully." << std::endl;

            return render_pass_handle;
        }
        VkRenderPass
        logical_device::destroy_render_pass( VkRenderPass& render_pass_handle ) const
        {
            vkDestroyRenderPass( device_handle_, render_pass_handle, nullptr );

            std::cout << "Render Pass destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkFramebuffer*
        logical_device::create_frame_buffers( const VkImageView* image_view_handles, VkFramebufferCreateInfo& create_info,
                                              uint32_t count ) const
        {
            auto* frame_buffer_handles = new VkFramebuffer[count];

            for( auto i = 0; i < count; ++i )
            {
                VkImageView attachments[] =
                {
                    image_view_handles[i]
                };

                create_info.attachmentCount = 1;
                create_info.pAttachments = attachments;

                if( vkCreateFramebuffer( device_handle_, &create_info, nullptr, &frame_buffer_handles[i] ) != VK_SUCCESS )
                    std::cerr << "Failed to create Frame Buffer." << std::endl;
                else
                    std::cout << "Frame Buffer created successfully." << std::endl;
            }

            return frame_buffer_handles;
        }
        VkFramebuffer*
        logical_device::destroy_frame_buffers( VkFramebuffer* frame_buffer_handles, uint32_t count ) const
        {
            for( auto i = 0; i < count; ++i )
            {
                vkDestroyFramebuffer( device_handle_, frame_buffer_handles[i], nullptr );

                frame_buffer_handles[i] = VK_NULL_HANDLE;

                std::cout << "Frame Buffer destroyed." << std::endl;
            }

            delete[] frame_buffer_handles;

            return nullptr;
        }

        VkShaderModule
        logical_device::create_shader_module( VkShaderModuleCreateInfo& create_info ) const
        {
            VkShaderModule shader_module_handle;

            if( vkCreateShaderModule( device_handle_, &create_info, nullptr, &shader_module_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Shader Module." << std::endl;
            else
                std::cout << "Shader Module created successfully." << std::endl;

            return shader_module_handle;
        }
        VkShaderModule
        logical_device::destroy_shader_module( VkShaderModule& shader_module_handle ) const
        {
            vkDestroyShaderModule( device_handle_, shader_module_handle, nullptr );

            std::cout << "Shader Module destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkPipelineLayout
        logical_device::create_pipeline_layout( VkPipelineLayoutCreateInfo& create_info ) const
        {
            VkPipelineLayout pipeline_layout_handle;

            if( vkCreatePipelineLayout( device_handle_, &create_info, nullptr, &pipeline_layout_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Pipeline Layout." << std::endl;
            else
                std::cout << "Pipeline Layout create successfully." << std::endl;

            return pipeline_layout_handle;
        }
        VkPipelineLayout
        logical_device::destroy_pipeline_layout( VkPipelineLayout& pipeline_layout_handle ) const
        {
            vkDestroyPipelineLayout( device_handle_, pipeline_layout_handle, nullptr );

            std::cout << "Pipeline Layout destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkPipelineCache
        logical_device::create_pipeline_cache( VkPipelineCacheCreateInfo& create_info ) const
        {
            VkPipelineCache pipeline_cache_handle;

            if( vkCreatePipelineCache( device_handle_, &create_info, nullptr, &pipeline_cache_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Pipeline Cache." << std::endl;
            else
                std::cout << "Pipeline Cache created successfully." << std::endl;

            return pipeline_cache_handle;
        }
        VkPipelineCache
        logical_device::destroy_pipeline_cache( VkPipelineCache& pipeline_cache_handle ) const
        {
            vkDestroyPipelineCache( device_handle_, pipeline_cache_handle, nullptr );

            std::cout << "Pipeline Cache destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkPipeline
        logical_device::create_compute_pipeline( VkPipelineCache pipeline_cache_handle, VkComputePipelineCreateInfo& create_info ) const
        {
            VkPipeline pipeline_handle;

            if( vkCreateComputePipelines( device_handle_, pipeline_cache_handle, 1, &create_info, nullptr, &pipeline_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Compute Pipeline." << std::endl;
            else
                std::cout << "Compute Pipeline created successfully." << std::endl;

            return pipeline_handle;
        }
        VkPipeline
        logical_device::create_graphics_pipeline( VkPipelineCache pipeline_cache_handle, VkGraphicsPipelineCreateInfo& create_info ) const
        {
            VkPipeline pipeline_handle;

            if( vkCreateGraphicsPipelines( device_handle_, pipeline_cache_handle, 1, &create_info, nullptr, &pipeline_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Graphics Pipeline." << std::endl;
            else
                std::cout << "Graphics Pipeline created successfully." << std::endl;

            return pipeline_handle;
        }
        VkPipeline
        logical_device::destroy_pipeline( VkPipeline& pipeline_handle ) const
        {
            vkDestroyPipeline( device_handle_, pipeline_handle, nullptr );

            std::cout << "Pipeline destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkBuffer
        logical_device::create_buffer( VkBufferCreateInfo& create_info ) const
        {
            VkBuffer buffer_handle;

            if( vkCreateBuffer( device_handle_, &create_info, nullptr, &buffer_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Buffer." << std::endl;
            else
                std::cout << "Buffer created successfully." << std::endl;

            return buffer_handle;
        }
        VkBuffer
        logical_device::destroy_buffer( VkBuffer& buffer_handle ) const
        {
            vkDestroyBuffer( device_handle_, buffer_handle, nullptr );

            std::cout << "Buffer destroyed." << std::endl;

            return VK_NULL_HANDLE;
        }

        VkMemoryRequirements
        logical_device::get_buffer_memory_requirements( VkBuffer& buffer_handle ) const
        {
            VkMemoryRequirements mem_reqs;

            vkGetBufferMemoryRequirements( device_handle_, buffer_handle, &mem_reqs );

            return mem_reqs;
        }

        VkDeviceMemory
        logical_device::allocate_memory( VkMemoryAllocateInfo& allocate_info ) const
        {
            VkDeviceMemory memory_handle;

            if( vkAllocateMemory( device_handle_, &allocate_info, nullptr, &memory_handle ) != VK_SUCCESS )
                std::cerr << "Failed to allocate memory." << std::endl;
            else
                std::cout << "Memory allocated successfully." << std::endl;

            return memory_handle;
        }
        VkDeviceMemory
        logical_device::free_memory( VkDeviceMemory& memory_handle ) const
        {
            vkFreeMemory( device_handle_, memory_handle, nullptr );

            std::cout << "Memory freed." << std::endl;

            return VK_NULL_HANDLE;
        }

        void
        logical_device::bind_buffer_memory( VkBuffer& buffer_handle, VkDeviceMemory& memory_handle,
                                                 VkDeviceSize& offset ) const
        {
            vkBindBufferMemory( device_handle_, buffer_handle, memory_handle, offset );
        }

        void
        logical_device::map_memory( VkDeviceMemory& memory_handle, VkDeviceSize& offset, VkDeviceSize& size,
                                    VkMemoryMapFlags& flags, void** pp_data ) const
        {
            vkMapMemory( device_handle_, memory_handle, offset, size, flags, pp_data );
        }
        void
        logical_device::unmap_memory( VkDeviceMemory& memory_handle ) const
        {
            vkUnmapMemory( device_handle_, memory_handle );
        }

        VkDescriptorSetLayout
        logical_device::create_descriptor_set_layout( VkDescriptorSetLayoutCreateInfo& create_info ) const
        {
            VkDescriptorSetLayout layout_handle;

            if( vkCreateDescriptorSetLayout( device_handle_, &create_info, nullptr, &layout_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Descriptor Set Layout." << std::endl;
            else
                std::cout << "Descriptor Set Layout created successfully." << std::endl;

            return layout_handle;
        }
        VkDescriptorSetLayout
        logical_device::destroy_descriptor_set_layout( VkDescriptorSetLayout& descriptor_set_layout_handle ) const
        {
            vkDestroyDescriptorSetLayout( device_handle_, descriptor_set_layout_handle, nullptr );

            return VK_NULL_HANDLE;
        }

        VkDescriptorSet*
        logical_device::allocate_descriptor_sets_( VkDescriptorSetAllocateInfo& allocate_info, uint32_t count ) const
        {
            auto* descriptor_set = new VkDescriptorSet[count];

            if( vkAllocateDescriptorSets( device_handle_, &allocate_info, descriptor_set ) != VK_NULL_HANDLE )
                std::cerr << "Failed to allocate Descriptor Sets" << std::endl;

            return descriptor_set;
        }
        VkDescriptorSet*
        logical_device::free_descriptor_sets_( const VkDescriptorPool& descriptor_pool_handle, VkDescriptorSet* descriptor_set_handles, uint32_t count ) const
        {
            vkFreeDescriptorSets( device_handle_, descriptor_pool_handle, count, descriptor_set_handles );

            return VK_NULL_HANDLE;
        }

        void
        logical_device::update_descriptor_set( uint32_t descriptor_write_count, const VkWriteDescriptorSet* p_descriptor_writes,
                                               uint32_t descriptor_copy_count, const VkCopyDescriptorSet* p_descriptor_copies ) const
        {
            vkUpdateDescriptorSets( device_handle_, descriptor_write_count, p_descriptor_writes, descriptor_copy_count, p_descriptor_copies );
        }

        VkDescriptorPool
        logical_device::create_descriptor_pool( VkDescriptorPoolCreateInfo& create_info ) const
        {
            VkDescriptorPool pool_handle;

            if( vkCreateDescriptorPool( device_handle_, &create_info, nullptr, &pool_handle ) != VK_SUCCESS )
                std::cerr << "Failed to create Descriptor Pool." << std::endl;

            return pool_handle;
        }
        VkDescriptorPool
        logical_device::destroy_descriptor_pool( VkDescriptorPool& descriptor_pool_handle ) const
        {
            vkDestroyDescriptorPool( device_handle_, descriptor_pool_handle, nullptr );

            return VK_NULL_HANDLE;
        }
    }
}