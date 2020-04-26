
def __validate_file(file: str, file_hash: str):
    from hashlib import md5
    fhash = md5()
    with open(file, 'rb') as f:
        fb = f.read(65536)
        while len(fb) > 0:
            fhash.update(fb)
            fb = f.read(65536)
    return file_hash == fhash.hexdigest()


def __get_file(name: str,
               file_hash: str = None,
               cache_subdir: str = None,
               origin: str = 'https://www2.informatik.hu-berlin.de/~naoth/ressources/log/'):
    import os
    from urllib.request import urlretrieve
    from urllib.parse import urljoin, quote
    from urllib.error import HTTPError, URLError

    # prepare cache dir
    cache_dir = os.path.join(os.path.expanduser('~'), '.naoth', 'datasets')

    # use subdir?
    if cache_subdir:
        cache_dir = os.path.join(cache_dir, cache_subdir)

    if not os.path.exists(cache_dir):
        os.makedirs(cache_dir)

    file = os.path.join(cache_dir, name)

    # check, if we need to (re-)download the file
    download = False
    if os.path.isfile(file):
        if file_hash is not None and not __validate_file(file, file_hash):
            print('A local file was found, but it seems to be incomplete or outdated, so we will re-download the data.')
            download = True
    else:
        download = True

    # download the actual file
    if download:
        origin_file = urljoin(origin, quote(name))
        print('Downloading data from', origin_file)

        def dl_progress(count, block_size, total_size):
            print('\r', 'Progress: {0:.2%}'.format(min((count * block_size) / total_size, 1.0)), sep='', end='', flush=True)

        error_msg = 'URL fetch failure on {} : {} -- {}'
        try:
            try:
                urlretrieve(origin_file, file, dl_progress)
                print('\nFinished')
            except HTTPError as e:
                raise Exception(error_msg.format(origin_file, e.code, e.reason))
            except URLError as e:
                raise Exception(error_msg.format(origin_file, e.errno, e.reason))
        except (Exception, KeyboardInterrupt):
            if os.path.exists(file):
                os.remove(file)
            raise

    return file


from . import motion
from . import fsr
from . import edgels
from . import joints
from . import images
