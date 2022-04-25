#pragma once

namespace Core
{
    namespace Render
    {
        class CAttachment
        {
        public:
            enum class EType 
            {
                eColor, eDepth, eKHR
            };

            CAttachment(uint32_t binding, std::string name, EType type, bool multisampled = false, vk::Format format = vk::Format::eR8G8B8A8Unorm,
                vk::ClearValue clearColour = {}) :
                binding(binding),
                name(std::move(name)),
                type(type),
                multisampled(multisampled),
                format(format),
                clearColour(clearColour) {
            }

            uint32_t GetBinding() const { return binding; }
            const std::string &GetName() const { return name; }
            EType GetType() const { return type; }
            bool IsMultisampled() const { return multisampled; }
            vk::Format GetFormat() const { return format; }
            const vk::ClearValue &GetClearColour() const { return clearColour; }
        private:
            uint32_t binding;
            std::string name;
            EType type;
            bool multisampled;
            vk::Format format;
            vk::ClearValue clearColour;
        };

        class CSubpassType
        {
        public:
            CSubpassType(uint32_t binding, std::vector<uint32_t> attachmentBindings) : 
            binding(binding),
            attachmentBindings(std::move(attachmentBindings))
            {
            }

            uint32_t GetBinding() const { return binding; }
            const std::vector<uint32_t> &GetAttachmentBindings() const { return attachmentBindings; }

        private:
            uint32_t binding;
            std::vector<uint32_t> attachmentBindings;
        };

        class CRenderArea 
        {
        public:
            explicit CRenderArea(const glm::vec2 &extent = {}, const glm::vec2 &offset = {}) :
            extent(extent),
            offset(offset) 
            {
            }

            bool operator==(const CRenderArea &rhs) const {
                return extent == rhs.extent && offset == rhs.offset;
            }

            bool operator!=(const CRenderArea &rhs) const {
                return !operator==(rhs);
            }

            const glm::vec2 &GetExtent() const { return extent; }
            void SetExtent(const glm::vec2 &extent) { this->extent = extent; }

            const glm::vec2 &GetOffset() const { return offset; }
            void SetOffset(const glm::vec2 &offset) { this->offset = offset; }

            float GetAspectRatio() const { return aspectRatio; }
            void SetAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }

        private:
            glm::vec2 extent;
            glm::vec2 offset;
            float aspectRatio = 1.0f;
        };

        class CViewport 
        {
        public:
            CViewport() = default;

            explicit CViewport(const glm::vec2 &size) :
                size(size) {
            }

            const glm::vec2 &GetScale() const { return scale; }
            void SetScale(const glm::vec2 &scale) { this->scale = scale; }

            const std::optional<glm::vec2> &GetSize() const { return size; }
            void SetSize(const std::optional<glm::vec2> &size) { this->size = size; }

            const glm::vec2 &GetOffset() const { return offset; }
            void SetOffset(const glm::vec2 &offset) { this->offset = offset; }

        private:
            glm::vec2 scale = {1.0f, 1.0f};
            std::optional<glm::vec2> size;
            glm::vec2 offset;
        };
    }
}