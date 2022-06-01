#pragma once
#include "Shader.h"

namespace engine
{
	namespace core
	{
		namespace pipeline
		{
			/**
			 * @brief Vertex input data container class
			 *
			 */
			class CVertexInput
			{
			public:
				CVertexInput() = default;
				CVertexInput(vk::VertexInputBindingDescription inputDesc, std::vector<vk::VertexInputAttributeDescription> atribDesc) : vertexInputDesc(inputDesc), vertexAtribDesc(atribDesc)
				{
				}
				inline const vk::VertexInputBindingDescription &getInputBindingDescription() { return vertexInputDesc; }
				inline const std::vector<vk::VertexInputAttributeDescription> &getInputAttributeDescription() { return vertexAtribDesc; }

			private:
				vk::VertexInputBindingDescription vertexInputDesc;
				std::vector<vk::VertexInputAttributeDescription> vertexAtribDesc;
			};

			class CShader;

			/**
			 * @brief Base pipeline class
			 *
			 */
			class CPipelineBase
			{
			public:
				friend class Builder;
				/**
				 * @brief Pipeline builder class
				 *
				 */
				class Builder
				{
				public:
					Builder() = default;
					inline Builder &setVertexInput(CVertexInput &&vertexInput)
					{
						m_vertexInput = std::move(vertexInput);
						return *this;
					}
					inline Builder &setColorAttachments(uint32_t attachments)
					{
						m_colorAttachments = attachments;
						return *this;
					}
					inline Builder &setCulling(vk::CullModeFlagBits culling)
					{
						m_culling = culling;
						return *this;
					}
					inline Builder &setFontFace(vk::FrontFace fontface)
					{
						m_fontface = fontface;
						return *this;
					}
					inline Builder &setDepthEnabled(vk::Bool32 enableDepth)
					{
						m_enableDepth = enableDepth;
						return *this;
					}
					inline Builder &addDynamicState(vk::DynamicState state)
					{
						m_vDynamicStateEnables.emplace_back(state);
						return *this;
					}
					inline Builder &setDynamicStates(const std::vector<vk::DynamicState> &states)
					{
						m_vDynamicStateEnables = states;
						return *this;
					}
					inline Builder &addShaderStage(const std::string &stage)
					{
						m_vStages.emplace_back(stage);
						return *this;
					}
					inline Builder &setShaderStages(const std::vector<std::string> &stages)
					{
						m_vStages = stages;
						return *this;
					}
					inline Builder &addDefine(const std::string &name, const std::string &value)
					{
						m_vDefines.emplace(std::make_pair(name, value));
						return *this;
					}
					inline Builder &setDefines(const std::map<std::string, std::string> &defines)
					{
						m_vDefines = defines;
						return *this;
					}
					scope_ptr<CPipelineBase> build(vk::PipelineBindPoint bindPoint);

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

				/**
				 * @brief Create pipeline with render pass and subpass
				 *
				 * @param renderPass Vulkan render pass
				 * @param subpass Vulkan subpass
				 */
				virtual void create(vk::RenderPass &renderPass, uint32_t subpass);

				/**
				 * @brief Create pipeline without using graphics instances
				 *
				 */
				virtual void create();

				/**
				 * @brief Bind pipeline to command buffer
				 *
				 * @param commandBuffer Current command buffer
				 */
				void bind(vk::CommandBuffer &commandBuffer);

				/**
				 * @brief Load shader method
				 *
				 * @param vShaders Vector with shader names
				 */
				virtual void loadShader(const std::vector<std::string> &vShaders);

				/**
				 * @brief ReCreating pipeline woth new render pass and subpass
				 *
				 * @param renderPass Vulkan render pass
				 * @param _subpass Vulkan subpass
				 */
				virtual void reCreate(vk::RenderPass &renderPass, uint32_t _subpass);

				/**
				 * @brief Clean all pipeline instances before deletion
				 *
				 */
				virtual void cleanup();

				/**
				 * @brief Get pipeline state object
				 *
				 * @return vk::Pipeline& Vulkan pso
				 */
				inline virtual vk::Pipeline &getPipeline() { return pipeline; }

				/**
				 * @brief Get pipeline bind point
				 *
				 * @return vk::PipelineBindPoint& Bind point
				 */
				inline virtual vk::PipelineBindPoint &getBindPoint() { return m_bindPoint; }

				/**
				 * @brief Get pipeline descriptor set layout
				 *
				 * @return vk::DescriptorSetLayout& Descriptor set layout
				 */
				inline vk::DescriptorSetLayout &getDescriptorSetLayout() { return descriptorSetLayout; }

				/**
				 * @brief Get descriptor pool
				 *
				 * @return vk::DescriptorPool& Vulkan descriptor pool
				 */
				inline vk::DescriptorPool &getDescriptorPool() { return descriptorPool; }

				/**
				 * @brief Get pipeline layout
				 *
				 * @return vk::PipelineLayout& Vulkan pipeline layout
				 */
				inline vk::PipelineLayout &getPipelineLayout() { return pipelineLayout; }

				/**
				 * @brief Get shader object
				 *
				 * @return scope_ptr<CShader>& Shader smart pointer
				 */
				inline scope_ptr<CShader> &getShader() { return m_pShader; }

			protected:
				/**
				 * @brief Creates desctiptor set for pipeline
				 *
				 */
				void createDescriptorSetLayout();

				/**
				 * @brief Creates descriptor pool for pipeline
				 *
				 */
				void createDescriptorPool();

				/**
				 * @brief Creates pipeline layout
				 *
				 */
				void createPipelineLayout();

				/**
				 * @brief Creates a pipeline object. Should be overloaded in specific child
				 *
				 */
				virtual void createPipeline() {}

				/**
				 * @brief Re creating shaders from compiled source
				 *
				 */
				virtual void recreateShaders();

				std::vector<std::string> vShaderCache;
				std::map<std::string, std::string> m_vDefines;
				scope_ptr<CShader> m_pShader;
				bool bIsClean{false};

				CVertexInput m_vertexInput;

				vk::DescriptorSetLayout descriptorSetLayout{nullptr};
				vk::DescriptorPool descriptorPool{nullptr};
				vk::PipelineLayout pipelineLayout{nullptr};

				vk::RenderPass m_renderPass{nullptr};
				vk::PipelineBindPoint m_bindPoint;
				uint32_t m_colorAttachments;
				vk::CullModeFlagBits m_culling;
				vk::FrontFace m_fontface;
				vk::Bool32 m_enableDepth;
				std::vector<vk::DynamicState> m_vDynamicStateEnables;
				std::vector<std::string> m_vStages;
				uint32_t subpass{0};

				vk::Pipeline pipeline;
			};
		}
	}
}