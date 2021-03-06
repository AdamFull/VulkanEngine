#pragma once
#include "serializer/Serialization.hpp"

namespace engine
{
	namespace core
	{
		namespace pipeline
		{
			struct FShaderCache
			{
				struct FCachedShader
				{
					vk::ShaderStageFlagBits shaderStage;
					std::vector<uint32_t> shaderCode;
					std::string hash{};

					bool operator==(const FCachedShader &rhs) const
					{
						return this->shaderStage == rhs.shaderStage && this->shaderCode == rhs.shaderCode;
					}

					bool operator!=(const FCachedShader &rhs) const
					{
						return this->shaderStage != rhs.shaderStage || this->shaderCode != rhs.shaderCode;
					}
				};

				std::map<std::string, FCachedShader> cache;
			};

			void to_json(nlohmann::json &json, const FShaderCache::FCachedShader &type);
			void from_json(const nlohmann::json &json, FShaderCache::FCachedShader &type);

			void to_json(nlohmann::json &json, const FShaderCache &type);
			void from_json(const nlohmann::json &json, FShaderCache &type);

			/**
			 * @brief Shader cahce class. Helping to load already builded shaders
			 *
			 */
			class CShaderCache : public utl::singleton<CShaderCache>
			{
			public:
				CShaderCache();

				/**
				 * @brief Add new shader to cache
				 *
				 * @param name Shader name
				 * @param stage Shader pipeline stage
				 * @param code Shader spir-v code
				 * @param moduleCode Shader glsl code
				 */
				void add(const std::string &name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t> &code, const std::string &moduleCode);

				/**
				 * @brief Updates shader if already exists or when shader was changed
				 *
				 * @param name Shader name
				 * @param code Shader spir-v code
				 * @param hash Shader hash
				 */
				void update(const std::string &name, const std::vector<uint32_t> &code, const std::string &hash);

				/**
				 * @brief Get shader from cache when shader exists
				 *
				 * @param name
				 * @param moduleCode
				 * @return std::optional<FShaderCache::FCachedShader>
				 */
				std::optional<FShaderCache::FCachedShader> get(const std::string &name, const std::string &moduleCode);

			private:
				/**
				 * @brief Load cached shaders
				 *
				 */
				void load();

				/**
				 * @brief Save shader cache
				 *
				 */
				void save();

				std::hash<std::string> hasher;
				FShaderCache cacheDTO;
			};
		}
	}
}