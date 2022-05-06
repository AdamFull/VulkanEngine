#pragma once
#include "Shader.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class CVertexInput
            {
            public:
                CVertexInput() = default;
                CVertexInput(vk::VertexInputBindingDescription inputDesc, std::vector<vk::VertexInputAttributeDescription> atribDesc) :
                vertexInputDesc(inputDesc), vertexAtribDesc(atribDesc)
                {}
                inline const vk::VertexInputBindingDescription& getInputBindingDescription() { return vertexInputDesc; }
                inline const std::vector<vk::VertexInputAttributeDescription>& getInputAttributeDescription() { return vertexAtribDesc; }
            private:
                vk::VertexInputBindingDescription vertexInputDesc;
                std::vector<vk::VertexInputAttributeDescription> vertexAtribDesc;
            };

            class CShader;
            class CPipelineBase
            {
            public:
                friend class Builder;
                class Builder
                {
                public:
                    Builder() = default;
                    inline Builder& setVertexInput(CVertexInput&& vertexInput) { m_vertexInput = std::move(vertexInput); return *this; }
                    inline Builder& setColorAttachments(uint32_t attachments) { m_colorAttachments = attachments; return *this; }
                    inline Builder& setCulling(vk::CullModeFlagBits culling) { m_culling = culling; return *this; }
                    inline Builder& setFontFace(vk::FrontFace fontface) { m_fontface = fontface; return *this; }
                    inline Builder& setDepthEnabled(vk::Bool32 enableDepth) { m_enableDepth = enableDepth; return *this; }
                    inline Builder& addDynamicState(vk::DynamicState state) { m_vDynamicStateEnables.emplace_back(state); return *this; }
                    inline Builder& setDynamicStates(const std::vector<vk::DynamicState>& states) { m_vDynamicStateEnables = states; return *this; }
                    inline Builder& addShaderStage(const std::string& stage) { m_vStages.emplace_back(stage); return *this; }
                    inline Builder& setShaderStages(const std::vector<std::string>& stages) { m_vStages = stages; return *this; }
                    inline Builder& addDefine(const std::string& name, const std::string& value) { m_vDefines.emplace(std::make_pair(name, value)); return *this; }
                    inline Builder& setDefines(const std::map<std::string, std::string>& defines) { m_vDefines = defines; return *this; }
                    std::unique_ptr<CPipelineBase> build(vk::PipelineBindPoint bindPoint);
                private:
                    CVertexInput m_vertexInput;
                    uint32_t m_colorAttachments{1};
                    vk::CullModeFlagBits m_culling{vk::CullModeFlagBits::eNone};
                    vk::FrontFace m_fontface{vk::FrontFace::eCounterClockwise};
                    vk::Bool32 m_enableDepth{VK_FALSE};
                    std::vector<vk::DynamicState> m_vDynamicStateEnables{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
                    std::vector<std::string> m_vStages;
                    std::map<std::string, std::string> m_vDefines;
                };
                CPipelineBase() = default;
                virtual ~CPipelineBase();

                virtual void create(vk::RenderPass& renderPass, uint32_t subpass);
                virtual void create();

                void bind(vk::CommandBuffer &commandBuffer);

                virtual void loadShader(const std::vector<std::string> &vShaders);
                virtual void recreatePipeline();

                virtual void cleanup();

                inline virtual vk::Pipeline &getPipeline() { return m_pipeline; }
                inline virtual vk::PipelineBindPoint &getBindPoint() { return m_bindPoint; }

                inline vk::DescriptorSetLayout& getDescriptorSetLayout() { return m_descriptorSetLayout; }
                inline vk::DescriptorPool& getDescriptorPool() { return m_descriptorPool; }
                inline vk::PipelineLayout& getPipelineLayout() { return m_pipelineLayout; }

                inline std::unique_ptr<CShader>& getShader() { return m_pShader; }

            protected:
                void createDescriptorSetLayout();
                void createDescriptorPool();
                void createPipelineLayout();

                virtual void createPipeline() {}
                virtual void recreateShaders();

                std::vector<std::string> m_vShaderCache;
                std::map<std::string, std::string> m_vDefines;
                std::unique_ptr<CShader> m_pShader;

                CVertexInput m_vertexInput;

                vk::DescriptorSetLayout m_descriptorSetLayout{nullptr};
                vk::DescriptorPool m_descriptorPool{nullptr};
                vk::PipelineLayout m_pipelineLayout{nullptr};

                vk::RenderPass m_renderPass{nullptr};
                vk::PipelineBindPoint m_bindPoint;
                uint32_t m_colorAttachments;
                vk::CullModeFlagBits m_culling;
                vk::FrontFace m_fontface;
                vk::Bool32 m_enableDepth;
                std::vector<vk::DynamicState> m_vDynamicStateEnables;
                std::vector<std::string> m_vStages;
                uint32_t subpass{0};

                vk::Pipeline m_pipeline;
            };
        }
    }
};