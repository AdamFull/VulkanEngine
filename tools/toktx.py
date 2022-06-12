import subprocess, os, fnmatch

# cubemap toktx --t2 --uastc 4 --srgb --genmipmap --zcmp 20 --cubemap MegaSun4k.ktx2 px.png nx.png py.png ny.png pz.png nz.png
target_dir = "../assets"
target_ext = "*.png"
uastc_quality = 4
compression_level = 20

def main():
    for root, dir, files in os.walk(target_dir):
        for file in fnmatch.filter(files, target_ext):
            file_path = os.path.abspath(os.path.normpath(os.path.join(root, file)))
            outfile_path = os.path.splitext(file_path)[0] + '.ktx2'

            packer_args = ["toktx", "--t2", f"--uastc", f"{uastc_quality}"]
            additional_args = ["--srgb"]
            if "normal" in file_path:
                additional_args = ["--linear", "--normal_map"]
            
            packer_args.extend(additional_args)
            packer_args.extend(["--genmipmap", "--zcmp", f"{compression_level}", f"{outfile_path}", f"{file_path}"])

            subprocess.Popen(packer_args).wait()

if __name__ == "__main__":
    main()