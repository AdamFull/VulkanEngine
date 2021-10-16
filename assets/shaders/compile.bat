@echo off
glslangValidator.exe -V diffuse.vert -o diffuse_vert.spv
glslangValidator.exe -V diffuse.frag -o diffuse_frag.spv
glslangValidator.exe -V ui.vert -o ui_vert.spv
glslangValidator.exe -V ui.frag -o ui_frag.spv
glslangValidator.exe -V skybox/skybox.vert -o skybox/skybox_vert.spv
glslangValidator.exe -V skybox/skybox.frag -o skybox/skybox_frag.spv
glslangValidator.exe -V pbr/pbr_base.vert -o pbr/pbr_base_vert.spv
glslangValidator.exe -V pbr/pbr_base.frag -o pbr/pbr_base_frag.spv
pause