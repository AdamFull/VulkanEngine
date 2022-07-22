import os, sys
from PIL import Image
from PIL.Image import Resampling

base_path = "F:\\materials\\wood"

# occlusion_texture = os.path.join(base_path, "broken_down_concrete1_bl\\broken_down_concrete1_ao.png")
# roughness_texture = os.path.join(base_path, "broken_down_concrete2_bl\\broken_down_concrete2_Roughness.png")
# metalness_texture = os.path.join(base_path, "broken_down_concrete2_bl\\broken_down_concrete2_Metallic.png")
# output_texture = os.path.join(base_path, "broken_down_concrete2_bl\\broken_down_concrete2_metalRough.jpg")

image_size = (2048, 2048)

def getImageChannel(path_to_image):
    texture = Image.open(path_to_image)
    if texture.mode != 'L':
        texture = texture.convert('L')
        print("Changed mode to L")
    if texture.size != image_size:
        if texture.size > image_size:
            texture.thumbnail(image_size, Resampling.LANCZOS)
        else:
            texture = texture.resize(image_size)
        print("Resized to 2048x2048")
    return texture.split()[0]

def main():
    empty = Image.new("RGB", image_size, (0, 0, 0))
    er, eg, eb = empty.split()

    roughness = er
    metallic = er
    occlusion = er
    height = er

    for root, dir, files in os.walk(base_path):
        material_name = os.path.basename(root)
        delete_list = list()
        for file in files:
            full_file_path = os.path.join(root, file)
            if os.path.exists(full_file_path):
                if ("metallic" in file.lower()) or ("metalness" in file.lower()):
                    metallic = getImageChannel(full_file_path)
                    delete_list.append(full_file_path)
                elif ("roughness" in file.lower()) or ("rough" in file.lower()):
                    roughness = getImageChannel(full_file_path)
                    delete_list.append(full_file_path)
                elif "preview" in file.lower():
                    delete_list.append(full_file_path)
                else:
                    if file.endswith((".txt", ".ini")):
                        delete_list.append(full_file_path)

        if metallic == er and roughness == er:
            print("No metallic/roughness found")
            continue

        output_texture = os.path.join(root, material_name + "_metalRough.jpg")
        print("Saving new texture: ", output_texture)
        im = Image.merge("RGB", (occlusion, roughness, metallic))
        im.save(output_texture)

        for to_delete in delete_list:
            os.remove(to_delete)
            print("Deleted", to_delete)


    # if os.path.exists(roughness_texture):
    #     rough_tex = Image.open(roughness_texture)
    #     roughness = rough_tex.split()[0]
    # if os.path.exists(metalness_texture):
    #     metal_tex = Image.open(metalness_texture)
    #     metallic = metal_tex.split()[0]

if __name__ == "__main__":
    main()