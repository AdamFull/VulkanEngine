import os, sys
from PIL import Image

roughness_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\WornShiny\\textures\\worn-shiny-metal-Roughness.png"
metalness_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\WornShiny\\textures\\worn-shiny-metal-Metallic.png"
ao_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\WornShiny\\textures\\worn-shiny-metal-ao.png"
height_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\WornShiny\\textures\\worn-shiny-metal-Height.png"
output_texture = "C:\\Users\\logot\\Documents\\GitHub\\VulkanEngine\\assets\\meshes\\WornShiny\\textures\\worn-shiny-metal_rmah.png"

def main():
    empty = Image.new("RGB", (2048, 2048), (0, 0, 0))
    er, eg, eb = empty.split()

    roughness = er
    metallic = er
    occlusion = er
    height = er

    if os.path.exists(roughness_texture):
        rough_tex = Image.open(roughness_texture)
        roughness = rough_tex.split()[0]

    if os.path.exists(metalness_texture):
        metal_tex = Image.open(metalness_texture)
        metallic = metal_tex.split()[0]

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