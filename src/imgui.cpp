//
// Created by nic on 26/06/2021.
//

#include "imgui.h"


void ImGUIRenderer::createFontTexture(const VkDevice device, const VkCommandPool command_pool) {
    unsigned char *data = nullptr;
    int width = 0;
    int height = 0;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&data, &width, &height);
    auto data_size = static_cast<uint32_t>(width * height * 4);
    
    VkImageCreateInfo font_image_create_info = {};
    font_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    font_image_create_info.imageType = VK_IMAGE_TYPE_2D;
    font_image_create_info.extent = {(uint32_t) width, (uint32_t) height, 1};
    font_image_create_info.mipLevels = 1;
    font_image_create_info.arrayLayers = 1;
    font_image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    font_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    font_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    font_image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    font_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    
    
    
    ImGui::GetIO().Fonts->SetTexID((ImTextureID)(intptr_t)m_font_texture->GetHandle());
    
    {
        std::shared_ptr<myvk::Fence> fence = myvk::Fence::Create(command_pool->GetDevicePtr());
        std::shared_ptr<myvk::Buffer> staging_buffer =
            myvk::Buffer::CreateStaging(command_pool->GetDevicePtr(), data_size);
        staging_buffer->UpdateData(data, data + data_size);
        
        std::shared_ptr<myvk::CommandBuffer> command_buffer = myvk::CommandBuffer::Create(command_pool);
        command_buffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {(uint32_t)width, (uint32_t)height, 1};
        
        command_buffer->CmdPipelineBarrier(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, {}, {},
            m_font_texture->GetDstMemoryBarriers({region}, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL));
        command_buffer->CmdCopy(staging_buffer, m_font_texture, {region});
        command_buffer->CmdPipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, {}, {},
            m_font_texture->GetDstMemoryBarriers({region}, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        
        command_buffer->End();
        
        command_buffer->Submit(fence);
        fence->Wait();
}

void ImGUIRenderer::createDescriptor(const VkDevice device) {

}

void ImGUIRenderer::createPipeline(const VkRenderPass render_pass, uint32_t subpass) {

}

void ImGUIRenderer::setupRenderState(const VkCommandBuffer command_buffer,
                                     int fb_width,
                                     int fb_height,
                                     uint32_t current_frame) const {
    
}

ImGUIRenderer::ImGUIRenderer(VkDevice device,
                             VkCommandPool command_pool,
                             VkRenderPass render_pass,
                             uint32_t subpass,
                             uint32_t frame_count) {
    createFontTexture(device, command_pool);
    createDescriptor(device);
    createPipeline(render_pass, subpass);
    vertexBuffers.resize(frame_count);
    indexBuffers.resize(frame_count);
}

void ImGUIRenderer::CmdDrawPipeline(const VkCommandBuffer command_buffer, uint32_t current_frame) {

}
