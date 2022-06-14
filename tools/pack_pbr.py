import os, sys
from PIL import Image

roughmetal_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\sponza\\textures\\ex\\upper_rounded_columns_mr.jpg"
# metalness_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\sponza\\textures\\ex\\flag_rods_metal.png"
ao_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\sponza\\textures\\ex\\upper_rounded_columns_occlusion.jpg"
height_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\sponza\\textures\\ex\\upper_rounded_columns_height.jpg"
output_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\sponza\\textures\ex\\upper_rounded_columns_rmah.png"

def main():
    empty = Image.new("RGB", (1024, 1024), (0, 0, 0))
    er, eg, eb = empty.split()

    roughness = er
    metallic = er
    occlusion = er
    height = er

    if os.path.exists(roughmetal_texture):
        rough_tex = Image.open(roughmetal_texture)
        roughness = rough_tex.split()[1]
        metallic = rough_tex.split()[2]

    # if os.path.exists(metalness_texture):
    #     metal_tex = Image.open(metalness_texture)
    #     metallic = metal_tex.split()[0]

    if os.path.exists(ao_texture):
        ao_tex = Image.open(ao_texture)
        occlusion = ao_tex.split()[0]

    if os.path.exists(height_texture):
        height_tex = Image.open(height_texture)
        height = height_tex.split()[0]

    im = Image.merge("RGBA", (roughness, metallic, occlusion, height))
    im.save(output_texture)

if __name__ == "__main__":
    main()