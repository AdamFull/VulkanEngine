@echo off
glslangValidator.exe -V assets/shaders/source/diffuse.vert -o assets/shaders/compiled/diffuse/vert.spv
glslangValidator.exe -V assets/shaders/source/diffuse.frag -o assets/shaders/compiled/diffuse/frag.spv
glslangValidator.exe -V assets/shaders/source/ui.vert -o assets/shaders/compiled/ui/vert.spv
glslangValidator.exe -V assets/shaders/source/ui.frag -o assets/shaders/compiled/ui/frag.spv
glslangValidator.exe -V assets/shaders/source/skybox.vert -o assets/shaders/compiled/skybox/vert.spv
glslangValidator.exe -V assets/shaders/source/skybox.frag -o assets/shaders/compiled/skybox/frag.spv
glslangValidator.exe -V assets/shaders/source/pbr.vert -o assets/shaders/compiled/pbr/vert.spv
glslangValidator.exe -V assets/shaders/source/pbr.frag -o assets/shaders/compiled/pbr/frag.spv
pause