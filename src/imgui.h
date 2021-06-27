#pragma once

#ifndef MANDELBROTPROJECT_SRC_IMGUI_H_
#define MANDELBROTPROJECT_SRC_IMGUI_H_

#include <vulkan/vulkan.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include <vector>


class ImGUIRenderer {
  private:
    VkImage fontTexture;
    VkImageView fontTextureView;
    VkSampler fontTextureSampler;
    
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    
    std::vector<VkBuffer> indexBuffers, vertexBuffers;
    
    void createFontTexture(const VkDevice device, const VkCommandPool command_pool);
    
    void createDescriptor(const VkDevice device);
    
    void createPipeline(const VkRenderPass render_pass, uint32_t subpass);
    
    void setupRenderState(const VkCommandBuffer command_buffer, int fb_width, int fb_height,
                          uint32_t current_frame) const;
    
  public:
    ImGUIRenderer(VkDevice device, VkCommandPool command_pool, VkRenderPass render_pass, uint32_t subpass,
                  uint32_t frame_count);
    
    void CmdDrawPipeline(const VkCommandBuffer command_buffer, uint32_t current_frame);
};

#endif //MANDELBROTPROJECT_SRC_IMGUI_H_
