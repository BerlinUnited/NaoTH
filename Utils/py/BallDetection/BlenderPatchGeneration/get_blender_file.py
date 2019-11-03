from pywget import wget
from pathlib import Path


if __name__ == "__main__":
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/Blender/"
    blender_file = "RoboCup_v_1_2_1_nonInteractive.blend"

    target_dir = Path(".")

    if not Path(target_dir / blender_file).is_file():
        wget.download(base_url + blender_file, target_dir)
