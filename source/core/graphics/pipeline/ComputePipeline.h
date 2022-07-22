#pragma once
#include "Pipeline.h"

namespace engine
{
	namespace core
	{
		namespace pipeline
		{
			/**
			 * @brief Compite pipeline class. Just contain data for creation compute pipeline
			 *
			 */
			class CComputePipeline : public CPipelineBase
			{
			public:
				CComputePipeline() = default;

				void create() override;

			protected:
				void createPipeline() override;
			};
		}
	}
}