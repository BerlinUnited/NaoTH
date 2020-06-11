
def load_data():
    from . import __get_file
    base = 'https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_edgels/'
    file = ('2019-07-05_11-45-00_Berlin United_vs_NomadZ_half2-1_93_Nao0212.log', 'e3f0c5fde010560cafd22c7d1e36cef7')
    return __get_file(file[0], file_hash=file[1], origin=base, cache_subdir='demo_edgels')
