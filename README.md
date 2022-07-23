# Incenerate - Vulkan based renderer

## About

I started this project for improve my skills in c++ and graphics technologies. I working in game development, but my duties do not include work with graphics in low level. In this project i developing render system based on OpenGL and Vulkan API. I don't think that i can call it as "engine", because it includes only graphics rendering. In future i'll add more features, and this "engine" will become to GLTF scene editor/renderer. "Engine" support only one mesh format(GLTF), and three formats for textures(KTX, KTX2(basisu), DDS).
 
### Most interesting resources

Must read resource: https://vulkan-tutorial.com/ and code from this tutorial https://github.com/Overv/VulkanTutorial

Repository with hello triangle example from previous resource but using Vulkan hpp: https://github.com/bwasty/vulkan-tutorial-hpp

Vulkan engine implementation with explanation on youtube: https://www.youtube.com/watch?v=lr93-_cC8v4&t=1s&ab_channel=BrendanGalea  and repo https://github.com/blurrypiano/littleVulkanEngine

SachaWilliems examples. To really hard, explore it after previous resources: https://github.com/SaschaWillems/Vulkan

Vulkan programming guide book. To consolidate the material and study the subtleties of working with Vulkan.

Game engine architecture. Goog book for creating not bad game engine architecture.

### Features
- [x] Vulkan API 1.3+
- [ ] OpenGL 3+
- [x] ImGui integration
- [x] GLTF support
- [x] KTX support
- [x] KTX2 support
- [ ] DDS support
- [x] Deferred shading
- [x] All light types (Directional light, point light spot light)
- [x] HDR
- [ ] Bloom
- [x] PBR(UE4)
- [x] IBL
- [x] Compute shaders
- [x] Multithreading
- [ ] Multisampling
- [x] Dynamic directional shadows(for spot light, max 15)
- [x] Omni shadows (single pass, max 16)
- [ ] Cascade shadows(Only for directional, max 1)
- [ ] Realtime reflections
- [ ] Instancing
- [x] FXAA (final image)
- [x] Tescellation
- [ ] SSAO
- [x] Parallax mapping
- [ ] Global illumination

### Screenshoots
![directional shadows](/assets/screenshoots/first_shadow_test.png "Directional shadows(Spotlights)")