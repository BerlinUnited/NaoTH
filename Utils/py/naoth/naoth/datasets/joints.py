
def load_data():
    from . import __get_file
    base = 'https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_jointdata/'
    file = ('collision_goal_left1.log', '23dba98265076edb8951db79e914e501')
    return __get_file(file[0], file_hash=file[1], origin=base, cache_subdir='demo_jointdata')
