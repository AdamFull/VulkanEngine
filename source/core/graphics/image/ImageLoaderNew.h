#pragma once
#include "PixelFormat.h"
#include "filesystem/FilesystemHelper.h"

namespace engine
{
	namespace core
	{
        enum class EImageFormat
		{
            eUnknown,

			eKTX,
			eKTX2,
			eDDS
		};

        struct FImageCreateInfo
        {
            bool isArray{false};
            bool isCubemap{false};
            bool isCompressed{false};
            bool generateMipmaps{false};

            uint32_t baseWidth{0U};
            uint32_t baseHeight{0U};
            uint32_t baseDepth{0U};
            uint32_t numDimensions{0U};
            uint32_t numLevels{0U};
            uint32_t numLayers{0U};
            uint32_t numFaces{0U};

            EPixelFormat pixFormat{};

            std::map<uint32_t, std::vector<size_t>> mipOffsets;

            size_t dataSize{0UL};
            utl::scope_ptr<uint8_t[]> pData{nullptr};
        };

		class CImageLoaderNew
		{
		public:
            static void load(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI, const std::vector<EPixelFormat>& supportedFormats, const fs::path &parentPath = FilesystemHelper::getWorkDir());
        private:
            static void loadKTX(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI);
            static void loadKTX2(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI, const std::vector<EPixelFormat>& supportedFormats);
            static void loadDDS(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI);
            static void imageCreateInfoDeleter();
            static EImageFormat getTextureFormat(const fs::path& fsPath);
		};
	}
}
