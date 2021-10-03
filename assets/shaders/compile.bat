@echo off
glslangValidator.exe -V diffuse.vert -o diffuse_vert.spv
glslangValidator.exe -V diffuse.frag -o diffuse_frag.spv
glslangValidator.exe -V ui.vert -o ui_vert.spv
glslangValidator.exe -V ui.frag -o ui_frag.spv
pause