@echo off
glslangValidator.exe -V diffuse.vert -o diffuse_vert.spv
glslangValidator.exe -V diffuse.frag -o diffuse_frag.spv
glslangValidator.exe -V ui.vert -o ui_vert.spv
glslangValidator.exe -V ui.frag -o ui_frag.spv
glslangValidator.exe -V skybox/skybox.vert -o skybox/skybox_vert.spv
glslangValidator.exe -V skybox/skybox.frag -o skybox/skybox_frag.spv
glslangValidator.exe -V skybox/reflect.vert -o skybox/reflect_vert.spv
glslangValidator.exe -V skybox/reflect.frag -o skybox/reflect_frag.spv
pause