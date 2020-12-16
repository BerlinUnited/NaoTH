try:
    from . import AudioData_pb2
    from . import CommonTypes_pb2
    from . import Framework_Representations_pb2
    from . import Messages_pb2
    from . import Representations_pb2
    from . import TeamMessage_pb2
    from . import RobotPose_pb2
except ImportError as e:
    print("\n\033[91mThe protobuf files can not be found/imported! Please run '\033[1mpremake --protoc\033[0m\033[91m' first.\033[0m\n")
    raise e