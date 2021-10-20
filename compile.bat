@echo off
glslangValidator.exe -V assets/shaders/diffuse.vert -o assets/shaders/diffuse_vert.spv
glslangValidator.exe -V assets/shaders/diffuse.frag -o assets/shaders/diffuse_frag.spv
glslangValidator.exe -V assets/shaders/ui.vert -o assets/shaders/ui_vert.spv
glslangValidator.exe -V assets/shaders/ui.frag -o assets/shaders/ui_frag.spv
glslangValidator.exe -V assets/shaders/skybox/skybox.vert -o assets/shaders/skybox/skybox_vert.spv
glslangValidator.exe -V assets/shaders/skybox/skybox.frag -o assets/shaders/skybox/skybox_frag.spv
glslangValidator.exe -V assets/shaders/pbr/pbr_base.vert -o assets/shaders/pbr/pbr_base_vert.spv
glslangValidator.exe -V assets/shaders/pbr/pbr_base.frag -o assets/shaders/pbr/pbr_base_frag.spv
pause