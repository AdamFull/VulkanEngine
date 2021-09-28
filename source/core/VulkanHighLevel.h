#pragma once
#include "Objects/Components/SceneRootComponent.h"

namespace Engine
{
    class WindowHandle;
    class Device;
    class SwapChain;
    class UniformBuffer;
    class Renderer;

    class VulkanHighLevel
    {
    public:
        VulkanHighLevel() = default;
        ~VulkanHighLevel();

        void Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);

        void AddVulkanMesh(std::string);

        void BeginFrame(float);
        //TODO: Dont forget about clean textures
        void Cleanup();

        void ValidateRunAbility();

        template<class T>
        std::shared_ptr<T> CreateDefaultSubobject(std::string srName)
        {
            return std::make_shared<T>(srName);
        }

        void Attach(std::shared_ptr<RenderObject> object)
        {
            object->SetParent(m_pRoot);
        }

        void CreateObjects()
        {
            m_pRoot->Create(m_pDevice, m_pSwapChain, m_pUniform);
        }

        //Getters

        inline std::unique_ptr<Device>& GetDevice() { return m_pDevice; }
        inline std::shared_ptr<SwapChain> GetSwapChain() { return m_pSwapChain; }
        inline std::shared_ptr<UniformBuffer>  GetUniformBuffer() { return m_pUniform; }
        inline std::shared_ptr<Renderer>  GetRenderer() { return m_pRenderer; }
    private:
        void RecreateSwapChain();
        void CleanupSwapChain();

        //Device
        std::unique_ptr<Device> m_pDevice;
        //SwapChain
        std::shared_ptr<SwapChain> m_pSwapChain;

        //Descriptors
        std::shared_ptr<UniformBuffer> m_pUniform;

        std::shared_ptr<Renderer> m_pRenderer;

        std::shared_ptr<SceneRootComponent> m_pRoot;

        float m_fDeltaTime{0.0f};
    };
}
