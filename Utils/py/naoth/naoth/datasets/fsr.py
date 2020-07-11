
def load_data():
    from . import __get_file
    base = 'https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_fsr/'
    files = [
        ("fallen_motion.log", 'cf94b2af23c5d7cedea45f5cb0916048'),
        ("sit_motion.log", '5fb0e5dc5d7d634b458a38e54ab30bff'),
        ("stand_motion.log", 'c87f91a9cc6fde60365865e38e70867a'),
        ("walk_motion.log", '71f68a978985f3b2df114c6624e3253a')
    ]

    return [ __get_file(f, origin=base, file_hash=h, cache_subdir='demo_fsr') for f, h in files ]
