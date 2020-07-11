from ._experimental_parser import Parser
from ._experimental_reader import Reader


def get_representations(path_to_file):
    my_parser = Parser()
    my_parser.register("MotorJointData", "JointData")  # TODO this should be done automatically
    my_parser.register("ImageTop", "Image")
    my_parser.register("CameraMatrixTop", "CameraMatrix")

    logged_representation = set()

    # iterate over the log and put the representation names in a set
    log = Reader(path_to_file, my_parser)
    for idx, frame in enumerate(log):
        names = frame.get_names()
        for name in names:
            logged_representation.add(name)

    return logged_representation
