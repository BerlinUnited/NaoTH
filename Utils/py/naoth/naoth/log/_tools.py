from pywget import wget
from pathlib import Path


def get_demo_logfiles(base_url, logfile_list):

    print("Downloading Logfiles: {}".format(logfile_list))

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    print(" Download from: {}".format(base_url))
    print(" Download to: {}".format(target_dir.resolve()))
    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            print("Download: {}".format(logfile))
            wget.download(base_url + logfile, str(target_dir))
            print("Done.")

    print("Finished downloading")
