
def load_data(type:str=None):
    from . import __get_file
    base = 'https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_image/'
    # taken from /vol/repl261-vol4/naoth/logs/2019-11-21_Koeln/
    # 2019-11-21_16-20-00_Berlin United_vs_Nao_Devils_Dortmund_half1/game_logs/1_96_Nao0377_191122-0148
    files = {
        'image': ('test.png', '97a053f4859e2850ca768f8e6f82d35a'),
        'image_log': ('images.log', '9550c35fc831d99d031455fd305d7494'),
        'game_log': ('game.log', '2e088b23039c5d83b70dabea8b658390'),
        'ball': ('rc17_ball_far.log', 'e85f5ed1ea5777aaf01049895481be70'),
    }

    if type:
        return __get_file(files[type][0], file_hash=files[type][1], origin=base, cache_subdir='demo_image')

    return {k: __get_file(v[0], file_hash=v[1], origin=base, cache_subdir='demo_image') for k, v in files.items()}
