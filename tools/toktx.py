import subprocess, os, fnmatch

# cubemap toktx --t2 --uastc 4 --srgb --zcmp 20 --cubemap DayInTheClouds4k.ktx2 px.jpg nx.jpg py.jpg ny.jpg pz.jpg nz.jpg
# target_dir = "../assets/meshes"
target_dir = "F:\\materials\\metals\\worn-metal4-bl"
target_ext = []
uastc_quality = 4
compression_level = 20

def main():
    for root, dir, files in os.walk(target_dir):
        for file in files:
            if file.endswith((".png", ".jpg")):
                file_path = os.path.abspath(os.path.normpath(os.path.join(root, file)))
                outfile_path = os.path.join(root, "basis_universal")
                if not os.path.exists(outfile_path):
                    os.makedirs(outfile_path)
                outfile_path = os.path.join(outfile_path, os.path.splitext(file)[0] + '.ktx2')
                print("Working on: ", os.path.basename(file_path))

                packer_args = ["toktx", "--t2", f"--uastc", f"{uastc_quality}"]
                additional_args = ["--srgb"]
                if "normal" in file_path:
                    additional_args = ["--linear", "--normal_map"]
                
                packer_args.extend(additional_args)
                packer_args.extend(["--genmipmap", "--zcmp", f"{compression_level}", f"{outfile_path}", f"{file_path}"])

                subprocess.Popen(packer_args).wait()

if __name__ == "__main__":
    main()