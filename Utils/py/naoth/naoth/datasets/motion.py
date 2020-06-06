
def load_data(type: str = None):
    from . import __get_file
    files = {
        'motion': ('walk_on_floor_motion.log', 'a7ba18cb744c05905879381a041a82a1'),
        'cognition': ('walk_on_floor_cognition.log', '58ea2fc18f3721e0c05cf0ba94cf055c')
    }

    if type:
        return __get_file(files[type][0], file_hash=files[type][1], cache_subdir='demo_motion')

    return {k: __get_file(v[0], file_hash=v[1], cache_subdir='demo_motion') for k, v in files.items()}
