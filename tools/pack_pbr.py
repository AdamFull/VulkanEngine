import os, sys
from PIL import Image

roughness_texture = "C:\\Users\\logot\\Documents\\GitHub\VulkanEngine\\assets\\meshes\\SimpleScene\\textures\\tidal-pool2\\tidalpool2_roughness.png"
metalness_texture = "C:\\Users\\logot\\Documents\\GitHub\VulkanEngine\\assets\\meshes\\SimpleScene\\textures\\tidal-pool2\\tidalpool2_metallic.png"
output_texture = "C:\\Users\\logot\\Documents\\GitHub\VulkanEngine\\assets\\meshes\\SimpleScene\\textures\\tidal-pool2\\tidal-pool2_metalRough.jpg"

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
        

    im = Image.merge("RGB", (occlusion, roughness, metallic))
    im.save(output_texture)

if __name__ == "__main__":
    main()